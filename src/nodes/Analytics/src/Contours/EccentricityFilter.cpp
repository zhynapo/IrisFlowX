#include "EccentricityFilter.hpp"
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

const NodeDesc EccentricityFilter::desc = {
    "Eccentricity Filter",
    "EccentricityFilter",
    CAT_ANALY_CONTOURS,
    {
        PortDesc::In("contours", NodeType::Contours)
    },
    {
        PortDesc::Out("filtered_contours", NodeType::Contours)
    },
    {
        ParamDesc::makeDouble("min_eccentricity", "Min Eccentricity", 0.0).range(0.0, 1.0),
        ParamDesc::makeDouble("max_eccentricity", "Max Eccentricity", 1.0).range(0.0, 1.0)
    }
};

EccentricityFilter::EccentricityFilter()
    : BaseNodeModel(desc)
{
}

void EccentricityFilter::process()
{
    // Get contour input
    auto contourData = std::dynamic_pointer_cast<ContoursNodeData>(_getInput(0));
    
    if (!contourData) {
        // 当没有输入数据时，确保清空所有输出并停止进一步处理
        setOutputData(0, nullptr);
        return;
    }
    
    std::vector<std::vector<cv::Point>> inputContours = contourData->value();
    
    if (inputContours.empty()) {
        setOutputData(0, nullptr);
        return;
    }
    
    // Get parameters with clamping to fixed valid range [0, 1]
    double minEccentricity = std::clamp(parameterValue("min_eccentricity").toDouble(), 0.0, 1.0);
    double maxEccentricity = std::clamp(parameterValue("max_eccentricity").toDouble(), 0.0, 1.0);

    // Ensure min <= max
    if (minEccentricity > maxEccentricity) {
        std::swap(minEccentricity, maxEccentricity);
        // Optionally sync corrected values back to UI without triggering update
        setParameter("min_eccentricity", minEccentricity);
        setParameter("max_eccentricity", maxEccentricity);
    }
    
    std::vector<std::vector<cv::Point>> filteredContours;
    
    // Apply eccentricity filter to each contour
    for (const auto& contour : inputContours) {
        bool passesFilter = true;
        
        // Eccentricity filter
        if (passesFilter) {
            if (contour.size() < 5) {
                passesFilter = false; // Need at least 5 points for fitEllipse
            } else {
                cv::RotatedRect ellipse;
                try {
                    ellipse = cv::fitEllipse(contour);
                } catch (const cv::Exception& e) {
                    // Skip contours that can't be fitted to ellipse
                    passesFilter = false;
                }
                
                if (passesFilter) {
                    double semiMajorAxis = std::max(ellipse.size.width, ellipse.size.height) / 2.0;
                    double semiMinorAxis = std::min(ellipse.size.width, ellipse.size.height) / 2.0;
                    
                    double eccentricity = 0.0;
                    if (semiMajorAxis > 0) {
                        double ratio = semiMinorAxis / semiMajorAxis;
                        eccentricity = std::sqrt(1.0 - ratio * ratio);
                    }
                    
                    if (eccentricity < minEccentricity || eccentricity > maxEccentricity) {
                        passesFilter = false;
                    }
                }
            }
        }
        
        if (passesFilter) {
            filteredContours.push_back(contour);
        }
    }
    
    if (filteredContours.empty()) {
        // No contours passed the filter, return null
        setOutputData(0, nullptr);
        return;
    }
    
    // Create contour data with all filtered contours
    auto outputData = std::make_shared<ContoursNodeData>(filteredContours);
    
    // Output the results
    setOutputData(0, outputData);
}

