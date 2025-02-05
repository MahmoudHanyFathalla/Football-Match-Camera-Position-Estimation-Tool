#ifndef GUI_H
#define GUI_H

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
#include "calcPointsPositions.h"


void MainWindow::setFocusPolicies() {
    // Set focus policy for UI elements
    QList<QPushButton*> buttons = {
                                    ui->RectTopSideToUp, ui->RectTopSideToDown, ui->RectBottomSideToUp, ui->RectBottomSideToDown,
        ui->RectLeftSideToRight, ui->RectLeftSideToLeft, ui->RectRightSideToLeft, ui->RectRightSideToRight,
        ui->AllRectToUp, ui->AllRectToDown, ui->AllRectToRight, ui->AllRectToLeft,
        ui->TrapTopSideToUp, ui->TrapTopSideToDown, ui->TrapBottomSideToUp, ui->TrapBottomSideToDown,
        ui->VerticalRotationToLeft, ui->VerticalRotationToRight, ui->openFolder, ui->decreaseVRotationAngle,
        ui->increaseVRotationAngle, ui->increaseMovingRatio, ui->decreaseMovingRatio, ui->nextImage,
        ui->previousImage, ui->previous10Image, ui->next10Image, ui->save,
        ui->HorizontalRotationToRight, ui->HorizontalRotationToLeft, ui->reset, ui->increaseZoomRatio,
        ui->decreaseZoomRatio, ui->pushButton_34, ui->pushButton_35, ui->pushButton_36,
        ui->pushButton_37,ui->load,ui->increaseHRotationAngle,ui->decreaseHRotationAngle
    };
    for (auto button : buttons)
        button->setFocusPolicy(Qt::NoFocus);

    ui->listWidget->setFocusPolicy(Qt::NoFocus);
    ui->horizontalSlider->setFocusPolicy(Qt::NoFocus);
}

void MainWindow::loadFirstImage(ImageState& imageState) {
    cv::Mat imageRect = cv::imread("GT.jpg");

    if (!imageRect.empty()) {
        cv::cvtColor(imageRect, imageRect, cv::COLOR_BGR2RGB);

        // Resize the image to fit QLabel while maintaining aspect ratio
        QSize labelSize2 = ui->label_2->size();

       // cv::resize(imageRect, imageRect, cv::Size(labelSize2.width(), labelSize2.height()));

        // Store the original image
        imageState.images.originalImageRect = imageRect.clone();
        imageState.images.modifiedImageRect = imageState.images.originalImageRect.clone();

        // Initialize the positions of the points
        initializePoints(imageState);


        // Draw circles at each point on the images
        drawImageWithPoints2(imageState);
        // Calculate ratios if needed
        // ratio = (labelSize.width() / originalImage.size().width)
        // ratio = (labelSize.height() / originalImage.size().height)

    } else {
        qDebug() << "Error: Unable to load the images.";
    }
}

void MainWindow::setInitialLabelsValues(ImageState& imageState) {
    systemState.labels.movingRatioLabel = ui->label_19;
    systemState.labels.imgscorelable = ui->label_32;
    systemState.labels.movingRatioLabel->setText(QString("%1").arg(systemState.ratios.movingratio));

    systemState.labels.zoomingRatioLabel = ui->label_28;
   systemState.labels.zoomingRatioLabel->setText(QString("%1").arg(systemState.ratios.zoomratio));

    systemState.labels.inscore = ui->label_30;

    systemState.labels.rotatingRatioLabel = ui->label_20;
    systemState.labels.rotatingRatioLabel->setText(QString("%1").arg(systemState.ratios.rotatingratio));

    systemState.labels.HrotatingRatioLabel = ui->label_8;
    systemState.labels.HrotatingRatioLabel->setText(QString("%1").arg(systemState.ratios.rectratio));
}

void MainWindow::connectActions() {

    connect(ui -> actionShortcuts, &QAction::triggered, this, &MainWindow::openShortCutsDialog);
    connect(ui -> actionOpen_Folder, &QAction::triggered, this, &MainWindow::on_openFolder_clicked);
    connect(ui -> actionExit_2, &QAction::triggered, this, &MainWindow::exitApplication);
    connect(ui -> actionSave_2, &QAction::triggered, this, &MainWindow::saveDesign);
    connect(ui -> actionLoad_Mode, &QAction::triggered, this, &MainWindow::on_load_clicked);
    connect(ui -> actionRatios, &QAction::triggered, this, &MainWindow::openEditDialog);
    connect(ui -> actionReset, &QAction::triggered, this, &MainWindow::on_reset_clicked);

    connect(ui->splitter_2, &QSplitter::splitterMoved, this, &MainWindow::Hsplitter2ControlLabelAndLabel2);
    connect(ui->splitter, &QSplitter::splitterMoved, this, &MainWindow::HsplitterControlLabel3AndLabel4);
    connect(ui->splitter_3, &QSplitter::splitterMoved, this, &MainWindow::Vsplitter3ControlSplitterAndSplitter2);
    connect(ui->splitter_4, &QSplitter::splitterMoved, this, &MainWindow::Vsplitter4ControlWidget6AndWidget7);
    connect(ui->splitter_5, &QSplitter::splitterMoved, this, &MainWindow::Hsplitter5ControlSplitter3AndSplitter4);
    connect(ui->splitter_6, &QSplitter::splitterMoved, this, &MainWindow::Vsplitter6ControlSplitter5AndWidget4);
    connect(ui->splitter_7, &QSplitter::splitterMoved, this, &MainWindow::Vsplitter7ControlSplitter6AndWidget8);

}

void MainWindow::setStyleSheet() {

    QString buttonStyleSheet = "color: black; background-color: blue; font-weight: bold;";
    QString buttonStyleSheetY = "color: black; background-color: yellow; font-weight: bold;";

    ui->RectTopSideToUp->setStyleSheet(buttonStyleSheet);
    ui->RectTopSideToDown->setStyleSheet(buttonStyleSheet);
    ui->RectBottomSideToUp->setStyleSheet(buttonStyleSheet);
    ui->RectBottomSideToDown->setStyleSheet(buttonStyleSheet);
    ui->RectLeftSideToRight->setStyleSheet(buttonStyleSheet);
    ui->RectLeftSideToLeft->setStyleSheet(buttonStyleSheet);
    ui->RectRightSideToLeft->setStyleSheet(buttonStyleSheet);
    ui->RectRightSideToRight->setStyleSheet(buttonStyleSheet);
    ui->AllRectToUp->setStyleSheet(buttonStyleSheetY);
    ui->AllRectToDown->setStyleSheet(buttonStyleSheetY);
    ui->AllRectToRight->setStyleSheet(buttonStyleSheetY);
    ui->AllRectToLeft->setStyleSheet(buttonStyleSheetY);
    ui->TrapTopSideToUp->setStyleSheet(buttonStyleSheet);
    ui->TrapTopSideToDown->setStyleSheet(buttonStyleSheet);
    ui->TrapBottomSideToUp->setStyleSheet(buttonStyleSheet);
    ui->TrapBottomSideToDown->setStyleSheet(buttonStyleSheet);


    ui -> line_7->setStyleSheet("color: blue; background-color:rgb(0, 255, 255);");
    ui -> line_10->setStyleSheet("color: blue; background-color: rgb(0, 255, 255);");
    ui -> line_11->setStyleSheet("color: blue; background-color: rgb(0, 255, 255);");
    ui -> line_13->setStyleSheet("color: blue; background-color:rgb(0, 255, 255);");
    ui -> line_12->setStyleSheet("color: blue; background-color:rgb(0, 255, 255);");
    ui -> line_14->setStyleSheet("color: blue; background-color:rgb(0, 255, 255);");

    ui->splitter->setStyleSheet("QSplitter::handle { background-color: red; width: 2px; }");
    ui->splitter_2->setStyleSheet("QSplitter::handle { background-color: red; width: 2px; }");
    ui->splitter_3->setStyleSheet("QSplitter::handle { background-color: red; width: 2px; }");
    ui->splitter_4->setStyleSheet("QSplitter::handle { background-color: red; width: 2px; }");
    ui->splitter_5->setStyleSheet("QSplitter::handle { background-color: red; width: 2px; }");
    ui->splitter_6->setStyleSheet("QSplitter::handle { background-color: red; width: 2px; }");
    ui->splitter_7->setStyleSheet("QSplitter::handle { background-color: red; width: 2px; }");


    ui->pushButton_34->setStyleSheet("background-color: green; color: white;");
    ui->pushButton_35->setStyleSheet("background-color: black; color: white;");
    ui->pushButton_36->setStyleSheet("background-color: green; color: white; border: 4px solid black;");
    ui->pushButton_37->setStyleSheet("background-color: black; color: white; border: 4px solid green;");

    ui->label_5->setStyleSheet("color: white;");

    ui->widget_6->setStyleSheet("background-color: black;");
    ui->widget_7->setStyleSheet("background-color: green;");



}

void MainWindow::buildUI() {

    QGridLayout *gridLayout = qobject_cast<QGridLayout*>(ui->widget_5->layout());

    ui->label_3->setScaledContents(true);
    ui->label_4->setScaledContents(true);
    ui->label->setScaledContents(true);
    ui->label_2->setScaledContents(true);

    ui->label_3->resize(300, 303);
    ui->label_4->resize(300, 303);

    ui->label->setMouseTracking(true); //real
    ui->label_2->setMouseTracking(true); //playGround

    ui->splitter_2->addWidget(ui ->label);
    ui->splitter_2->addWidget(ui ->label_2);
    ui->splitter->addWidget(ui ->label_3);
    ui->splitter->addWidget(ui ->label_4);
    ui->splitter_3->addWidget(ui ->splitter_2);
    ui->splitter_3->addWidget(ui ->splitter);

    ui->listWidget->setVisible(false);
    ui->pushButton_34->setVisible(false);
    ui->pushButton_35->setVisible(false);
    ui->pushButton_36->setVisible(false);
    ui->pushButton_37->setVisible(false);
    ui->load->setVisible(false);
    ui->save->setVisible(false);
    ui->reset->setVisible(false);
    ui->openFolder->setVisible(false);



    ui->line_7->setFixedHeight(10000);
    ui->line_10->setFixedHeight(10000);
    ui->line_11->setFixedWidth(10000);
    ui->line_13->setFixedWidth(10000);
    ui->line_12->setFixedWidth(10000);
    ui->line_14->setFixedWidth(10000);

    ui->splitter->setMinimumHeight(10);
    ui->splitter->setMinimumWidth(10);
    ui->splitter_2->setMinimumHeight(10);
    ui->splitter_2->setMinimumWidth(10);
    ui->splitter_3->setMinimumHeight(10);
    ui->splitter_3->setMinimumWidth(10);
    ui->splitter_4->setMinimumHeight(10);
    ui->splitter_4->setMinimumWidth(10);
    ui->splitter_5->setMinimumHeight(10);
    ui->splitter_5->setMinimumWidth(10);
    ui->splitter_6->setMinimumHeight(10);
    ui->splitter_6->setMinimumWidth(10);
    ui->splitter_7->setMinimumHeight(30);
    ui->splitter_7->setMinimumWidth(30);

}

void MainWindow:: statusBarBuild() {

    QWidget *spacerWidget = new QWidget();
    QHBoxLayout *spacerLayout = new QHBoxLayout(spacerWidget);
    spacerLayout->setContentsMargins(0, 0, 0, 0);
    spacerLayout->addSpacerItem(new QSpacerItem(100, 20, QSizePolicy::Expanding, QSizePolicy::Preferred));
    statusBar()->addWidget(ui -> label_26);
    statusBar()->addWidget(ui -> label_25);
    statusBar()->addWidget(ui -> label_6);
    statusBar()->addWidget(spacerWidget);
    statusBar()->addWidget(ui -> label_29);
    statusBar()->addWidget(ui -> label_30);
    statusBar()->addWidget(ui -> label_31);
    statusBar()->addWidget(ui -> label_32);
    statusBar()->addPermanentWidget(ui ->label_23);

}


#endif // GUI_H
