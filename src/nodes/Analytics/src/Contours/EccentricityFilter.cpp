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
        // Boolean parameters to enable/disable filters
        ParamDesc::makeBool("enable_eccentricity", "Enable Eccentricity Filter", false),
        ParamDesc::makeDouble("min_eccentricity", "Min Eccentricity", 0.0).range(0.0, 1.0),
        ParamDesc::makeDouble("max_eccentricity", "Max Eccentricity", 1.0).range(0.0, 1.0),
        
        // Index parameter for selecting from filtered results
        ParamDesc::makeInt("index", "Contour Index", 0).range(0, 100),
        
        // Parameters to display contour metrics using labels
        ParamDesc::makeLabel("eccentricity_label", "Eccentricity", "0.0")
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
        setOutputData(0, nullptr);
        setParameter("eccentricity_label", "Eccentricity: 0.000");
        return;
    }
    
    std::vector<std::vector<cv::Point>> inputContours = contourData->value();
    
    if (inputContours.empty()) {
        setOutputData(0, nullptr);
        setParameter("eccentricity_label", "Eccentricity: 0.000");
        return;
    }
    
    // Get parameters
    bool enableEccentricity = parameterValue("enable_eccentricity").toBool();
    double minEccentricity = parameterValue("min_eccentricity").toDouble();
    double maxEccentricity = parameterValue("max_eccentricity").toDouble();
    
    int index = parameterValue("index").toInt();
    
    std::vector<std::vector<cv::Point>> filteredContours;
    
    // Apply filters to each contour
    for (const auto& contour : inputContours) {
        bool passesAllFilters = true;
        
        // Eccentricity filter
        if (passesAllFilters && enableEccentricity) {
            if (contour.size() < 5) {
                passesAllFilters = false; // Need at least 5 points for fitEllipse
            } else {
                cv::RotatedRect ellipse = cv::fitEllipse(contour);
                double semiMajorAxis = std::max(ellipse.size.width, ellipse.size.height) / 2.0;
                double semiMinorAxis = std::min(ellipse.size.width, ellipse.size.height) / 2.0;
                
                double eccentricity = 0.0;
                if (semiMajorAxis > semiMinorAxis) {
                    eccentricity = sqrt(1 - (semiMinorAxis * semiMinorAxis) / (semiMajorAxis * semiMajorAxis));
                }
                
                if (eccentricity < minEccentricity || eccentricity > maxEccentricity) {
                    passesAllFilters = false;
                }
            }
        }
        
        if (passesAllFilters) {
            filteredContours.push_back(contour);
        }
    }
    
    // Update parameter ranges based on filtered contour count
    int n = filteredContours.size();
    setParamRange("index", 0, n - 1);
    setParamEnabled("index", n > 0);
    
    std::vector<std::vector<cv::Point>> outputContours;
    
    if (filteredContours.empty()) {
        // No contours passed all filters, return null
        setOutputData(0, nullptr);
        setParameter("eccentricity_label", "Eccentricity: 0.000");
        return;
    }
    
    std::vector<cv::Point> selectedContour;
    if (index >= 0 && index < n) {
        selectedContour = filteredContours[index];
        outputContours.push_back(selectedContour);
    } else {
        // If index is out of range, default to the first contour
        selectedContour = filteredContours[0];
        outputContours.push_back(selectedContour);
    }
    
    // Calculate metric for the selected contour
    double eccentricity = 0.0;
    if (selectedContour.size() >= 5) {
        cv::RotatedRect ellipse = cv::fitEllipse(selectedContour);
        double semiMajorAxis = std::max(ellipse.size.width, ellipse.size.height) / 2.0;
        double semiMinorAxis = std::min(ellipse.size.width, ellipse.size.height) / 2.0;
        
        if (semiMajorAxis > semiMinorAxis) {
            eccentricity = sqrt(1 - (semiMinorAxis * semiMinorAxis) / (semiMajorAxis * semiMajorAxis));
        }
    }
    
    setParameter("eccentricity_label", QString("Eccentricity: %1").arg(eccentricity, 0, 'f', 3));
    
    // Create contour data
    auto outputData = std::make_shared<ContoursNodeData>(outputContours);
    
    // Output the results
    setOutputData(0, outputData);
}