#ifndef SCORE_H
#define SCORE_H

#include "ImageState.h"
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
#include "systemState.h"





double calculateIOA(const cv::Mat& binary_scoreImageGTMask, const cv::Mat& binary_mask2) {
    cv::Mat intersection;
    cv::bitwise_and(binary_scoreImageGTMask, binary_mask2, intersection);

    double ioa = static_cast<double>(cv::countNonZero(intersection)) / static_cast<double>(cv::countNonZero(binary_mask2));
    return ioa;
}

double calculateIOU(const cv::Mat& binary_scoreImageGTMask, const cv::Mat& binary_mask2) {
    cv::Mat intersection, union_mask;
    cv::bitwise_and(binary_scoreImageGTMask, binary_mask2, intersection);
    cv::bitwise_or(binary_scoreImageGTMask, binary_mask2, union_mask);

    double iou = static_cast<double>(cv::countNonZero(intersection)) / static_cast<double>(cv::countNonZero(union_mask));
    return iou;
}

void calcScore(ImageState& imageState,SystemState& systemState) {
    cv::resize(imageState.images.scoreImageGTMask,imageState.images.scoreImageGTMask , imageState.images.transformedImageMask.size(), 0, 0, cv::INTER_NEAREST);
    double ioa = calculateIOA(imageState.images.scoreImageGTMask / 255, imageState.images.transformedImageMask / 255);
    double iou = calculateIOU(imageState.images.scoreImageGTMask / 255, imageState.images.transformedImageMask / 255);
    double s = ((0.65 * ioa + 0.35 * iou) * 100);
    imageState.score = s;

    systemState.labels.imgscorelable->setText(QString(" %1").arg(imageState.score));
    if (imageState.score < 85) {
        systemState.labels.imgscorelable->setStyleSheet("background-color: red;");
    } else if (imageState.score < 90 && imageState.score >= 85) {
        systemState.labels.imgscorelable->setStyleSheet("background-color: orange;");
    } else if (imageState.score >= 90) {
        systemState.labels.imgscorelable->setStyleSheet("background-color: green;");
    }
}

void loadscoreImage(int position, ImageState& imageState,QSize labelSize1,SystemState& systemState) {
    try {
        if (position >= 0 && position < systemState.stringData.imageFileList.size()) {
            QString imageName = systemState.stringData.imageFileList[position];
            imageName.replace(".jpg", ".png", Qt::CaseInsensitive);
            QString imagePath = systemState.stringData.selectedFolderPath2 + QDir::separator() + imageName;
            imageState.images.scoreImageGTMask = cv::imread(imagePath.toStdString());

            if (!imageState.images.scoreImageGTMask.empty()) {


                cv::cvtColor(imageState.images.scoreImageGTMask, imageState.images.scoreImageGTMask, cv::COLOR_BGR2GRAY);
                cv::threshold(imageState.images.scoreImageGTMask, imageState.images.scoreImageGTMask, 0, 255, cv::THRESH_BINARY);
               // cv::resize(imageState.images.scoreImageGTMask, imageState.images.scoreImageGTMask, cv::Size(labelSize1.width(), labelSize1.height()));
                calcScore(imageState,systemState);
                if(systemState.flags.load==false){
                  imageState.imgscore = imageState.score;
                }
                else{
                    imageState.imgscore = imageState.imgscore ;

                }

                if (imageState.imgscore < 85) {
                   systemState.labels.inscore->setStyleSheet("background-color: red;");
                } else if (imageState.imgscore < 90 && imageState.imgscore >= 85) {
                    systemState.labels.inscore->setStyleSheet("background-color: orange;");
                } else if (imageState.imgscore >= 90) {
                   systemState.labels.inscore->setStyleSheet("background-color: green;");
                }

                systemState.labels.inscore->setText(QString("%1").arg(imageState.imgscore));

                QImage mask = QImage(imageState.images.scoreImageGTMask.data, imageState.images.scoreImageGTMask.cols, imageState.images.scoreImageGTMask.rows, imageState.images.scoreImageGTMask.step, QImage::Format_Grayscale8);
            } else {
                imageState.images.imageName = imageName;
            }
        }
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
    }
}





#endif // SCORE_H
