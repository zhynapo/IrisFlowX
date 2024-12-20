// Common.hpp
#pragma once
#include <QImage>
#include <QPixmap>
#include <opencv2/opencv.hpp>

static QImage MatToQImage(const cv::Mat &mat) {
    if (mat.empty()) return QImage();

    if (mat.type() == CV_8UC1) {
        return QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step),
                      QImage::Format_Grayscale8).copy();
    } else if (mat.type() == CV_8UC3) {
        // OpenCV uses BGR by default -> convert to RGB
        cv::Mat rgb;
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
        return QImage(rgb.data, rgb.cols, rgb.rows, static_cast<int>(rgb.step),
                      QImage::Format_RGB888).copy();
    } else if (mat.type() == CV_8UC4) {
        cv::Mat rgba;
        cv::cvtColor(mat, rgba, cv::COLOR_BGRA2RGBA);
        return QImage(rgba.data, rgba.cols, rgba.rows, static_cast<int>(rgba.step),
                      QImage::Format_RGBA8888).copy();
    }
    // unsupported types -> convert to 8-bit 3-channel
    cv::Mat tmp;
    mat.convertTo(tmp, CV_8UC3);
    cv::cvtColor(tmp, tmp, cv::COLOR_BGR2RGB);
    return QImage(tmp.data, tmp.cols, tmp.rows, static_cast<int>(tmp.step),
                  QImage::Format_RGB888).copy();
}

static QPixmap MatToQPixmap(const cv::Mat &mat) {
    QImage img = MatToQImage(mat);
    return img.isNull() ? QPixmap() : QPixmap::fromImage(img);
}

static cv::Mat QImageToMat(const QImage &img) {
    QImage conv = img.convertToFormat(QImage::Format_RGB888);
    cv::Mat m(conv.height(), conv.width(), CV_8UC3, const_cast<uchar*>(conv.bits()), conv.bytesPerLine());
    cv::Mat bgr;
    cv::cvtColor(m, bgr, cv::COLOR_RGB2BGR);
    return bgr.clone();
}
