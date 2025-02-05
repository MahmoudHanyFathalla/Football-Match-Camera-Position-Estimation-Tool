#ifndef DRAW_H
#define DRAW_H

#include "calcPointsPositions.h"
#include "ui_mainwindow.h"
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
#include "mainwindow.h"
#include "txtOperations.h"
#include "score.h"
#include "matrix.h"


void drawShapes(cv::Mat& image, const cv::Point& p1, const cv::Point& p2, const cv::Point& p3, const cv::Point& p4, const cv::Point& pc, SelectedPoint selectedPoint) {
    cv::circle(image, p1, 10, cv::Scalar(255, 0, 0), 1);
    cv::circle(image, p2, 10, cv::Scalar(255, 0, 0), 1);
    cv::circle(image, p3, 10, cv::Scalar(255, 0, 0), 1);
    cv::circle(image, p4, 10, cv::Scalar(255, 0, 0), 1);
    cv::circle(image, pc, 10, cv::Scalar(0, 255, 255), 2);

    cv::line(image, p1, p2, cv::Scalar(200, 200, 0), 1);
    cv::line(image, p1, p3, cv::Scalar(200, 200, 0), 1);
    cv::line(image, p2, p4, cv::Scalar(200, 200, 0), 1);
    cv::line(image, p4, p3, cv::Scalar(200, 200, 0), 1);

    if (selectedPoint == SelectedPoint::TrapTopLeftSelected) cv::circle(image, p1, 13, cv::Scalar(0, 0, 255), 1);
    if (selectedPoint == SelectedPoint::TrapTopRightSelected) cv::circle(image, p2, 13, cv::Scalar(0, 0, 255), 1);
    if (selectedPoint == SelectedPoint::TrapBottomRightSelected) cv::circle(image, p3, 13, cv::Scalar(0, 0, 255), 1);
    if (selectedPoint == SelectedPoint::TrapBottomLeftSelected) cv::circle(image, p4, 13, cv::Scalar(0, 0, 255), 1);
    if (selectedPoint == SelectedPoint::centerTrapSelected) cv::circle(image, pc, 13, cv::Scalar(0, 0, 255), 2);

}

void drawShapesRect(cv::Mat& image, const cv::Point& p1, const cv::Point& p2, const cv::Point& p3, const cv::Point& p4, const cv::Point& pc, SelectedPoint selectedPoint) {
    cv::circle(image, p1, 10, cv::Scalar(255, 0, 0), 1);
    cv::circle(image, p2, 10, cv::Scalar(255, 0, 0), 1);
    cv::circle(image, p3, 10, cv::Scalar(255, 0, 0), 1);
    cv::circle(image, p4, 10, cv::Scalar(255, 0, 0), 1);
    cv::circle(image, pc, 10, cv::Scalar(0, 255, 255), 2);

    cv::line(image, p1, p2, cv::Scalar(200, 200, 0), 1);
    cv::line(image, p1, p3, cv::Scalar(200, 200, 0), 1);
    cv::line(image, p2, p4, cv::Scalar(200, 200, 0), 1);
    cv::line(image, p4, p3, cv::Scalar(200, 200, 0), 1);


    if (selectedPoint == SelectedPoint::RectTopLeftSelected) cv::circle(image, p1, 13, cv::Scalar(0, 0, 255), 1);
    if (selectedPoint == SelectedPoint::RectTopRightSelected) cv::circle(image, p2, 13, cv::Scalar(0, 0, 255), 1);
    if (selectedPoint == SelectedPoint::RectBottomRightSelected) cv::circle(image, p3, 13, cv::Scalar(0, 0, 255), 1);
    if (selectedPoint == SelectedPoint::RectBottomLeftSelected) cv::circle(image, p4, 13, cv::Scalar(0, 0, 255), 1);
    if (selectedPoint == SelectedPoint::centerRectSelected) cv::circle(image, pc, 13, cv::Scalar(0, 0, 255), 2);
}

void MainWindow::drawImageWithPoints(ImageState& imageState) {
    try {


        // Reset the modified image to the original image
        imageState.images.modifiedImageTrap = imageState.images.originalImageTrap.clone();

        // Draw circles and lines using helper function
        drawShapes(imageState.images.modifiedImageTrap,
                   imageState.points.TrapTopLeft, // point1Trap
                   imageState.points.TrapTopRight, // point2Trap
                   imageState.points.TrapBottomRight, // point3Trap
                   imageState.points.TrapBottomLeft, // point4Trap
                   imageState.points.centerTrap, // centerTrap
                    imageState.selectedPoint);

        // Convert BGR to RGB for OpenCV images
        cv::cvtColor(imageState.images.modifiedImageTrap, imageState.images.modifiedImageTrap, cv::COLOR_BGR2RGB);

        // Convert OpenCV Mat to QImage for the modified image
        QImage modifiedImg = QImage(imageState.images.modifiedImageTrap.data,
                                    imageState.images.modifiedImageTrap.cols,
                                    imageState.images.modifiedImageTrap.rows,
                                    imageState.images.modifiedImageTrap.step,
                                    QImage::Format_RGB888);

        // Display the modified image in the QLabel
        cv::Point zoomCenter;
        if ((imageState.selectedPoint == SelectedPoint::TrapTopLeftSelected ||
             imageState.selectedPoint == SelectedPoint::TrapTopRightSelected ||
             imageState.selectedPoint == SelectedPoint::TrapBottomRightSelected ||
             imageState.selectedPoint == SelectedPoint::TrapBottomLeftSelected ||
             imageState.selectedPoint == SelectedPoint::centerTrapSelected) &&
            systemState.flags.zoomed) {
            if (imageState.selectedPoint == SelectedPoint::TrapTopLeftSelected) zoomCenter = imageState.points.TrapTopLeft;
            else if (imageState.selectedPoint == SelectedPoint::TrapTopRightSelected) zoomCenter = imageState.points.TrapTopRight;
            else if (imageState.selectedPoint == SelectedPoint::TrapBottomRightSelected) zoomCenter = imageState.points.TrapBottomRight;
            else if (imageState.selectedPoint == SelectedPoint::TrapBottomLeftSelected) zoomCenter = imageState.points.TrapBottomLeft;
            else if (imageState.selectedPoint == SelectedPoint::centerTrapSelected) zoomCenter = imageState.points.centerTrap;


            ui->label->setPixmap(QPixmap::fromImage(modifiedImg));
            zoomInOnPoint(imageState.images.modifiedImageTrap, zoomCenter, systemState.ratios.zoomratio, ui->label, imageState);
        }
        else {
            ui->label->setPixmap(QPixmap::fromImage(modifiedImg));
        }

        // Bring controlWindow to front if already open
        if (!systemState.pointers.RealW || systemState.pointers.RealW.isNull()) {
            // If not, create a new instance
        } else {
            on_pushButton_34_clicked();
        }
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::drawImageWithPoints2(ImageState& imageState) {
    try {
        // Reset the modified image to the original image for label_2
        imageState.images.modifiedImageRect = imageState.images.originalImageRect.clone();

        // Draw circles and lines using helper function for label_2
        drawShapesRect(imageState.images.modifiedImageRect,
                   imageState.points.RectTopLeft,
                   imageState.points.RectTopRight,
                   imageState.points.RectBottomRight,
                   imageState.points.RectBottomLeft,
                   imageState.points.centerRect,
                    imageState.selectedPoint);

        // Convert BGR to RGB for OpenCV images
        cv::cvtColor(imageState.images.modifiedImageRect, imageState.images.modifiedImageRect, cv::COLOR_BGR2RGB);

        // Convert OpenCV Mat to QImage for the modified image for label_2
        QImage modifiedImg2 = QImage(imageState.images.modifiedImageRect.data,
                                     imageState.images.modifiedImageRect.cols,
                                     imageState.images.modifiedImageRect.rows,
                                     imageState.images.modifiedImageRect.step,
                                     QImage::Format_RGB888);

        // Display the modified image in the QLabel for label_2
        ui->label_2->setPixmap(QPixmap::fromImage(modifiedImg2));

        // Bring controlWindow to front if already open
        if (!systemState.pointers.PlayGroundW || systemState.pointers.PlayGroundW.isNull()) {
            // If not, create a new instance
        } else {
            on_pushButton_35_clicked();
        }
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::drawPerspctive(ImageState& imageState){


    // Convert OpenCV Mat to QImage for the blended image
    QImage blendedImg = QImage(imageState.images.blendedImageLeft.data, imageState.images.blendedImageLeft.cols, imageState.images.blendedImageLeft.rows, imageState.images.blendedImageLeft.step, QImage::Format_RGB888);
    QImage blendedImg2 = QImage(imageState.images.blendedImageRight.data, imageState.images.blendedImageRight.cols, imageState.images.blendedImageRight.rows, imageState.images.blendedImageRight.step, QImage::Format_RGB888);

    // Display the blended image in QLabel for label_3
    ui->label_3->setPixmap(QPixmap::fromImage(blendedImg));
    ui->label_4->setPixmap(QPixmap::fromImage(blendedImg2));

    //cv::cvtColor(image1, image1, cv::COLOR_BGR2GRAY);

    if (!systemState.pointers.RealToPredW || systemState.pointers.RealToPredW.isNull()) {


    } else {

        on_pushButton_36_clicked();
    }
    if (!systemState.pointers.PredToRealW || systemState.pointers.PredToRealW.isNull()) {

    } else {

        on_pushButton_37_clicked();
    }

    calcScore(imageState,systemState);
}

void MainWindow::loadAndDisplayImage(int position, ImageState& imageState,SystemState& systemState)
{
    try {
        if (position >= 0 && position < systemState.stringData.imageFileList.size()) {
            QString imageName = systemState.stringData.imageFileList[position];
            QString imagePath = systemState.stringData.selectedFolderPath + QDir::separator() + imageName;

            cv::Mat image = cv::imread(imagePath.toStdString());

            if (!image.empty()) {
                QSize labelSize1 = ui->label->size();

              //  cv::resize(image, image, cv::Size(labelSize1.width(), labelSize1.height()));

                imageState.images.originalImageTrap = image.clone();
                imageState.images.modifiedImageTrap = imageState.images.originalImageTrap.clone();
                ui->label_21->setText(QString("%1/%2").arg(position + 1).arg(systemState.stringData.imageFileList.size()));
                ui->label_22->setText(imageName);

                // Update the imageName variable
                imageState.images.imageName = imageName;
                imageState.VerticalAngleT = 0;

                // Get the size of the label
                systemState.stringData.labelSize = ui->label->size();
                systemState.stringData.label2Size = ui->label_2->size();
                loadPointsFromTextFile(systemState.stringData.selectedFolderPath, imageState,systemState);
                imageState.points.centerRect = calculateCenter(imageState.points.RectTopLeft, imageState.points.RectTopRight, imageState.points.RectBottomRight, imageState.points.RectBottomLeft); // centerRect
                imageState.points.centerTrap = calculateCenter(imageState.points.TrapTopLeft, imageState.points.TrapTopRight, imageState.points.TrapBottomRight, imageState.points.TrapBottomLeft); // centerTrap

                drawImageWithPoints2(imageState);
                drawImageWithPoints(imageState);

                applyPerspectiveTransformation(imageState.images.originalImageTrap, imageState.images.originalImageRect, imageState,systemState);
                drawPerspctive(imageState);

                systemState.stringData.labelSize1 = ui->label->size();
                loadscoreImage(position, imageState, systemState.stringData.labelSize1,systemState);
            } else {
                imageState.images.imageName = imageName;
                qDebug() << "Failed to load image: " << imagePath;
                // Get the size of the label
                systemState.stringData.labelSize = ui->label->size();
                systemState.stringData.label2Size = ui->label_2->size();
                loadPointsFromTextFile(systemState.stringData.selectedFolderPath, imageState,systemState);
                drawImageWithPoints2(imageState);
                drawImageWithPoints(imageState);
            }
        }
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
    }
}






#endif // DRAW_H
