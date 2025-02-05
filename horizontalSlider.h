#ifndef HORIZONTALSLIDER_H
#define HORIZONTALSLIDER_H


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



void MainWindow::onHorizontalSliderMoved(int position)
{
    // Update the displayed image based on the slider value
    loadAndDisplayImage(position,imageState,systemState);
}

void MainWindow::onHorizontalSliderPressed()
{
    // Handle slider pressed event
    disconnect(ui->horizontalSlider, &QSlider::sliderMoved, this, &MainWindow::onHorizontalSliderMoved);
    systemState.ratios.previousSliderPosition = ui->horizontalSlider->value();
    // For example, you can add code here to pause automatic slider updates
}

void MainWindow::onHorizontalSliderReleased()
{
    // Connect the sliderMoved signal back
    connect(ui->horizontalSlider, &QSlider::sliderMoved, this, &MainWindow::onHorizontalSliderMoved);

    // Update the displayed image based on the current slider value
    loadAndDisplayImage(ui->horizontalSlider->value(), imageState,systemState);

    // For example, you can add code here to resume automatic slider updates
}

void MainWindow::updateSliderSize(ImageState& imageState)
{
    // Update the slider based on its width
    int availableSpace = ui->horizontalSlider->width();

    // Calculate the maximum value of the slider based on the available space
    int maxSliderValue = systemState.stringData.imageFileList.size() - 1;
    int sliderValue = static_cast<int>((static_cast<double>(maxSliderValue) / availableSpace) * ui->horizontalSlider->width());

    // Save the current slider value before updating
    int currentSliderValue = ui->horizontalSlider->value();

    // Disconnect the sliderMoved signal temporarily
    disconnect(ui->horizontalSlider, &QSlider::sliderMoved, this, &MainWindow::onHorizontalSliderMoved);

    // Set the single step size to zero to prevent handle movement on click
    ui->horizontalSlider->setSingleStep(0);
    ui->horizontalSlider->setSliderPosition(sliderValue);

    // Reset the single step size
    ui->horizontalSlider->setSingleStep(1);

    // Reconnect the sliderMoved signal
    connect(ui->horizontalSlider, &QSlider::sliderMoved, this, &MainWindow::onHorizontalSliderMoved);

    // Reset the slider to its previous position to prevent movement when clicking on the line
    ui->horizontalSlider->setValue(currentSliderValue);
}







#endif // HORIZONTALSLIDER_H
