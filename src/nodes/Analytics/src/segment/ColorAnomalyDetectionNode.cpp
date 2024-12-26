#include "ColorAnomalyDetectionNode.hpp"
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QJsonObject>
#include <QJsonValue>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <QDebug>

const NodeDesc ColorAnomalyDetectionNode::desc = {
    "Color Anomaly Detection",
    "ColorAnomalyDetectionNode",
    CAT_ANALY_SEG,
    {
        PortDesc::In("image", NodeType::Mat),
        PortDesc::In("background", NodeType::Mat)
    },
    {
        PortDesc::Out("mask", NodeType::Mat)
    },
    {
        ParamDesc::makeInt("border_margin", "Border Margin", 10).range(0, 50),
        ParamDesc::makeInt("blur_ksize", "Blur Kernel Size", 5).range(1, 31),
        ParamDesc::makeDouble("k_sigma", "K Sigma", 2.5).range(0.0, 10.0),
        ParamDesc::makeDouble("thr_min", "Threshold Min", 15.0).range(0.0, 255.0),
        ParamDesc::makeDouble("thr_max", "Threshold Max", 60.0).range(0.0, 255.0),
        ParamDesc::makeInt("morph_size", "Morphology Size", 3).range(1, 30),
        ParamDesc::makeInt("min_area", "Min Area", 50).range(1, 1000),
        ParamDesc::makeDouble("lower_h", "Lower H", 0.0).range(0.0, 179.0),
        ParamDesc::makeDouble("lower_s", "Lower S", 0.0).range(0.0, 255.0),
        ParamDesc::makeDouble("lower_v", "Lower V", 0.0).range(0.0, 255.0),
        ParamDesc::makeDouble("upper_h", "Upper H", 179.0).range(0.0, 179.0),
        ParamDesc::makeDouble("upper_s", "Upper S", 255.0).range(0.0, 255.0),
        ParamDesc::makeDouble("upper_v", "Upper V", 255.0).range(0.0, 255.0)
    }
};

ColorAnomalyDetectionNode::ColorAnomalyDetectionNode()
    : BaseNodeModel(desc)
{
    // 参数默认值已经在NodeDesc定义中设置，不需要在此处重复设置
}

// Helper function to calculate gain and bias for background matching
static void fitGainBiasBGR(const cv::Mat& img, const cv::Mat& bg, const cv::Mat& mask, cv::Vec2f gb[3])
{
    for(int ch = 0; ch < 3; ch++) {
        cv::Mat imgCh, bgCh;
        std::vector<cv::Mat> imgPlanes;
        std::vector<cv::Mat> bgPlanes;
        
        cv::split(img, imgPlanes);
        cv::split(bg, bgPlanes);
        
        imgCh = imgPlanes[ch];
        bgCh = bgPlanes[ch];
        
        cv::Mat validImg, validBg;
        imgCh.copyTo(validImg, mask);
        bgCh.copyTo(validBg, mask);
        
        cv::Mat nonZeroMask = (validBg != 0);
        cv::Mat ratio;
        cv::divide(validImg, validBg + cv::Scalar(1e-6), ratio);
        
        cv::Mat validRatios;
        ratio.copyTo(validRatios, nonZeroMask);
        
        cv::Scalar meanVal = cv::mean(validRatios, nonZeroMask);
        
        gb[ch][0] = static_cast<float>(meanVal[0]);  // gain
        gb[ch][1] = 0.0f;                           // bias
    }
}

// Helper function to apply gain and bias
static cv::Mat applyGainBias(const cv::Mat& bg, cv::Vec2f gb[3])
{
    std::vector<cv::Mat> planes;
    cv::split(bg, planes);
    
    for(int ch = 0; ch < 3; ch++) {
        planes[ch] = planes[ch] * gb[ch][0] + gb[ch][1];
    }
    
    cv::Mat result;
    cv::merge(planes, result);
    return result;
}

// Helper function to calculate Delta E (CIE76)
static cv::Mat deltaE76_BGR(const cv::Mat& img1, const cv::Mat& img2, int type)
{
    CV_Assert(img1.type() == CV_8UC3 && img2.type() == CV_8UC3);
    cv::Mat aLab8, bLab8;
    cv::cvtColor(img1, aLab8, cv::COLOR_BGR2Lab);
    cv::cvtColor(img2, bLab8, cv::COLOR_BGR2Lab);

    aLab8.convertTo(aLab8, CV_32F);
    bLab8.convertTo(bLab8, CV_32F);

    std::vector<cv::Mat> ac, bc; cv::split(aLab8, ac); cv::split(bLab8, bc);
    // OpenCV 的 Lab8：L∈[0,255]≈(L*∈[0,100]×255/100)，a,b 带 +128 偏移
    cv::Mat diff;
    if (type==0)
    {
        cv::Mat dL = ac[0] - bc[0];
        cv::max(dL, 0, dL);   // 只保留"变亮"

        cv::Mat da = ac[1] - bc[1];
        cv::Mat db = ac[2] - bc[2];

        diff = dL.mul(dL) + da.mul(da) + db.mul(db);
        cv::sqrt(diff, diff);
        return diff; // CV_32F
    }else 
    if (type==2)
    {
        cv::Mat dL = (ac[0] - bc[0]);// *(100.0 / 255.0); // 把 L* 差值还原到 0..100 量纲
        cv::Mat da = (ac[1] - bc[1]);                 // a*,b* 的 +128 偏移相减会抵消
        cv::Mat db = (ac[2] - bc[2]);
        cv::magnitude(dL, da, diff); cv::magnitude(diff, db, diff);
    }
    return diff;
}

void ColorAnomalyDetectionNode::process()
{
    // Get image inputs
    auto imgData = std::dynamic_pointer_cast<MatNodeData>(_getInput(0));
    auto bgData = std::dynamic_pointer_cast<MatNodeData>(_getInput(1));
    
    if (!imgData || !bgData || imgData->mat().empty() || bgData->mat().empty()) {
        setOutputData(0, nullptr);
        return;
    }
    
    cv::Mat imgBGR = imgData->mat();
    cv::Mat bgBGR = bgData->mat();
    
    if (imgBGR.size() != bgBGR.size()) {
        qWarning() << "Image and background sizes do not match!";
        setOutputData(0, nullptr);
        return;
    }
    
    // Get parameters
    _borderMargin = parameterValue("border_margin").toInt();
    _blurKSize = parameterValue("blur_ksize").toInt();
    if (_blurKSize % 2 == 0) _blurKSize++; // Ensure odd kernel size
    
    _kSigma = parameterValue("k_sigma").toDouble();
    _thrMin = parameterValue("thr_min").toDouble();
    _thrMax = parameterValue("thr_max").toDouble();
    _morphSize = parameterValue("morph_size").toInt();
    _minArea = parameterValue("min_area").toInt();
    
    _lowerH = parameterValue("lower_h").toDouble();
    _lowerS = parameterValue("lower_s").toDouble();
    _lowerV = parameterValue("lower_v").toDouble();
    _upperH = parameterValue("upper_h").toDouble();
    _upperS = parameterValue("upper_s").toDouble();
    _upperV = parameterValue("upper_v").toDouble();
    
    // Create HSV range scalars
    cv::Scalar lowerHSV(_lowerH, _lowerS, _lowerV);
    cv::Scalar upperHSV(_upperH, _upperS, _upperV);
    
    // Step 1: Outer ring mask
    cv::Mat ring = cv::Mat::zeros(imgBGR.size(), CV_8UC1);
    cv::rectangle(ring,
                  cv::Rect(_borderMargin, _borderMargin,
                          imgBGR.cols - 2 * _borderMargin,
                          imgBGR.rows - 2 * _borderMargin),
                  cv::Scalar(255), cv::FILLED);
    cv::bitwise_not(ring, ring);

    // Step 2: Blur
    cv::Mat imgBlur, bgBlur;
    cv::GaussianBlur(imgBGR, imgBlur, cv::Size(_blurKSize, _blurKSize), 0);
    cv::GaussianBlur(bgBGR, bgBlur, cv::Size(_blurKSize, _blurKSize), 0);

    // Step 3: Background brightness matching
    cv::Vec2f gb[3];
    fitGainBiasBGR(imgBlur, bgBlur, ring, gb);
    cv::Mat bgMatched = applyGainBias(bgBlur, gb);

    // Step 4: ΔE calculation
    cv::Mat dE = deltaE76_BGR(imgBlur, bgMatched, 2);

    cv::Mat diff8;
    float maxDiff = 80.0f;

    // Clamp to prevent extreme values
    cv::Mat diffClipped;
    cv::min(dE, maxDiff, diffClipped);

    // Linear mapping to [0,255]
    diffClipped.convertTo(diff8, CV_8UC1, 255.0 / maxDiff);

    // Step 5: Thresholding
    cv::Mat anomalyMask;
    double thr = cv::threshold(diff8, anomalyMask, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    thr = (thr + 3*_thrMin)/4;
    cv::threshold(diff8, anomalyMask, thr, 255, cv::THRESH_BINARY);
    
    // NEW: Apply color filtering using HSV range
    cv::Mat imgHSV;
    cv::cvtColor(imgBGR, imgHSV, cv::COLOR_BGR2HSV);
    cv::Mat colorMask;
    cv::inRange(imgHSV, lowerHSV, upperHSV, colorMask);
    
    // Combine anomaly detection with color filtering
    cv::Mat finalMask;
    cv::bitwise_and(anomalyMask, colorMask, finalMask);

    // Step 6: Morphological operations
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(_morphSize, _morphSize));
    cv::morphologyEx(finalMask, finalMask, cv::MORPH_OPEN, kernel);

    // Step 7: Small area filtering
    cv::Mat labels, stats, centroids;
    int n = cv::connectedComponentsWithStats(finalMask, labels, stats, centroids);

    cv::Mat clean = cv::Mat::zeros(finalMask.size(), CV_8UC1);
    for (int i = 1; i < n; ++i) {
        int area = stats.at<int>(i, cv::CC_STAT_AREA);
        if (area >= _minArea)
            clean.setTo(255, labels == i);
    }
    
    // Additional closing operation
    int closeSize = static_cast<int>(_morphSize * 1.5);
    if (closeSize % 2 == 0) closeSize += 1;
    kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(closeSize, closeSize));
    cv::morphologyEx(clean, clean, cv::MORPH_CLOSE, kernel);

    // Output the result
    setOutputData(0, std::make_shared<MatNodeData>(clean));
}