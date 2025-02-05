#ifndef CALCPOINTSPOSITIONS_H
#define CALCPOINTSPOSITIONS_H

#include "ImageState.h"
#include "systemState.h"
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



inline cv::Point2f calculateCenter(const cv::Point& p1, const cv::Point& p2, const cv::Point& p3, const cv::Point& p4) {
    try {
        cv::Point2f center;
        center.x = (p1.x + p2.x + p3.x + p4.x) / 4;
        center.y = (p1.y + p2.y + p3.y + p4.y) / 4;
        return center;
    } catch (const std::exception& ex) {
        qDebug() << "Exception caught: " << ex.what();
        return cv::Point2f(0, 0); // Return a default point in case of an exception
    }
}

inline void updatePointPosition(cv::Point2f& point, const QPointF &offset)
{
    // Update the position of the point based on the offset
    point.x += static_cast<float>(offset.x());
    point.y += static_cast<float>(offset.y());

}

inline void initializePoints(ImageState& imageState) {

    imageState.points.TrapTopLeft = cv::Point(100, 300); // point1Trap
    imageState.points.TrapTopRight = cv::Point(500, 300); // point2Trap
    imageState.points.TrapBottomRight = cv::Point(50, 450); // point3Trap
    imageState.points.TrapBottomLeft = cv::Point(500, 450); // point4Trap

    imageState.points.RectTopLeft = cv::Point(450, 250); // point1Rect
    imageState.points.RectTopRight = cv::Point(450, 450); // point2Rect
    imageState.points.RectBottomRight = cv::Point(600, 250); // point3Rect
    imageState.points.RectBottomLeft = cv::Point(600, 450); // point4Rect

    imageState.points.centerRect = calculateCenter(imageState.points.RectTopLeft, imageState.points.RectTopRight, imageState.points.RectBottomRight, imageState.points.RectBottomLeft); // centerRect
    imageState.points.centerTrap = calculateCenter(imageState.points.TrapTopLeft, imageState.points.TrapTopRight, imageState.points.TrapBottomRight, imageState.points.TrapBottomLeft); // centerTrap
}

inline void zoomInOnPoint(const cv::Mat& originalImage, cv::Point selectedPoint, double zoomFactor, QLabel* label, ImageState& imageState)
{
    try {
        // Calculate ROI dimensions
        int roiWidth = originalImage.cols / zoomFactor;
        int roiHeight = originalImage.rows / zoomFactor;

        // Calculate ROI coordinates
        int roiX = std::max(0, selectedPoint.x - roiWidth / 2);
        int roiY = std::max(0, selectedPoint.y - roiHeight / 2);

        // Ensure ROI does not exceed image boundaries
        roiX = std::min(roiX, originalImage.cols - roiWidth);
        roiY = std::min(roiY, originalImage.rows - roiHeight);

        // Create ROI
        cv::Rect roiRect(roiX, roiY, roiWidth, roiHeight);
        cv::Mat zoomedRegion = originalImage(roiRect);

        // Resize the zoomed region to the original size
        cv::resize(zoomedRegion, zoomedRegion, originalImage.size());

        // Display the zoomed image
        QImage zoomedImg = QImage(zoomedRegion.data, zoomedRegion.cols, zoomedRegion.rows, zoomedRegion.step, QImage::Format_RGB888);
        label->setPixmap(QPixmap::fromImage(zoomedImg));
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
    }
}


#endif // CALCPOINTSPOSITIONS_H
