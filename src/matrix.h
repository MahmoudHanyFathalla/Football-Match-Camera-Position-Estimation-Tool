#ifndef MATRIX_H
#define MATRIX_H

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
#include"calcPointsPositions.h"
#include"ImageState.h"
#include "systemState.h"



std::pair<cv::Mat, cv::Mat> calculatePerspectiveMatrix(ImageState& imageState,SystemState& systemState)
{
    // Define source and destination points using data from imageState struct

    cv::Point2f destPoints[4] = {
        imageState.points.TrapTopLeft, // point1Trap
        imageState.points.TrapTopRight, // point2Trap
        imageState.points.TrapBottomRight, // point3Trap
        imageState.points.TrapBottomLeft // point4Trap
    };

    cv::Point2f srcPoints[4] = {
        imageState.points.RectTopLeft, // point1Rect
        imageState.points.RectBottomRight, // point3Rect
        imageState.points.RectTopRight, // point2Rect
        imageState.points.RectBottomLeft // point4Rect
    };

    // Update rotation angle based on flags from imageState struct
    if (systemState.flags.VerticalRotationToLeft) {
        imageState.VerticalAngleT += systemState.ratios.rotatingratio;
    }
    if (systemState.flags.VerticalRotationToRight) {
        imageState.VerticalAngleT -= systemState.ratios.rotatingratio;
    }
    if (systemState.flags.HorizontalRotationToLeft) {
        imageState.HorizontalAngleR -= systemState.ratios.rectratio;
    }
    if (systemState.flags.HorizontalRotationToRight) {
        imageState.HorizontalAngleR += systemState.ratios.rectratio;
    }

    // Calculate center points
    cv::Point2f centerr = calculateCenter(
        imageState.points.TrapTopLeft, // point1Trap
        imageState.points.TrapTopRight, // point2Trap
        imageState.points.TrapBottomRight, // point3Trap
        imageState.points.TrapBottomLeft // point4Trap
        );

    cv::Point2f centerrr = calculateCenter(
        imageState.points.RectTopLeft, // point1Rect
        imageState.points.RectTopRight, // point2Rect
        imageState.points.RectBottomRight, // point3Rect
        imageState.points.RectBottomLeft // point4Rect
        );



    // Convert rotation angles to radians
    double angleInRadians = imageState.VerticalAngleT * CV_PI / 180.0;
    double a = imageState.HorizontalAngleR * CV_PI / 180.0;

    // Apply rotation to destination points
    for (int i = 0; i < 4; ++i) {
        cv::Point2f temp = destPoints[i] - centerr;
        destPoints[i].x = temp.x * cos(angleInRadians) - temp.y * sin(angleInRadians) + centerr.x;
        destPoints[i].y = temp.x * sin(angleInRadians) + temp.y * cos(angleInRadians) + centerr.y;
    }

    // Apply rotation to source points
    for (int i = 0; i < 4; ++i) {
        cv::Point2f tempo = srcPoints[i] - centerrr;
        srcPoints[i].x = tempo.x * cos(a) - tempo.y * sin(a) + centerrr.x;
        srcPoints[i].y = tempo.x * sin(a) + tempo.y * cos(a) + centerrr.y;
    }

    // Calculate the forward perspective transformation matrix
    cv::Mat forwardPerspectiveMatrix = cv::getPerspectiveTransform(srcPoints, destPoints);

    // Calculate the inverse perspective transformation matrix
    cv::Mat inversePerspectiveMatrix = cv::getPerspectiveTransform(destPoints, srcPoints);

    return std::make_pair(forwardPerspectiveMatrix, inversePerspectiveMatrix);
}


void applyPerspectiveTransformation(const cv::Mat& image1, const cv::Mat& image2, ImageState& imageState,SystemState& systemState)
{
    // Calculate the perspective transformation matrices
    auto perspectiveMatrices = calculatePerspectiveMatrix(imageState,systemState);
    cv::Mat forwardPerspectiveMatrix = perspectiveMatrices.first;
    cv::Mat inversePerspectiveMatrix = perspectiveMatrices.second;

    // Create a copy of the original images for label_3 and label_4
    cv::Mat transformedImage2_forward = image2.clone();
    cv::Mat transformedImage2_inverse = image1.clone();

    // Apply forward perspective transformation to image2 using image1.size() as the output size
    cv::warpPerspective(image2, transformedImage2_forward, forwardPerspectiveMatrix, image1.size());

    // Apply inverse perspective transformation to image1 using image1.size() as the output size
    cv::warpPerspective(image1, transformedImage2_inverse, inversePerspectiveMatrix, image2.size());

    imageState.images.transformedImageMask = transformedImage2_forward.clone();

    cv::cvtColor(imageState.images.transformedImageMask, imageState.images.transformedImageMask, cv::COLOR_BGR2GRAY);

    QImage mask = QImage(imageState.images.transformedImageMask.data, imageState.images.transformedImageMask.cols, imageState.images.transformedImageMask.rows, imageState.images.transformedImageMask.step, QImage::Format_Grayscale8);

    // Blend the two forward and inverse transformed images using equal weights
    imageState.images.blendedImageLeft = 0.5 * image1 + 0.5 * transformedImage2_forward; //  hta5od dy : image1
    imageState.images.blendedImageRight = 0.5 * image2 + 0.5 * transformedImage2_inverse;
    // Convert BGR to RGB for OpenCV images
    cv::cvtColor(imageState.images.blendedImageLeft, imageState.images.blendedImageLeft, cv::COLOR_BGR2RGB);
    cv::cvtColor(imageState.images.blendedImageRight, imageState.images.blendedImageRight, cv::COLOR_BGR2RGB);

}



#endif // MATRIX_H


