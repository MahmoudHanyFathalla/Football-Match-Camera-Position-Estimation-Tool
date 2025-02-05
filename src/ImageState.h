#ifndef IMAGESTATE_H
#define IMAGESTATE_H
// TODO :: ImageState
#include <opencv2/core.hpp>
#include <QString>
#include <QStringList>
#include <QPointer>
#include "real.h"
#include "realToPred.h"
#include "predToReal.h"
#include "playGround.h"
#include "qlabel.h"
#include <QMainWindow>
#include <QMouseEvent>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <QFileDialog>
#include <QDir>
#include <QFileInfoList>
#include <QStringList>
#include <QPointer>

// TODO :: create struct point -> float x , float y

// Struct for storing points
struct Points {
    cv::Point2f TrapTopLeft; // point1Trap
    cv::Point2f TrapTopRight; // point2Trap
    cv::Point2f TrapBottomRight; // point3Trap
    cv::Point2f TrapBottomLeft; // point4Trap
    cv::Point2f centerTrap; // centerTrap

    cv::Point2f RectTopLeft; // point1Rect
    cv::Point2f RectTopRight; // point2Rect
    cv::Point2f RectBottomRight; // point3Rect
    cv::Point2f RectBottomLeft; // point4Rect
    cv::Point2f centerRect; // centerRect

   // QString hash;
};


// Struct for storing image-related data
struct Images {
    // GT image path
    // Original
    // Blended left
    // Blended Right

    // remove the rest ...
    cv::Mat transformedImageMask;
    cv::Mat scoreImageGTMask;

    cv::Mat blendedImageLeft;  // blendedImage
    cv::Mat blendedImageRight; // blendedImage2
    cv::Mat originalImageTrap; // originalImage
    cv::Mat originalImageRect; // originalImage2

    cv::Mat modifiedImageTrap; // modifiedImage
    cv::Mat modifiedImageRect; // modifiedImage2

    QString imageName;
    QString scoreImgsName;
    QString imgFolderName;



};

// create new header file -> system state
// remove it or move it outside this context to SystemState.h

// create enum point types : NONE, TRAP_TOP_LEFT, TRAP_TOP_RIGHT,

// move most of them to SystemState.h
// any point here remove it
// Struct for storing flags

enum class SelectedPoint {
    NoneSelected,
    TrapTopLeftSelected,
    TrapTopRightSelected,
    TrapBottomRightSelected,
    TrapBottomLeftSelected,
    centerTrapSelected,

    RectTopLeftSelected,
    RectTopRightSelected,
    RectBottomRightSelected,
    RectBottomLeftSelected,
    centerRectSelected
};


// movementsPrecisions
// -> translation delta
// -> rotation delta
// -> zoom level

// remove the whole struct
// Struct for storing ratios

// remove this
// Struct for storing labels

// remove this
// Struct for storing pointers

// remove it
// Main constraints structure
struct ImageState {
    Points points;
    Images images;
    SelectedPoint selectedPoint = SelectedPoint::NoneSelected;
    double VerticalAngleT = 0.0;
    double HorizontalAngleR = 0.0;
    double score =0;
    double imgscore = 0;




};

// ImageState struct
// -> points
// -> Images
// -> PointType selectedPoint;
// -> rotation angle raw
// -> ration angle roll
// -> machine score
// -> human score


#endif // IMAGESTATE_H
