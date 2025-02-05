#ifndef RATIO_H
#define RATIO_H

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

#include "matrix.h"
#include "score.h"
#include "systemState.h"
#include "draw.h"

void updateRotatingRatio(ImageState &imageState, float change,SystemState& systemState) {
    systemState.ratios.rotatingratio += change;
    if (systemState.ratios.rotatingratio < 0.2)
        systemState.ratios.rotatingratio = 0.2;
    systemState.labels.rotatingRatioLabel->setText(QString("%1").arg(systemState.ratios.rotatingratio));
}

void updateHRotatingRatio(ImageState &imageState, float change,SystemState& systemState) {
    systemState.ratios.rectratio += change;
    if (systemState.ratios.rectratio < 0.05)
        systemState.ratios.rectratio = 0.05;
    systemState.labels.HrotatingRatioLabel->setText(QString("%1").arg(systemState.ratios.rectratio));
}

void updateMovingRatio(ImageState &imageState, float change,SystemState& systemState) {
    systemState.ratios.movingratio += change;
    if (systemState.ratios.movingratio < 0.2)
        systemState.ratios.movingratio = 0.2;
    systemState.labels.movingRatioLabel->setText(QString("%1").arg(systemState.ratios.movingratio));
}

void updateZoomingRatio(ImageState &imageState, float change,SystemState& systemState) {
    systemState.ratios.zoomratio += change;
    if (systemState.ratios.zoomratio < 1)
        systemState.ratios.zoomratio = 1;
    systemState.labels.zoomingRatioLabel->setText(QString("%1").arg(systemState.ratios.zoomratio));

}

void VrotateAndCalculateScore(bool rotateToLeft,ImageState& imageState,SystemState& systemState) {
    try {
        // Toggle the modification state when the button is clicked
        if (rotateToLeft)
            systemState.flags.VerticalRotationToLeft = true;
        else
            systemState.flags.VerticalRotationToRight = true;

        // Update the perspective transformation
        applyPerspectiveTransformation(imageState.images.originalImageTrap, imageState.images.originalImageRect, imageState, systemState);


        if (rotateToLeft)
            systemState.flags.VerticalRotationToLeft = false;
        else
            systemState.flags.VerticalRotationToRight = false;

        calcScore(imageState, systemState);

    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void HrotateAndCalculateScore(bool rotateToLeft,ImageState& imageState,SystemState& systemState) {
    try {
        // Toggle the modification state when the button is clicked
        if (rotateToLeft)
            systemState.flags.HorizontalRotationToLeft = true;
        else
            systemState.flags.HorizontalRotationToRight = true;

        // Update the perspective transformation
        applyPerspectiveTransformation(imageState.images.originalImageTrap, imageState.images.originalImageRect, imageState, systemState);


        if (rotateToLeft)
            systemState.flags.HorizontalRotationToLeft = false;
        else
            systemState.flags.HorizontalRotationToRight = false;

        calcScore(imageState, systemState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}


#endif // RATIO_H
