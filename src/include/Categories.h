#pragma once
#include <QString>

// ---------------------------------------------
// 一级分类
// ---------------------------------------------
static const QString CAT_PROCESSING = "Processing";
static const QString CAT_ANALYTICS  = "Analytics";
static const QString CAT_REASONING  = "Reasoning";
static const QString CAT_UTILITIES  = "Utilities";


// =============================================
// Processing 子分类
// =============================================

// ---- IO ----
static const QString CAT_PROC_IO =
    "Processing/IO";

// ---- Basic Operations ----
static const QString CAT_PROC_BASIC =
    "Processing/BasicOps";

// ---- Filtering ----
static const QString CAT_PROC_FILTERING =
    "Processing/Filtering";

static const QString CAT_PROC_FILTER_SMOOTH =
    "Processing/Filtering/Smooth";

static const QString CAT_PROC_FILTER_SHARPEN =
    "Processing/Filtering/Sharpen";

// ---- Morphology ----
static const QString CAT_PROC_MORPH =
    "Processing/Morphology";

// ---- Color ----
static const QString CAT_PROC_COLOR =
    "Processing/Color";

// ---- Geometry / Warp ----
static const QString CAT_PROC_GEOMETRY =
    "Processing/Geometry";

// ---- Drawing ----
static const QString CAT_PROC_DRAW =
    "Processing/Drawing";



// =============================================
// Analytics 子分类
// =============================================

// ---- Thresholding ----
static const QString CAT_ANALY_THRESHOLD =
    "Analytics/Thresholding";

// ---- Contours ----
static const QString CAT_ANALY_CONTOURS =
    "Analytics/Contours";

// ---- Segmentation ----
static const QString CAT_ANALY_SEG =
    "Analytics/Segmentation";

// ---- Edge Detection ----
static const QString CAT_ANALY_EDGES =
    "Analytics/Detect/Edges";

// ---- General Detection ----
static const QString CAT_ANALY_DETECT =
    "Analytics/Detect";

// ---- Feature Detection ----
static const QString CAT_ANALY_FEATURES =
    "Analytics/Features";

// ---- Descriptors ----
static const QString CAT_ANALY_DESCRIPTORS =
    "Analytics/Descriptors";

// ---- Matching ----
static const QString CAT_ANALY_MATCHING =
    "Analytics/Matching";

// ---- Object Detection ----
static const QString CAT_ANALY_OBJECT =
    "Analytics/ObjectDetection";

// ---- AI / DNN ----
static const QString CAT_ANALY_AI =
    "Analytics/AI";

// ---- YOLO ----
static const QString CAT_ANALY_YOLO =
    "Analytics/AI/YOLO";

// =============================================
// Reasoning 子分类
// =============================================


// =============================================
// Utilities 子分类
// =============================================

// ---- Info & Metadata ----
static const QString CAT_UTIL_INFO =
    "Utilities/Info";

// ---- Conversion ----
static const QString CAT_UTIL_CONVERT =
    "Utilities/Conversion";

// ---- Debug ----
static const QString CAT_UTIL_DEBUG =
    "Utilities/Debug";

// ---- Timing ----
static const QString CAT_UTIL_TIME =
    "Utilities/Timing";