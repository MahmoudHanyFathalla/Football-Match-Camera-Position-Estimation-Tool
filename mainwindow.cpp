#include "mainwindow.h"
#include "matrix.h"
#include "gui.h"
#include "calcPointsPositions.h"
#include "horizontalSlider.h"
#include "shortcutsdialog.h"
#include "edit.h"
#include "txtOperations.h"
#include "draw.h"
#include "viewWindows.h"
#include "score.h"
#include "ratio.h"
#include "events.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <QDebug>
#include <cmath>
#include <QImage>
#include <QPixmap>
#include <QTimer>
#include "real.h"
#include <QPointer>
#include <QMessageBox>
#include <iostream>
#include "ImageState.h"
#include <QPainter>
#include <QRegularExpression>
#include "systemState.h"
#include "edit.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    try {
        ui->setupUi(this);

        //setFixedSize(1360, 695);
        QWidget window;
        window.showMaximized();

        buildUI();

        setFocusPolicies();

        loadFirstImage(imageState);

        statusBarBuild();

        connectActions();

        setStyleSheet();

        setInitialLabelsValues(imageState);

       // removeWiget8();

        removeSplitter4();

         ui->splitter->setVisible(false);

        ui->label_2->setVisible(false);
        ui->label_4->setVisible(false);
        ui->label_3->setVisible(false);






        QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, Qt::Key_J, Qt::ControlModifier);
        QApplication::postEvent(this, event);

    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_pushButton_clicked()
{
    // Get the file path for the first video
    videoFilePath1 = QFileDialog::getOpenFileName(this, "Open First Video", "", "Video Files (*.mp4 *.avi *.mkv)");
    if (!videoFilePath1.isEmpty()) {
        pixmap1 = getFirstFrame(videoFilePath1);
        updateLabel();
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    // Get the file path for the second video
    videoFilePath2 = QFileDialog::getOpenFileName(this, "Open Second Video", "", "Video Files (*.mp4 *.avi *.mkv)");
    if (!videoFilePath2.isEmpty()) {
        pixmap2 = getFirstFrame(videoFilePath2);
        updateLabel();
    }
}

QPixmap MainWindow::getFirstFrame(const QString &filePath)
{
    cv::VideoCapture cap(filePath.toStdString());
    if (!cap.isOpened()) {
        qDebug() << "Error: Unable to open video file";
        return QPixmap();
    }

    cv::Mat frame;
    cap >> frame; // Capture the first frame
    if (frame.empty()) {
        qDebug() << "Error: Unable to capture frame";
        return QPixmap();
    }

    // Convert cv::Mat to QImage
    QImage img(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_BGR888);
    return QPixmap::fromImage(img);
}

void MainWindow::updateLabel()
{
    int totalWidth = 0;
    int maxHeight = 0;

    if (!pixmap1.isNull()) {
        totalWidth += pixmap1.width();
        maxHeight = std::max(maxHeight, pixmap1.height());
    }

    if (!pixmap2.isNull()) {
        totalWidth += pixmap2.width();
        maxHeight = std::max(maxHeight, pixmap2.height());
    }

    combinedPixmap = QPixmap(totalWidth, maxHeight); // Assign combinedPixmap
    originalPixmap =  combinedPixmap ;

    combinedPixmap.fill(Qt::transparent);

    QPainter painter(&combinedPixmap);

    int xOffset = 0;

    if (!pixmap1.isNull()) {
        painter.drawPixmap(xOffset, 0, pixmap1);
        xOffset += pixmap1.width();
    }

    if (!pixmap2.isNull()) {
        painter.drawPixmap(xOffset, 0, pixmap2);
    }

    // Print the dimensions of the concatenated image
    qDebug() << "Dimensions of concatenated image: " << totalWidth << "x" << maxHeight;

    ui->label->setPixmap(combinedPixmap);
}


void MainWindow::zoomInn()
{
    if (combinedPixmap.isNull()) {
        qDebug() << "Combined pixmap is null.";
        return;
    }

    try {
        // Calculate the size of the zoomed region
        int roiWidth = static_cast<int>(combinedPixmap.width() / zoomFactor);
        int roiHeight = static_cast<int>(combinedPixmap.height() / zoomFactor);

        // Calculate the ROI coordinates centered around the original center
        int roiX = combinedPixmap.width() / 2 - roiWidth / 2;
        int roiY = centerY - roiHeight / 2;

        // Ensure ROI does not exceed image boundaries
        roiX = std::max(0, roiX);
       // roiY = std::max(0, roiY);
        roiWidth = std::min(combinedPixmap.width() - roiX, roiWidth);
        roiHeight = std::min(combinedPixmap.height() - roiY, roiHeight);

        // Adjust ROI Y-coordinate and height to fit within the image boundaries
        if (roiY + roiHeight > combinedPixmap.height()) {
            roiHeight = combinedPixmap.height() - roiY;
        }

        // Create the ROI
        QPixmap zoomedPixmap = combinedPixmap.copy(roiX, roiY, roiWidth, roiHeight);

        // Draw a small blue circle at the center of the zoomed area
        QPainter painter(&zoomedPixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(Qt::blue, 2));
        painter.setBrush(Qt::blue);
        int centerX = zoomedPixmap.width() / 2;
        int zoomCenterY = zoomedPixmap.height() / 2;
        painter.drawEllipse(centerX - 5, zoomCenterY - 5, 10, 10);
        painter.end();

        // Resize the zoomed region to the original size
        QPixmap scaledZoomedPixmap = zoomedPixmap.scaled(combinedPixmap.size(), Qt::IgnoreAspectRatio);

        // Display the zoomed image
        ui->label->setPixmap(scaledZoomedPixmap);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
    }
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if (combinedPixmap.isNull()) {
        qDebug() << "Combined pixmap is null.";
        return;
    }

    // Calculate zoom parameters
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    int delta = 0;
    if (!numPixels.isNull()) {
        // Mouse wheel scrolls in pixel increments
        delta = numPixels.y();
    } else if (!numDegrees.isNull()) {
        // Mouse wheel scrolls in degree increments (e.g., on a touchpad)
        delta = numDegrees.y();
    }

    if (delta != 0) {
        // Adjust the zoom factor based on the direction of scrolling
        if (delta > 0) {
            zoomFactor += 0.2;
        } else {
            if (zoomFactor <= 0.2)
                zoomFactor = 0.2; // Ensure zoomFactor does not go below 0.2
            else
                zoomFactor -= 0.2;
        }
        zoomInn();
    }

    event->accept();
}




void MainWindow::zooom(bool zoomIn, float zoomFactor)
{

    // Adjust the zoom factor based on the direction of scrolling

    float factor = zoomIn ? zoomFactor : 1 / zoomFactor;

    // Calculate the size of the zoomed region
    int roiWidth = combinedPixmap.width() / factor;
    int roiHeight = combinedPixmap.height() / factor;

    // Calculate the ROI coordinates centered around the original center
    int roiX = combinedPixmap.width() / 2 - roiWidth / 2;
    int roiY = combinedPixmap.height() / 2 - roiHeight / 2;

    // Ensure ROI does not exceed image boundaries
    roiX = std::max(0, roiX);
    roiY = std::max(0, roiY);
    roiWidth = std::min(combinedPixmap.width() - roiX, roiWidth);
    roiHeight = std::min(combinedPixmap.height() - roiY, roiHeight);

    // Create the ROI
    QPixmap zoomedPixmap = combinedPixmap.copy(roiX, roiY, roiWidth, roiHeight);

    // Resize the zoomed region to the original size
    QPixmap scaledZoomedPixmap = zoomedPixmap.scaled(combinedPixmap.size(), Qt::KeepAspectRatio);

    // Display the zoomed image
    ui->label->setPixmap(scaledZoomedPixmap);
}

void MainWindow::on_pushButton_3_clicked()
{
    // Open the first video file associated with videoCapture1
    videoCapture1.open(videoFilePath1.toStdString());
    if (!videoCapture1.isOpened()) {
        qDebug() << "Error: Unable to open first video file";
        return;
    }

    // Move to the next frame in videoCapture1
    if (frameIndex1 >= 0) {
        videoCapture1.set(cv::CAP_PROP_POS_FRAMES, frameIndex1);
    }
    cv::Mat frame1;
    videoCapture1 >> frame1;
    if (frame1.empty()) {
        qDebug() << "End of video (Video 1)";
        return;
    }
    pixmap1 = convertMatToPixmap(frame1);
    updateLabel();
    frameIndex1++;

    // Print the frame number
    qDebug() << "Frame number (Video 1):" << frameIndex1;
    ui->label->setPixmap(pixmap1);

    // Open the second video file associated with videoCapture2
    videoCapture2.open(videoFilePath2.toStdString());
    if (!videoCapture2.isOpened()) {
        qDebug() << "Error: Unable to open second video file";
        return;
    }

    // Move to the next frame in videoCapture2
    if (frameIndex2 >= 0) {
        videoCapture2.set(cv::CAP_PROP_POS_FRAMES, frameIndex2);
    }
    cv::Mat frame2;
    videoCapture2 >> frame2;
    if (frame2.empty()) {
        qDebug() << "End of video (Video 2)";
        return;
    }
    pixmap2 = convertMatToPixmap(frame2);
    updateLabel();
    frameIndex2++;

    // Print the frame number
    qDebug() << "Frame number (Video 2):" << frameIndex2;
    ui->label_2->setPixmap(pixmap2);
    zoomFactor =1.2;
    centerY =500;
    clickedPoints.clear();
}

void MainWindow::on_pushButton_4_clicked()
{
    // Open the first video file associated with videoCapture1
    videoCapture1.open(videoFilePath1.toStdString());
    if (!videoCapture1.isOpened()) {
        qDebug() << "Error: Unable to open first video file";
        return;
    }

    // Move to the previous frame in videoCapture1
    if (frameIndex1 > 1) {
        videoCapture1.set(cv::CAP_PROP_POS_FRAMES, frameIndex1 - 2);
        cv::Mat frame1;
        videoCapture1 >> frame1;
        if (frame1.empty()) {
            qDebug() << "Error: Unable to capture frame (Video 1)";
            return;
        }
        pixmap1 = convertMatToPixmap(frame1);
        updateLabel();
        frameIndex1--;

        // Print the frame number
        qDebug() << "Frame number (Video 1):" << frameIndex1;
        ui->label->setPixmap(pixmap1);
    } else {
        qDebug() << "Already at the first frame (Video 1)";
    }

    // Open the second video file associated with videoCapture2
    videoCapture2.open(videoFilePath2.toStdString());
    if (!videoCapture2.isOpened()) {
        qDebug() << "Error: Unable to open second video file";
        return;
    }

    // Move to the previous frame in videoCapture2
    if (frameIndex2 > 1) {
        videoCapture2.set(cv::CAP_PROP_POS_FRAMES, frameIndex2 - 2);
        cv::Mat frame2;
        videoCapture2 >> frame2;
        if (frame2.empty()) {
            qDebug() << "Error: Unable to capture frame (Video 2)";
            return;
        }
        pixmap2 = convertMatToPixmap(frame2);
        updateLabel();
        frameIndex2--;

        // Print the frame number
        qDebug() << "Frame number (Video 2):" << frameIndex2;
        ui->label_2->setPixmap(pixmap2);
    } else {
        qDebug() << "Already at the first frame (Video 2)";
    }

    zoomFactor =1.2;
    centerY =500;
    clickedPoints.clear();
}



QPixmap MainWindow::convertMatToPixmap(const cv::Mat &mat) {
    // Convert cv::Mat to QImage
    QImage img(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_BGR888);

    // Convert QImage to QPixmap
    return QPixmap::fromImage(img);
}




void MainWindow::removeLabel2() {
    // Get the size of splitter_3
    QSize splitterSize = ui->splitter_3->size();

    resetLabels();

    // Set the minimum width and height of label_2 to match the size of splitter_3

    // Hide splitter_2
    ui->splitter->setVisible(false);
    // Hide splitter_2
    ui->label->setVisible(false);

    // Hide splitter_2
    ui->label_3->setVisible(false);
     ui->label_4->setVisible(false);
     ui->label_2->setMinimumSize(splitterSize);


}

void MainWindow::removeLabel() {
    // Get the size of splitter_3
    QSize splitterSize = ui->splitter_3->size();

    // Set the minimum width and height of label_2 to match the size of splitter_3

    // Hide splitter_2
    resetLabels();

    ui->splitter->setVisible(false);
    // Hide splitter_2
    ui->label_2->setVisible(false);

    // Hide splitter_2
    ui->label_3->setVisible(false);
    ui->label_4->setVisible(false);
    ui->label->setMinimumSize(splitterSize);


}

void MainWindow::removeLabel3() {
    // Get the size of splitter_3
    QSize splitterSize = ui->splitter_3->size();

    // Set the minimum width and height of label_2 to match the size of splitter_3

    resetLabels();

    ui->splitter_2->setVisible(false);
    // Hide splitter_2
    ui->label_2->setVisible(false);

    // Hide splitter_2
    ui->label->setVisible(false);
    ui->label_4->setVisible(false);
    ui->label_3->setMinimumSize(splitterSize);


}

void MainWindow::removeLabel4() {
    // Get the size of splitter_3
    QSize splitterSize = ui->splitter_3->size();

    // Set the minimum width and height of label_2 to match the size of splitter_3

    resetLabels();

    ui->splitter_2->setVisible(false);
    // Hide splitter_2
    ui->label_2->setVisible(false);

    // Hide splitter_2
    ui->label->setVisible(false);
    ui->label_3->setVisible(false);
    ui->label_4->setMinimumSize(splitterSize);


}

void MainWindow::resetLabels() {
    // Get the size of splitter_3
    QSize splitterSize = ui->splitter_3->size();

    // Set the minimum width and height of label_2 to match the size of splitter_3

    // Hide splitter_2
    ui->splitter->setVisible(true);
     ui->splitter_2->setVisible(true);
    ui->label->setVisible(true);
    ui->label_2->setVisible(true);
    ui->label_3->setVisible(true);
    ui->label_4->setVisible(true);

    ui->label_2->setMinimumSize(0, 0);
    ui->label->setMinimumSize(0, 0);
    ui->label_3->setMinimumSize(0, 0);
    ui->label_4->setMinimumSize(0, 0);


}

void MainWindow::removeWiget8() {
    // Get the size of splitter_3
    QSize splitterSize = ui->splitter_7->size();

    // Set the minimum width and height of label_2 to match the size of splitter_3

    ui->widget_8->setVisible(false);
    // Hide splitter_2

    ui->splitter_6->setMinimumSize(splitterSize);

}

void MainWindow::resetWiget8() {

    ui->widget_8->setVisible(true);
    // Hide splitter_2

    ui->widget_8->setMinimumSize(0, 0);
    ui->splitter_6->setMinimumSize(0, 0);
    ui->label_2->setMinimumSize(0, 0);
    ui->label->setMinimumSize(0, 0);
    ui->label_3->setMinimumSize(0, 0);
    ui->label_4->setMinimumSize(0, 0);

    ui->widget_6->setMinimumSize(0, 0);
    ui->widget_7->setMinimumSize(0, 0);
    ui->widget_4->setMinimumSize(0, 0);
    ui->splitter_3->setMinimumSize(0, 0);
     ui->splitter_4->setMinimumSize(0, 0);
      ui->splitter_5->setMinimumSize(0, 0);
       ui->splitter_2->setMinimumSize(0, 0);
        ui->splitter->setMinimumSize(0, 0);


}

void MainWindow::removeSplitter4() {
    // Get the size of splitter_3
    QSize splitterSize = ui->splitter_5->size();


    // Set the minimum width and height of label_2 to match the size of splitter_3

    // Hide splitter_2
    ui->splitter_4->setVisible(false);
     ui->widget_7->setVisible(false);
      ui->widget_6->setVisible(false);

    ui->splitter_3->setMinimumSize(splitterSize);


}

void MainWindow::resetSplitter4() {
    // Get the size of splitter_3


    // Set the minimum width and height of label_2 to match the size of splitter_3

    // Hide splitter_2
    ui->splitter_4->setVisible(true);
    ui->widget_7->setVisible(true);
    ui->widget_6->setVisible(true);

    ui->widget_6->setMinimumSize(0, 0);
    ui->widget_7->setMinimumSize(0, 0);
    ui->widget_4->setMinimumSize(0, 0);
    ui->splitter_3->setMinimumSize(0, 0);
    ui->splitter_4->setMinimumSize(0, 0);
    ui->splitter_5->setMinimumSize(0, 0);
    ui->splitter_2->setMinimumSize(0, 0);
    ui->splitter->setMinimumSize(0, 0);
    ui->label->setMinimumSize(0, 0);
    ui->label_2->setMinimumSize(0, 0);
    ui->label_3->setMinimumSize(0, 0);
    ui->label_4->setMinimumSize(0, 0);


}

void MainWindow::Vsplitter3ControlSplitterAndSplitter2(int pos, int index)
{
    // Calculate the total height of the outer splitter
    int totalHeight = ui->splitter_3->height();

    // Get the cursor position
    QPoint cursorPos = QCursor::pos();
    QPoint splitterPos = ui->splitter_3->mapFromGlobal(cursorPos);

    // Calculate the position of the outer splitter's handle
    int handlePosition = splitterPos.y();

    // Calculate the height for splitter and splitter_2 based on the handle position
    int splitterHeight = handlePosition;
    int splitter_2Height = totalHeight - handlePosition;

    // Set the sizes of splitter and splitter_2
    ui->splitter_2->setMinimumHeight(splitterHeight);
    ui->splitter->setMinimumHeight(splitter_2Height);

    // Update label sizes in splitter_2
    Hsplitter2ControlLabelAndLabel2(handlePosition, index);

    // Update label sizes in splitter
    HsplitterControlLabel3AndLabel4(totalHeight - handlePosition, index);

    labelsSize();

}

void MainWindow::Vsplitter7ControlSplitter6AndWidget8(int pos, int index)
{
    // Calculate the total height of the outer splitter
    int totalHeight = ui->splitter_7->height();

    // Get the cursor position
    QPoint cursorPos = QCursor::pos();
    QPoint splitterPos = ui->splitter_7->mapFromGlobal(cursorPos);

    // Calculate the position of the outer splitter's handle
    int handlePosition = splitterPos.y();

    // Calculate the height for splitter and splitter_2 based on the handle position
    int splitterHeight = handlePosition;
    int splitter_2Height = totalHeight - handlePosition;

    // Set the sizes of splitter and splitter_2
    ui->splitter_6->setMinimumHeight(splitterHeight);
    ui->widget_8->setMinimumHeight(splitter_2Height);
    adjustLabelsBySplitter7(handlePosition);



    labelsSize();

}

void MainWindow::adjustLabelsBySplitter7(int handlePosition)
{
    int totalHeight = ui->splitter_6->height();

    // Calculate the height for labels and splitters inside splitter_3
    int labelHeight = totalHeight * 0.8; // Example: divide equally
    int splitterHeight = totalHeight - labelHeight;





    ui->splitter_5->setMinimumHeight(labelHeight);
    ui->widget_4->setMinimumHeight(totalHeight * 0.2);
    adjustLabelsBySplitter6(handlePosition);



    // You may need to adjust other labels and splitters inside splitter_3 as necessary
}

void MainWindow::Vsplitter4ControlWidget6AndWidget7(int pos, int index)
{
    // Calculate the total height of splitter_4
    int totalHeight = ui->splitter_4->height();

    // Calculate the position of the splitter handle within splitter_4
    QPoint cursorPos = QCursor::pos();
    QPoint splitterPos = ui->splitter_4->mapFromGlobal(cursorPos);
    int handlePosition = splitterPos.y();

    // Calculate the height for the two widgets inside splitter_4 based on the handle position
    int widget1Height = handlePosition;
    int widget2Height = totalHeight - handlePosition;

    // Set the sizes of the widgets inside splitter_4
    ui->widget_6->setMinimumHeight(widget1Height);
    ui->widget_7->setMinimumHeight(widget2Height);

    labelsSize();

    // Update label sizes or perform any other necessary updates
    //updateLabelSizesInsideSplitter4(widget1Height, widget2Height, index);
}

void MainWindow::Vsplitter6ControlSplitter5AndWidget4(int pos, int index)
{
    // Calculate the total height of splitter_4
    int totalHeight = ui->splitter_6->height();

    // Calculate the position of the splitter handle within splitter_4
    QPoint cursorPos = QCursor::pos();
    QPoint splitterPos = ui->splitter_6->mapFromGlobal(cursorPos);
    int handlePosition = splitterPos.y();

    // Calculate the height for the two widgets inside splitter_4 based on the handle position
    int widget1Height = handlePosition;
    int widget2Height = totalHeight - handlePosition;

    // Set the sizes of the widgets inside splitter_4
    ui->widget_4->setMinimumHeight(widget1Height);
    ui->splitter_5->setMinimumHeight(widget2Height);



    //updateAllLabelSizes(totalHeight - handlePosition,index);

    labelsSize();
     adjustLabelsBySplitter6(handlePosition);
}

void MainWindow::adjustLabelsBySplitter6(int handlePosition)
{
    int totalHeight = ui->splitter_3->height();

    // Calculate the height for labels and splitters inside splitter_3
    int labelHeight = totalHeight * 0.5; // Example: divide equally
    int splitterHeight = totalHeight - labelHeight;

/*
    ui->label->setMinimumHeight(labelHeight);
    ui->label_2->setMinimumHeight(labelHeight);

    ui->label_3->setMinimumHeight(labelHeight);
    ui->label_4->setMinimumHeight(labelHeight);
*/
    ui->widget_6->setMinimumHeight(labelHeight);
    ui->widget_7->setMinimumHeight(labelHeight);

    ui->splitter->setMinimumHeight(splitterHeight);
    ui->splitter_2->setMinimumHeight(splitterHeight);

    ui->splitter_4->setMinimumHeight(totalHeight);

    // You may need to adjust other labels and splitters inside splitter_3 as necessary
}

void MainWindow::Hsplitter5ControlSplitter3AndSplitter4(int pos, int index)
{
    // Calculate the total width of splitter_5
    int totalWidth = ui->splitter_5->width();

    // Calculate the position of the splitter handle within splitter_5
    QPoint cursorPos = QCursor::pos();
    QPoint splitterPos = ui->splitter_5->mapFromGlobal(cursorPos);
    int handlePosition = splitterPos.x();

    // Calculate the width for the widget and the splitter inside splitter_5 based on the handle position
    int widgetWidth = handlePosition;
    int splitterWidth = totalWidth - handlePosition;


    ui->splitter_5->widget(0)->setMinimumWidth(widgetWidth); //splitter_3
    ui->splitter_5->widget(1)->setMinimumWidth(splitterWidth); //splitter_4

    adjustLabelsBySplitter5(handlePosition);


    labelsSize();

    // You may need to call additional functions or perform further updates here if necessary
}

void MainWindow::adjustLabelsBySplitter5(int handlePosition)
{
    // Calculate the total width of splitter_3
    int totalWidth = ui->splitter_3->width();

    // Calculate the width for labels and splitters inside splitter_3
    int labelWidth = totalWidth * 0.5; // Example: divide equally
    int splitterWidth = totalWidth - labelWidth;

    // Set the sizes of labels and splitters inside splitter_3
    ui->label->setMinimumWidth(labelWidth);
    ui->label_2->setMinimumWidth(labelWidth);

    ui->label_3->setMinimumWidth(labelWidth);
    ui->label_4->setMinimumWidth(labelWidth);

    ui->splitter->setMinimumWidth(splitterWidth);
    ui->splitter_2->setMinimumWidth(splitterWidth);

    // You may need to adjust other labels and splitters inside splitter_3 as necessary
}


void MainWindow::Hsplitter2ControlLabelAndLabel2(int pos, int index)
{
    // Calculate the total width of the splitter
    int totalWidth = ui->splitter_2->width();

    // Get the cursor position
    QPoint cursorPos = QCursor::pos();
    QPoint splitterPos = ui->splitter_2->mapFromGlobal(cursorPos);

    // Calculate the position of the splitter's handle
    int handlePosition = splitterPos.x();

    // Calculate the width for label and label_2 based on the handle position
    int labelWidth = handlePosition;
    int label_2Width = totalWidth - handlePosition;

    // Set the sizes of label and label_2
    ui->splitter_2->widget(0)->setMinimumWidth(labelWidth); //label
    ui->splitter_2->widget(1)->setMinimumWidth(label_2Width);  //label_22

    labelsSize();
}

void MainWindow::HsplitterControlLabel3AndLabel4(int pos, int index)
{
    // Calculate the total width of the splitter
    int totalWidth = ui->splitter->width();

    // Get the cursor position
    QPoint cursorPos = QCursor::pos();
    QPoint splitterPos = ui->splitter->mapFromGlobal(cursorPos);

    // Calculate the position of the splitter's handle
    int handlePosition = splitterPos.x();

    // Calculate the width for label and label_2 based on the handle position
    int label_3Width = handlePosition;
    int label_4Width = totalWidth - handlePosition;

    // Set the sizes of label and label_2
    ui->splitter->widget(0)->setMinimumWidth(label_3Width); //label_3
    ui->splitter->widget(1)->setMinimumWidth(label_4Width);  //label_4

    labelsSize();
}

void MainWindow::setComponentsBasedOnRatios(ImageState &imageState) {
    QFile file("ratios.config"); // Assuming the config file is named "ratios.config"
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open config file for reading.";
        return;
    }

    QTextStream in(&file);

    // Declare variables to store ratio values
    double widget8HeightRatio = 0.0;
    double splitter6HeightRatio = 0.0;
    double widget4HeightRatio = 0.0;
    double splitter5HeightRatio = 0.0;
    double splitter4WidthRatio = 0.0;
    double widget6HeightRatio = 0.0;
    double widget7HeightRatio = 0.0;
    double splitter3WidthRatio = 0.0;
    double splitter2HeightRatio = 0.0;
    double splitterHeightRatio = 0.0;
    double labelWidthRatio = 0.0;
    double label2WidthRatio = 0.0;
    double label3WidthRatio = 0.0;
    double label4WidthRatio = 0.0;

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split("=");
        if (parts.size() == 2) {
            QString key = parts[0].trimmed();
            double value = parts[1].trimmed().toDouble();
            // Save the values in variables for later use
            if (key == "widget8HeightRatio") {
                widget8HeightRatio = value;
            } else if (key == "splitter6HeightRatio") {
                splitter6HeightRatio = value;
            } else if (key == "widget4HeightRatio") {
                widget4HeightRatio = value;
            } else if (key == "splitter5HeightRatio") {
                splitter5HeightRatio = value;
            } else if (key == "splitter4WidthRatio") {
                splitter4WidthRatio = value;
            } else if (key == "widget6HeightRatio") {
                widget6HeightRatio = value;
            } else if (key == "widget7HeightRatio") {
                widget7HeightRatio = value;
            } else if (key == "splitter3WidthRatio") {
                splitter3WidthRatio = value;
                qDebug() << "Splitter 3 Width Ratio: " << splitter3WidthRatio;
            } else if (key == "splitter2HeightRatio") {
                splitter2HeightRatio = value;
            } else if (key == "splitterHeightRatio") {
                splitterHeightRatio = value;
            } else if (key == "labelWidthRatio") {
                labelWidthRatio = value;
            } else if (key == "label2WidthRatio") {
                label2WidthRatio = value;
            } else if (key == "label3WidthRatio") {
                label3WidthRatio = value;
            } else if (key == "label4WidthRatio") {
                label4WidthRatio = value;
            }
            // Add more conditions to save values for other ratios
        }
    }


    qDebug() << "Splitters:";
    qDebug() << "Splitter 1 - Width:" << ui->splitter->width() << "Height:" << ui->splitter->height();
    qDebug() << "Splitter 2 - Width:" << ui->splitter_2->width() << "Height:" << ui->splitter_2->height();
    qDebug() << "Splitter 3 - Width:" << ui->splitter_3->width() << "Height:" << ui->splitter_3->height();
    qDebug() << "Splitter 4 - Width:" << ui->splitter_4->width() << "Height:" << ui->splitter_4->height();
    qDebug() << "Splitter 5 - Width:" << ui->splitter_5->width() << "Height:" << ui->splitter_5->height();
    qDebug() << "Splitter 6 - Width:" << ui->splitter_6->width() << "Height:" << ui->splitter_6->height();
    qDebug() << "Splitter 7 - Width:" << ui->splitter_7->width() << "Height:" << ui->splitter_7->height();

    qDebug() << "Labels:";
    qDebug() << "Label 1 - Width:" << ui->label->width() << "Height:" << ui->label->height();
    qDebug() << "Label 2 - Width:" << ui->label_2->width() << "Height:" << ui->label_2->height();
    qDebug() << "Label 3 - Width:" << ui->label_3->width() << "Height:" << ui->label_3->height();
    qDebug() << "Label 4 - Width:" << ui->label_4->width() << "Height:" << ui->label_4->height();

    // Similarly for widgets
    qDebug() << "Widgets:";
    qDebug() << "Widget 4 - Width:" << ui->widget_4->width() << "Height:" << ui->widget_4->height();
    qDebug() << "Widget 6 - Width:" << ui->widget_6->width() << "Height:" << ui->widget_6->height();
    qDebug() << "Widget 7 - Width:" << ui->widget_7->width() << "Height:" << ui->widget_7->height();
    qDebug() << "Widget 8 - Width:" << ui->widget_8->width() << "Height:" << ui->widget_8->height();


    // Store widths and heights of UI elements in variables
    int splitter7Height = ui->splitter_7->height();
    int splitter5Width = ui->splitter_5->width();

    int splitter4Height = ui->splitter_4->height();
    int splitter3Width = ui->splitter_3->width();
    int splitter2Height = ui->splitter_2->height();
    int splitterHeight = ui->splitter->height();
    int splitter6Height = ui->splitter_6->height();
     int splitter3Height = ui->splitter_3->height();

    // Declare additional variables for labels and widgets
    int label2Width = ui->label_2->width();
    int labelWidth = ui->label->width();
    int splitter2Width = ui->splitter_2->width();
    int splitterWidth = ui->splitter->width();
    int widget6Height = ui->widget_6->height();
    int widget7Height = ui->widget_7->height();
    int widget8Height = ui->widget_8->height();
    int widget4Height = ui->widget_4->height();

    // Set minimum heights and widths using stored variables and ratios
    ui->widget_8->setMinimumHeight(widget8HeightRatio * splitter7Height);
    ui->splitter_6->setMinimumHeight(splitter6HeightRatio * splitter7Height);
    ui->widget_4->setMinimumHeight(widget4HeightRatio *splitter6Height );
    ui->splitter_5->setMinimumHeight(splitter5HeightRatio * splitter6Height);
    ui->splitter_4->setMinimumWidth(splitter4WidthRatio * splitter5Width);
    ui->widget_6->setMinimumHeight(widget6HeightRatio * splitter4Height);
    ui->widget_7->setMinimumHeight(widget7HeightRatio * splitter4Height);
    ui->splitter_3->setMinimumWidth(splitter3WidthRatio * splitter5Width);
    ui->splitter_2->setMinimumHeight(splitter2HeightRatio * splitter3Height);
    ui->splitter->setMinimumHeight(splitterHeightRatio * splitter3Height);
    ui->label->setMinimumWidth(labelWidthRatio * splitter2Width);
    ui->label_2->setMinimumWidth(label2WidthRatio * splitter2Width);
    ui->label_3->setMinimumWidth(label3WidthRatio * splitterWidth);
    ui->label_4->setMinimumWidth(label4WidthRatio * splitterWidth);




    // Inside your function or method where you have access to the UI elements
    // Assuming the splitters are of type QSplitter, labels are of type QLabel, and widgets are of QWidget

    qDebug() << "Splitters:";
    qDebug() << "Splitter 1 - Width:" << ui->splitter->width() << "Height:" << ui->splitter->height();
    qDebug() << "Splitter 2 - Width:" << ui->splitter_2->width() << "Height:" << ui->splitter_2->height();
    qDebug() << "Splitter 3 - Width:" << ui->splitter_3->width() << "Height:" << ui->splitter_3->height();
    qDebug() << "Splitter 4 - Width:" << ui->splitter_4->width() << "Height:" << ui->splitter_4->height();
    qDebug() << "Splitter 5 - Width:" << ui->splitter_5->width() << "Height:" << ui->splitter_5->height();
    qDebug() << "Splitter 6 - Width:" << ui->splitter_6->width() << "Height:" << ui->splitter_6->height();
    qDebug() << "Splitter 7 - Width:" << ui->splitter_7->width() << "Height:" << ui->splitter_7->height();

    qDebug() << "Labels:";
    qDebug() << "Label 1 - Width:" << ui->label->width() << "Height:" << ui->label->height();
    qDebug() << "Label 2 - Width:" << ui->label_2->width() << "Height:" << ui->label_2->height();
    qDebug() << "Label 3 - Width:" << ui->label_3->width() << "Height:" << ui->label_3->height();
    qDebug() << "Label 4 - Width:" << ui->label_4->width() << "Height:" << ui->label_4->height();

    // Similarly for widgets
    qDebug() << "Widgets:";
    qDebug() << "Widget 4 - Width:" << ui->widget_4->width() << "Height:" << ui->widget_4->height();
    qDebug() << "Widget 6 - Width:" << ui->widget_6->width() << "Height:" << ui->widget_6->height();
    qDebug() << "Widget 7 - Width:" << ui->widget_7->width() << "Height:" << ui->widget_7->height();
    qDebug() << "Widget 8 - Width:" << ui->widget_8->width() << "Height:" << ui->widget_8->height();


    file.close();
}

void MainWindow::getAllSplittersWidthAndHeight(ImageState &imageState) {

    // Get width and height for splitter
    systemState.uiDimensions.splitterWidth = ui->splitter->width();
    systemState.uiDimensions.splitterHeight = ui->splitter->height();

    // Get width and height for splitter_2
    systemState.uiDimensions.splitter2Width = ui->splitter_2->width();
    systemState.uiDimensions.splitter2Height = ui->splitter_2->height();

    // Get width and height for splitter_3
    systemState.uiDimensions.splitter3Width = ui->splitter_3->width();
    systemState.uiDimensions.splitter3Height = ui->splitter_3->height();

    // Get width and height for splitter_4
    systemState.uiDimensions.splitter4Width = ui->splitter_4->width();
    systemState.uiDimensions.splitter4Height = ui->splitter_4->height();

    // Get width and height for splitter_5
    systemState.uiDimensions.splitter5Width = ui->splitter_5->width();
    systemState.uiDimensions.splitter5Height = ui->splitter_5->height();

    // Get width and height for splitter_6
    systemState.uiDimensions.splitter6Width = ui->splitter_6->width();
    systemState.uiDimensions.splitter6Height = ui->splitter_6->height();

    // Get width and height for splitter_7
    systemState.uiDimensions.splitter7Width = ui->splitter_7->width();
    systemState.uiDimensions.splitter7Height = ui->splitter_7->height();

    systemState.uiDimensions.labelWidth = ui->label->width();
    systemState.uiDimensions.label2Width = ui->label_2->width();
    systemState.uiDimensions.label3Width = ui->label_3->width();
    systemState.uiDimensions.label4Width = ui->label_4->width();


    systemState.uiDimensions.widget4Height = ui->widget_4->height();
    systemState.uiDimensions.widget6Height = ui->widget_6->height();
    systemState.uiDimensions.widget7Height = ui->widget_7->height();
    systemState.uiDimensions.widget8Height = ui->widget_8->height();


    systemState.uiDimensions.labelWidthRatio = (double)systemState.uiDimensions.labelWidth / systemState.uiDimensions.splitter3Width;
    systemState.uiDimensions.label2WidthRatio = (double)systemState.uiDimensions.label2Width / systemState.uiDimensions.splitter3Width;
    systemState.uiDimensions.label3WidthRatio = (double)systemState.uiDimensions.label3Width / systemState.uiDimensions.splitter3Width;
    systemState.uiDimensions.label4WidthRatio = (double)systemState.uiDimensions.label4Width / systemState.uiDimensions.splitter3Width;

    systemState.uiDimensions.widget4HeightRatio = (double)systemState.uiDimensions.widget4Height / systemState.uiDimensions.splitter6Height;
    systemState.uiDimensions.widget6HeightRatio = (double)systemState.uiDimensions.widget6Height / systemState.uiDimensions.splitter5Height;
    systemState.uiDimensions.widget7HeightRatio = (double)systemState.uiDimensions.widget7Height / systemState.uiDimensions.splitter5Height;
    systemState.uiDimensions.widget8HeightRatio = (double)systemState.uiDimensions.widget8Height / systemState.uiDimensions.splitter7Height;

    systemState.uiDimensions.splitter6HeightRatio = (double)systemState.uiDimensions.splitter6Height / systemState.uiDimensions.splitter7Height;
    systemState.uiDimensions.splitter5HeightRatio = (double)systemState.uiDimensions.splitter5Height / systemState.uiDimensions.splitter6Height;
    systemState.uiDimensions.splitterHeightRatio = (double)systemState.uiDimensions.splitterHeight / systemState.uiDimensions.splitter5Height;
    systemState.uiDimensions.splitter2HeightRatio = (double)systemState.uiDimensions.splitter2Height / systemState.uiDimensions.splitter5Height;
    systemState.uiDimensions.splitter4WidthRatio = (double)systemState.uiDimensions.splitter4Width / systemState.uiDimensions.splitter7Width;
    systemState.uiDimensions.splitter3WidthRatio = (double)systemState.uiDimensions.splitter3Width / systemState.uiDimensions.splitter7Width;

    qDebug() << "Splitter h r to s3: " << systemState.uiDimensions.splitterHeightRatio;
    qDebug() << "Splitter2 h r to s3" << systemState.uiDimensions.splitter2HeightRatio;
    qDebug() << "Splitter3 w r to s5" << systemState.uiDimensions.splitter3WidthRatio;
    qDebug() << "Splitter4 w r to s5" << systemState.uiDimensions.splitter4WidthRatio;
    qDebug() << "l1  w r to s2" << systemState.uiDimensions.labelWidthRatio;
    qDebug() << "l2 w r to s2" << systemState.uiDimensions.label2WidthRatio;
    qDebug() << "l3 w r to s" << systemState.uiDimensions.label3WidthRatio;
    qDebug() << "l4 w r to s" << systemState.uiDimensions.label4WidthRatio;
    qDebug() << "w4 h r to s6" << systemState.uiDimensions.widget4HeightRatio;
    qDebug() << "s5 h r to s6" << systemState.uiDimensions.splitter5HeightRatio;
    qDebug() << "w6 h r to s4" << systemState.uiDimensions.widget6HeightRatio;
    qDebug() << "w7 h r to s4" << systemState.uiDimensions.widget7HeightRatio;
    qDebug() << "w8 h r to s7" << systemState.uiDimensions.widget8HeightRatio;
    qDebug() << "s6 h r to s7" << systemState.uiDimensions.splitter6HeightRatio;



    qDebug() << "Splittersssssssssssssssssssssss:";
    qDebug() << "Splitter 1 - Width:" << ui->splitter->width() << "Height:" << ui->splitter->height();
    qDebug() << "Splitter 2 - Width:" << ui->splitter_2->width() << "Height:" << ui->splitter_2->height();
    qDebug() << "Splitter 3 - Width:" << ui->splitter_3->width() << "Height:" << ui->splitter_3->height();
    qDebug() << "Splitter 4 - Width:" << ui->splitter_4->width() << "Height:" << ui->splitter_4->height();
    qDebug() << "Splitter 5 - Width:" << ui->splitter_5->width() << "Height:" << ui->splitter_5->height();
    qDebug() << "Splitter 6 - Width:" << ui->splitter_6->width() << "Height:" << ui->splitter_6->height();
    qDebug() << "Splitter 7 - Width:" << ui->splitter_7->width() << "Height:" << ui->splitter_7->height();

    qDebug() << "Labels:";
    qDebug() << "Label 1 - Width:" << ui->label->width() << "Height:" << ui->label->height();
    qDebug() << "Label 2 - Width:" << ui->label_2->width() << "Height:" << ui->label_2->height();
    qDebug() << "Label 3 - Width:" << ui->label_3->width() << "Height:" << ui->label_3->height();
    qDebug() << "Label 4 - Width:" << ui->label_4->width() << "Height:" << ui->label_4->height();

    // Similarly for widgets
    qDebug() << "Widgets:";
    qDebug() << "Widget 4 - Width:" << ui->widget_4->width() << "Height:" << ui->widget_4->height();
    qDebug() << "Widget 6 - Width:" << ui->widget_6->width() << "Height:" << ui->widget_6->height();
    qDebug() << "Widget 7 - Width:" << ui->widget_7->width() << "Height:" << ui->widget_7->height();
    qDebug() << "Widget 8 - Width:" << ui->widget_8->width() << "Height:" << ui->widget_8->height();

    saveRatiosToConfig(imageState, "ratios.config",systemState);


}

void MainWindow::saveDesign()
{
    getAllSplittersWidthAndHeight(imageState);

}



void MainWindow::exitApplication() {
    QApplication::quit(); // Quit the application
}

void MainWindow::openShortCutsDialog() {
    // Implementation for opening the shortcuts dialog
    // For example:
    ShortcutsDialog *dialog = new ShortcutsDialog(this);
    dialog->exec();
}

void MainWindow::openEditDialog() {
    // Implementation for opening the shortcuts dialog
    // For example:
    edit *editWindow = new edit( systemState,this);
    editWindow->show();
    // Inside the constructor or an appropriate place in MainWindow
    connect(editWindow, &edit::SVerticalRotationToLeft, this, &MainWindow::on_VerticalRotationToLeft_clicked);
    connect(editWindow, &edit::SVerticalRotationToRight, this, &MainWindow::on_VerticalRotationToRight_clicked);
    connect(editWindow, &edit::SHorizontalRotationToLeft, this, &MainWindow::on_HorizontalRotationToLeft_clicked);
    connect(editWindow, &edit::SHorizontalRotationToRight, this, &MainWindow::on_HorizontalRotationToRight_clicked);
    connect(editWindow, &edit::SincreaseVRotationAngle, this, &MainWindow::on_increaseVRotationAngle_clicked);
    connect(editWindow, &edit::SdecreaseVRotationAngle, this, &MainWindow::on_decreaseVRotationAngle_clicked);
    connect(editWindow, &edit::SincreaseMovingRatio, this, &MainWindow::on_increaseMovingRatio_clicked);
    connect(editWindow, &edit::SdecreaseMovingRatio, this, &MainWindow::on_decreaseMovingRatio_clicked);
    connect(editWindow, &edit::SincreaseZoomRatio, this, &MainWindow::on_increaseZoomRatio_clicked);
    connect(editWindow, &edit::SdecreaseZoomRatio, this, &MainWindow::on_decreaseZoomRatio_clicked);
    connect(editWindow, &edit::SincreaseHRotationAngle, this, &MainWindow::on_increaseHRotationAngle_clicked);
    connect(editWindow, &edit::SdecreaseHRotationAngle, this, &MainWindow::on_decreaseHRotationAngle_clicked);

}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event); // Call the base class implementation

    // Set the size of your widget to match the size of the MainWindow
    ui -> widget->setGeometry(0, 0, this->width(), this->height());

    // Calculate the new width for splitter_6 based on the width of ui->widget
    int newSplitterWidth = ui->widget->width() -50; // Example: use the width of ui->widget

    int THeight = ui->widget->height(); // Example: use the width of ui->widget
    int widget4Height = ui->widget_4->height();
    int widget8Height = ui->widget_8->height();
    int splitter6Hight = ui->splitter_6->height();

    float ratio1 = widget4Height / THeight;
    float ratio2 = widget8Height / THeight;
    float ratio3 = splitter6Hight / THeight;


     ui->splitter_6->setMinimumHeight(THeight * ratio3);
     ui->splitter_3->setMinimumHeight(THeight * ratio3);
      ui->splitter_4->setMinimumHeight(THeight * ratio3);
      ui->splitter_5->setMinimumHeight(THeight * ratio3);

      ui->splitter_2->setMinimumHeight(THeight * ratio3 *0.5);
      ui->splitter->setMinimumHeight(THeight * ratio3 *0.5);

     ui->widget_8->setMinimumHeight(THeight * ratio2);
     ui->widget_6->setMinimumHeight(THeight * ratio3 *0.5);
     ui->widget_7->setMinimumHeight(THeight * ratio3 *0.5);
     ui->widget_4->setMinimumHeight(THeight * ratio1);

    ui->label->setMinimumHeight(THeight * ratio3 *0.5);
    ui->label_2->setMinimumHeight(THeight * ratio3 *0.5);

    ui->label_3->setMinimumHeight(THeight * ratio3 *0.5);
    ui->label_4->setMinimumHeight(THeight * ratio3 *0.5);
    //ui->splitter_5->setMinimumHeight(newSplitterHeight);


    // Set the new width for splitter_6
    ui->splitter_6->setMinimumWidth(newSplitterWidth);
     ui->splitter_5->setMinimumWidth(newSplitterWidth);
     ui->splitter_3->setMinimumWidth(newSplitterWidth * 0.8);
      ui->splitter_4->setMinimumWidth(newSplitterWidth * 0.2);
      ui->splitter_2->setMinimumWidth(newSplitterWidth * 0.5);
      ui->splitter->setMinimumWidth(newSplitterWidth * 0.5);


     ui->label->setMinimumWidth(newSplitterWidth * 0.25);
     ui->label_2->setMinimumWidth(newSplitterWidth * 0.25);

     ui->label_3->setMinimumWidth(newSplitterWidth * 0.25);
     ui->label_4->setMinimumWidth(newSplitterWidth * 0.25);




}

void MainWindow::labelsSize() {
    // Print sizes of labels whenever a splitter is moved
    qDebug() << "Label sizes:";
    qDebug() << "label: " << ui->label->size();
    qDebug() << "label_2: " << ui->label_2->size();
    qDebug() << "label_3: " << ui->label_3->size();
    qDebug() << "label_4: " << ui->label_4->size();
    qDebug() << "s1: " << ui->splitter->size();
    qDebug() << "s2: " << ui->splitter_2->size();
    qDebug() << "s3: " << ui->splitter_3->size();
    qDebug() << "s4: " << ui->splitter_4->size();
    qDebug() << "s5: " << ui->splitter_5->size();
    qDebug() << "s6: " << ui->splitter_6->size();
    qDebug() << "s7: " << ui->splitter_7->size();
}



//ratio.h
void MainWindow::on_increaseVRotationAngle_clicked() {
    try {
        updateRotatingRatio(imageState,0.2,systemState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_decreaseVRotationAngle_clicked() {
    try {
        updateRotatingRatio(imageState,-0.2,systemState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_increaseHRotationAngle_clicked()
{
    try {
        updateHRotatingRatio(imageState,0.05,systemState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_decreaseHRotationAngle_clicked()
{
    try {
        updateHRotatingRatio(imageState,-0.05,systemState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_increaseMovingRatio_clicked() {
    try {
        updateMovingRatio(imageState,0.2,systemState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_decreaseMovingRatio_clicked() {
    try {
        updateMovingRatio(imageState,-0.2,systemState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_increaseZoomRatio_clicked() {
    try {
        updateZoomingRatio(imageState,0.5,systemState);
        drawImageWithPoints(imageState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_decreaseZoomRatio_clicked() {
    try {
        updateZoomingRatio(imageState,-0.5,systemState);
        drawImageWithPoints(imageState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}


//txtOperation.h
void MainWindow::on_save_clicked() {
    try {
        saveImageData(imageState,systemState);
        ui->label_23->setText("Saved successfully");

        // Show a message box indicating successful saving
        QMessageBox* messageBox = new QMessageBox(QMessageBox::Information, "Saved", "Saved successfully", QMessageBox::Ok, this);
        messageBox->show();

        // Create a QTimer to close the message box after 2 seconds
        QTimer::singleShot(1000, messageBox, &QMessageBox::deleteLater);

        systemState.flags.saved = true;

        // Create a QTimer to hide the message after 3 seconds
        QTimer::singleShot(2000, this, [this]() {
            ui->label_23->setText("Keep Going!");
        });
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

// viweWindows.h
void MainWindow::on_pushButton_34_clicked() {
    // Check if the window is already open
    if (!systemState.pointers.RealW || systemState.pointers.RealW.isNull()) {
        // If not, create a new instance
        systemState.pointers.RealW = new real(imageState.images.modifiedImageTrap, this);
        systemState.pointers.RealW->setAttribute(Qt::WA_DeleteOnClose); // Optional: Delete window on close
        systemState.pointers.RealW->setFocusPolicy(Qt::NoFocus);
        systemState.pointers.RealW->show();
    } else {
        // If already open, just bring it to front
        systemState.pointers.RealW->updateRealImage(imageState.images.modifiedImageTrap);


    }
    systemState.pointers.RealW->updateRealImage(imageState.images.modifiedImageTrap);

}

void MainWindow::on_pushButton_35_clicked() {
    // Check if the window is already open
    if (!systemState.pointers.PlayGroundW || systemState.pointers.PlayGroundW.isNull()) {
        // If not, create a new instance
        systemState.pointers.PlayGroundW = new playground(imageState.images.modifiedImageRect, this);
        systemState.pointers.PlayGroundW->setAttribute(Qt::WA_DeleteOnClose); // Optional: Delete window on close
        systemState.pointers.PlayGroundW->setFocusPolicy(Qt::NoFocus);
        systemState.pointers.PlayGroundW->show();
    } else {
        // If already open, just bring it to front
        systemState.pointers.PlayGroundW->updatePlayGroundImage(imageState.images.modifiedImageRect);

    }
    systemState.pointers.PlayGroundW->updatePlayGroundImage(imageState.images.modifiedImageRect);
}

void MainWindow::on_pushButton_36_clicked() {
    // Check if the window is already open
    if (!systemState.pointers.RealToPredW || systemState.pointers.RealToPredW.isNull()) {
        // If not, create a new instance
        systemState.pointers.RealToPredW = new realtopred(imageState.images.blendedImageLeft, this);
        systemState.pointers.RealToPredW->setAttribute(Qt::WA_DeleteOnClose); // Optional: Delete window on close
        systemState.pointers.RealToPredW->setFocusPolicy(Qt::NoFocus);
        systemState.pointers.RealToPredW->show();
    } else {
        // If already open, just bring it to front
        systemState.pointers.RealToPredW->updateRealToPredImage(imageState.images.blendedImageLeft);
    }

    systemState.pointers.RealToPredW->updateRealToPredImage(imageState.images.blendedImageLeft);

}

void MainWindow::on_pushButton_37_clicked() {
    // Check if the window is already open
    if (!systemState.pointers.PredToRealW || systemState.pointers.PredToRealW.isNull()) {
        // If not, create a new instance
        systemState.pointers.PredToRealW = new predtoreal(imageState.images.blendedImageRight, this);
        systemState.pointers.PredToRealW->setAttribute(Qt::WA_DeleteOnClose); // Optional: Delete window on close
        systemState.pointers.PredToRealW->setFocusPolicy(Qt::NoFocus);
        systemState.pointers.PredToRealW->show();
    } else {
        // If already open, just bring it to front
        systemState.pointers.PredToRealW->updatePredToRealImage(imageState.images.blendedImageLeft);
    }

    systemState.pointers.PredToRealW->updatePredToRealImage(imageState.images.blendedImageRight);

}



// moveButtons.h
void MainWindow::on_RectTopSideToUp_clicked() {
    try {
        imageState.points.RectTopLeft.y -= systemState.ratios.movingratio; // point1Rect
        imageState.points.RectBottomRight.y -= systemState.ratios.movingratio; // point3Rect

        imageState.points.centerRect = calculateCenter(imageState.points.RectTopLeft, imageState.points.RectTopRight, imageState.points.RectBottomRight, imageState.points.RectBottomLeft); // centerRect

        // Redraw the image with the updated points for label_2
        drawImageWithPoints2(imageState);
        applyPerspectiveTransformation(imageState.images.originalImageTrap, imageState.images.originalImageRect, imageState, systemState);

        drawPerspctive(imageState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_RectTopSideToDown_clicked() {
    try {
        imageState.points.RectTopLeft.y += systemState.ratios.movingratio; // point1Rect
        imageState.points.RectBottomRight.y += systemState.ratios.movingratio; // point3Rect

        imageState.points.centerRect = calculateCenter(imageState.points.RectTopLeft, imageState.points.RectTopRight, imageState.points.RectBottomRight, imageState.points.RectBottomLeft); // centerRect

        // Redraw the image with the updated points for label_2
        drawImageWithPoints2(imageState);
        applyPerspectiveTransformation(imageState.images.originalImageTrap, imageState.images.originalImageRect, imageState,systemState);

        drawPerspctive(imageState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_RectBottomSideToUp_clicked() {
    try {
        imageState.points.RectTopRight.y -= systemState.ratios.movingratio; // point2Rect
        imageState.points.RectBottomLeft.y -= systemState.ratios.movingratio; // point4Rect

        imageState.points.centerRect = calculateCenter(imageState.points.RectTopLeft, imageState.points.RectTopRight, imageState.points.RectBottomRight, imageState.points.RectBottomLeft); // centerRect

        // Redraw the image with the updated points for label_2
        drawImageWithPoints2(imageState);
        applyPerspectiveTransformation(imageState.images.originalImageTrap, imageState.images.originalImageRect, imageState,systemState);

        drawPerspctive(imageState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_RectBottomSideToDown_clicked() {
    try {
        imageState.points.RectTopRight.y += systemState.ratios.movingratio; // point2Rect
        imageState.points.RectBottomLeft.y += systemState.ratios.movingratio; // point4Rect

        imageState.points.centerRect = calculateCenter(imageState.points.RectTopLeft, imageState.points.RectTopRight, imageState.points.RectBottomRight, imageState.points.RectBottomLeft); // centerRect

        // Redraw the image with the updated points for label_2
        drawImageWithPoints2(imageState);
        applyPerspectiveTransformation(imageState.images.originalImageTrap, imageState.images.originalImageRect, imageState,systemState);

        drawPerspctive(imageState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_RectLeftSideToRight_clicked() {
    try {
        imageState.points.RectTopLeft.x += systemState.ratios.movingratio; // point1Rect
        imageState.points.RectTopRight.x += systemState.ratios.movingratio; // point2Rect

        imageState.points.centerRect = calculateCenter(imageState.points.RectTopLeft, imageState.points.RectTopRight, imageState.points.RectBottomRight, imageState.points.RectBottomLeft); // centerRect

        // Redraw the image with the updated points for label_2
        drawImageWithPoints2(imageState);
        applyPerspectiveTransformation(imageState.images.originalImageTrap, imageState.images.originalImageRect, imageState,systemState);

        drawPerspctive(imageState);

    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_RectLeftSideToLeft_clicked() {
    try {
        imageState.points.RectTopLeft.x -= systemState.ratios.movingratio; // point1Rect
        imageState.points.RectTopRight.x -= systemState.ratios.movingratio; // point2Rect

        imageState.points.centerRect = calculateCenter(imageState.points.RectTopLeft, imageState.points.RectTopRight, imageState.points.RectBottomRight, imageState.points.RectBottomLeft); // centerRect

        // Redraw the image with the updated points for label_2
        drawImageWithPoints2(imageState);
        applyPerspectiveTransformation(imageState.images.originalImageTrap, imageState.images.originalImageRect, imageState,systemState);

        drawPerspctive(imageState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_RectRightSideToLeft_clicked() {
    try {
        imageState.points.RectBottomRight.x -= systemState.ratios.movingratio; // point3Rect
        imageState.points.RectBottomLeft.x -= systemState.ratios.movingratio; // point4Rect

        imageState.points.centerRect = calculateCenter(imageState.points.RectTopLeft, imageState.points.RectTopRight, imageState.points.RectBottomRight, imageState.points.RectBottomLeft); // centerRect

        // Redraw the image with the updated points for label_2
        drawImageWithPoints2(imageState);
        applyPerspectiveTransformation(imageState.images.originalImageTrap, imageState.images.originalImageRect, imageState,systemState);

        drawPerspctive(imageState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_RectRightSideToRight_clicked() {
    try {
        imageState.points.RectBottomRight.x += systemState.ratios.movingratio; // point3Rect
        imageState.points.RectBottomLeft.x += systemState.ratios.movingratio; // point4Rect

        imageState.points.centerRect = calculateCenter(imageState.points.RectTopLeft, imageState.points.RectTopRight, imageState.points.RectBottomRight, imageState.points.RectBottomLeft); // centerRect

        // Redraw the image with the updated points for label_2
        drawImageWithPoints2(imageState);
        applyPerspectiveTransformation(imageState.images.originalImageTrap, imageState.images.originalImageRect, imageState,systemState);

        drawPerspctive(imageState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_AllRectToUp_clicked() {
    try {
        imageState.points.RectTopLeft.y -= systemState.ratios.movingratio; // point1Rect
        imageState.points.RectTopRight.y -= systemState.ratios.movingratio; // point2Rect
        imageState.points.RectBottomRight.y -= systemState.ratios.movingratio; // point3Rect
        imageState.points.RectBottomLeft.y -= systemState.ratios.movingratio; // point4Rect

        imageState.points.centerRect = calculateCenter(imageState.points.RectTopLeft, imageState.points.RectTopRight, imageState.points.RectBottomRight, imageState.points.RectBottomLeft); // centerRect

        // Redraw the image with the updated points for label_2
        drawImageWithPoints2(imageState);
        applyPerspectiveTransformation(imageState.images.originalImageTrap, imageState.images.originalImageRect, imageState,systemState);

        drawPerspctive(imageState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_AllRectToDown_clicked() {
    try {
        imageState.points.RectTopLeft.y += systemState.ratios.movingratio; // point1Rect
        imageState.points.RectTopRight.y += systemState.ratios.movingratio; // point2Rect
        imageState.points.RectBottomRight.y += systemState.ratios.movingratio; // point3Rect
        imageState.points.RectBottomLeft.y += systemState.ratios.movingratio; // point4Rect

        imageState.points.centerRect = calculateCenter(imageState.points.RectTopLeft, imageState.points.RectTopRight, imageState.points.RectBottomRight, imageState.points.RectBottomLeft); // centerRect

        // Redraw the image with the updated points for label_2
        drawImageWithPoints2(imageState);
        applyPerspectiveTransformation(imageState.images.originalImageTrap, imageState.images.originalImageRect, imageState,systemState);

        drawPerspctive(imageState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_AllRectToLeft_clicked() {
    try {
        imageState.points.RectTopLeft.x -= systemState.ratios.movingratio; // point1Rect
        imageState.points.RectTopRight.x -= systemState.ratios.movingratio; // point2Rect
        imageState.points.RectBottomRight.x -= systemState.ratios.movingratio; // point3Rect
        imageState.points.RectBottomLeft.x -= systemState.ratios.movingratio; // point4Rect

        imageState.points.centerRect = calculateCenter(imageState.points.RectTopLeft, imageState.points.RectTopRight, imageState.points.RectBottomRight, imageState.points.RectBottomLeft); // centerRect

        // Redraw the image with the updated points for label_2
        drawImageWithPoints2(imageState);
        applyPerspectiveTransformation(imageState.images.originalImageTrap, imageState.images.originalImageRect, imageState,systemState);

        drawPerspctive(imageState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_AllRectToRight_clicked() {
    try {
        imageState.points.RectTopLeft.x += systemState.ratios.movingratio; // point1Rect
        imageState.points.RectTopRight.x += systemState.ratios.movingratio; // point2Rect
        imageState.points.RectBottomRight.x += systemState.ratios.movingratio; // point3Rect
        imageState.points.RectBottomLeft.x += systemState.ratios.movingratio; // point4Rect

        imageState.points.centerRect = calculateCenter(imageState.points.RectTopLeft, imageState.points.RectTopRight, imageState.points.RectBottomRight, imageState.points.RectBottomLeft); // centerRect

        // Redraw the image with the updated points for label_2
        drawImageWithPoints2(imageState);
        applyPerspectiveTransformation(imageState.images.originalImageTrap, imageState.images.originalImageRect, imageState,systemState);

        drawPerspctive(imageState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_TrapTopSideToUp_clicked() {
    try {
        imageState.points.TrapTopLeft.y -= systemState.ratios.movingratio; // point1Trap
        imageState.points.TrapTopRight.y -= systemState.ratios.movingratio; // point2Trap

        // Redraw the image with the updated points for label_2
        imageState.points.centerTrap = calculateCenter(imageState.points.TrapTopLeft, imageState.points.TrapTopRight, imageState.points.TrapBottomRight, imageState.points.TrapBottomLeft); // centerTrap
        drawImageWithPoints(imageState);
        applyPerspectiveTransformation(imageState.images.originalImageTrap, imageState.images.originalImageRect, imageState,systemState);

        drawPerspctive(imageState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_TrapTopSideToDown_clicked() {
    try {
        imageState.points.TrapTopLeft.y += systemState.ratios.movingratio; // point1Trap
        imageState.points.TrapTopRight.y += systemState.ratios.movingratio; // point2Trap

        // Redraw the image with the updated points for label_2
        imageState.points.centerTrap = calculateCenter(imageState.points.TrapTopLeft, imageState.points.TrapTopRight, imageState.points.TrapBottomRight, imageState.points.TrapBottomLeft); // centerTrap
         drawImageWithPoints(imageState);
        applyPerspectiveTransformation(imageState.images.originalImageTrap, imageState.images.originalImageRect, imageState,systemState);

        drawPerspctive(imageState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_TrapBottomSideToUp_clicked() {
    try {
        imageState.points.TrapBottomRight.y -= systemState.ratios.movingratio; // point3Trap
        imageState.points.TrapBottomLeft.y -= systemState.ratios.movingratio; // point4Trap

        // Redraw the image with the updated points for label_2
        imageState.points.centerTrap = calculateCenter(imageState.points.TrapTopLeft, imageState.points.TrapTopRight, imageState.points.TrapBottomRight, imageState.points.TrapBottomLeft); // centerTrap
        drawImageWithPoints(imageState);
        applyPerspectiveTransformation(imageState.images.originalImageTrap, imageState.images.originalImageRect, imageState,systemState);

        drawPerspctive(imageState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_TrapBottomSideToDown_clicked() {
    try {
        imageState.points.TrapBottomRight.y += systemState.ratios.movingratio; // point3Trap
        imageState.points.TrapBottomLeft.y += systemState.ratios.movingratio; // point4Trap

        // Redraw the image with the updated points for label_2
        imageState.points.centerTrap = calculateCenter(imageState.points.TrapTopLeft, imageState.points.TrapTopRight, imageState.points.TrapBottomRight, imageState.points.TrapBottomLeft); // centerTrap
       drawImageWithPoints(imageState);
        applyPerspectiveTransformation(imageState.images.originalImageTrap, imageState.images.originalImageRect, imageState,systemState);

        drawPerspctive(imageState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}


void MainWindow::on_VerticalRotationToLeft_clicked() {
    try {
        VrotateAndCalculateScore(true, imageState, systemState); // Rotate to left
        drawPerspctive(imageState);
    } catch (const std::exception& e) {
        qDebug() << "Exception caught in on_VerticalRotationToLeft_clicked(): " << e.what();
        // Handle the exception here
    }
}

void MainWindow::on_VerticalRotationToRight_clicked() {
    try {
        VrotateAndCalculateScore(false, imageState, systemState); // Rotate to right
        drawPerspctive(imageState);
    } catch (const std::exception& e) {
        qDebug() << "Exception caught in on_VerticalRotationToRight_clicked(): " << e.what();
        // Handle the exception here
    }
}

void MainWindow::on_HorizontalRotationToLeft_clicked() {
    try {
        HrotateAndCalculateScore(true, imageState, systemState); // Rotate to left
        drawPerspctive(imageState);
    } catch (const std::exception& e) {
        qDebug() << "Exception caught in on_HorizontalRotationToLeft_clicked(): " << e.what();
        // Handle the exception here
    }
}

void MainWindow::on_HorizontalRotationToRight_clicked() {
    try {
        HrotateAndCalculateScore(false, imageState, systemState); // Rotate to right
        drawPerspctive(imageState);
    } catch (const std::exception& e) {
        qDebug() << "Exception caught in on_HorizontalRotationToRight_clicked(): " << e.what();
        // Handle the exception here
    }
}


//horizontalSlider.h
void MainWindow::on_nextImage_clicked() {
    try {
        if (systemState.flags.load==false) {
        // Get the current slider position
                if (!systemState.flags.saved) {
                    if (imageState.score != imageState.imgscore) //(imageState.flags.Edited == true)
                   {
                        // Points for either label_1 or label_2 have changed
                        QMessageBox::StandardButton reply;
                        reply = QMessageBox::question(this, "Message", "You didn't save. Do you want to move now?",
                                                      QMessageBox::Yes | QMessageBox::No);
                        if (reply == QMessageBox::Yes) {
                            int currentPosition = ui->horizontalSlider->value();

                            // Increment the slider position to move to the next image
                            int newPosition = currentPosition + 1;

                            // Ensure the new position is within the valid range
                            if (newPosition < ui->horizontalSlider->maximum() + 1) {
                                // Set the new slider position
                                ui->horizontalSlider->setValue(newPosition);

                                // Update the displayed image based on the new slider position
                                loadAndDisplayImage(newPosition, imageState,systemState);
                                systemState.stringData.labelSize1 = ui->label->size();
                                loadscoreImage(newPosition, imageState,systemState.stringData.labelSize1,systemState);
                                systemState.flags.saved = false;
                            }
                        } else {
                            systemState.flags.saved = true;
                        }
                    } else {
                        int currentPosition = ui->horizontalSlider->value();

                        // Increment the slider position to move to the next image
                        int newPosition = currentPosition + 1;

                        // Ensure the new position is within the valid range
                        if (newPosition < ui->horizontalSlider->maximum() + 1) {
                            // Set the new slider position
                            ui->horizontalSlider->setValue(newPosition);

                            // Update the displayed image based on the new slider position
                            loadAndDisplayImage(newPosition, imageState,systemState);
                            systemState.stringData.labelSize1 = ui->label->size();
                            loadscoreImage(newPosition, imageState,systemState.stringData.labelSize1,systemState);
                            systemState.flags.saved = false;
                        }
                    }
        }

                else {
                    int currentPosition = ui->horizontalSlider->value();

                    // Increment the slider position to move to the next image
                    int newPosition = currentPosition + 1;

                    // Ensure the new position is within the valid range
                    if (newPosition < ui->horizontalSlider->maximum() + 1) {
                        // Set the new slider position
                        ui->horizontalSlider->setValue(newPosition);

                        // Update the displayed image based on the new slider position
                        loadAndDisplayImage(newPosition, imageState,systemState);
                        systemState.stringData.labelSize1 = ui->label->size();
                        loadscoreImage(newPosition, imageState,systemState.stringData.labelSize1,systemState);
                        systemState.flags.saved = false;
                    }
                }

        }else {
            int currentPosition = ui->horizontalSlider->value();

            // Increment the slider position to move to the next image
            int newPosition = currentPosition + 1;

            // Ensure the new position is within the valid range
            if (newPosition < ui->horizontalSlider->maximum() + 1) {
                // Set the new slider position
                ui->horizontalSlider->setValue(newPosition);

                // Update the displayed image based on the new slider position
                loadAndDisplayImage(newPosition, imageState,systemState);
                systemState.stringData.labelSize1 = ui->label->size();
                loadscoreImage(newPosition, imageState,systemState.stringData.labelSize1,systemState);
                systemState.flags.saved = false;
            }
        }
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_previousImage_clicked() {
    try {
        if (systemState.flags.load==false) {
                if (!systemState.flags.saved) {
                    if (imageState.score != imageState.imgscore) {


                        QMessageBox::StandardButton reply;
                        reply = QMessageBox::question(this, "Message", "You didn't save. Do you want to move now?",
                                                      QMessageBox::Yes | QMessageBox::No);
                        if (reply == QMessageBox::Yes) {
                            int currentPosition = ui->horizontalSlider->value();
                            int newPosition = currentPosition - 1;
                            if (newPosition >= ui->horizontalSlider->minimum()) {
                                ui->horizontalSlider->setValue(newPosition);
                                loadAndDisplayImage(newPosition, imageState,systemState);
                                systemState.stringData.labelSize1 = ui->label->size();
                                loadscoreImage(newPosition, imageState,systemState.stringData.labelSize1,systemState);
                                systemState.flags.saved = false;
                            }
                        } else {
                            systemState.flags.saved = true;
                        }
                    } else {
                        int currentPosition = ui->horizontalSlider->value();
                        int newPosition = currentPosition - 1;
                        if (newPosition >= ui->horizontalSlider->minimum()) {
                            ui->horizontalSlider->setValue(newPosition);
                            loadAndDisplayImage(newPosition, imageState,systemState);
                            systemState.stringData.labelSize1 = ui->label->size();
                            loadscoreImage(newPosition, imageState,systemState.stringData.labelSize1,systemState);
                            systemState.flags.saved = false;
                        }
                    }

     }else {
                    int currentPosition = ui->horizontalSlider->value();

                    // Increment the slider position to move to the next image
                    int newPosition = currentPosition + 1;

                    // Ensure the new position is within the valid range
                    if (newPosition < ui->horizontalSlider->maximum() + 1) {
                        // Set the new slider position
                        ui->horizontalSlider->setValue(newPosition);

                        // Update the displayed image based on the new slider position
                        loadAndDisplayImage(newPosition, imageState,systemState);
                        systemState.stringData.labelSize1 = ui->label->size();
                        loadscoreImage(newPosition, imageState,systemState.stringData.labelSize1,systemState);
                        systemState.flags.saved = false;
                    }
                }

        } else {
            int currentPosition = ui->horizontalSlider->value();
            int newPosition = currentPosition - 1;
            if (newPosition >= ui->horizontalSlider->minimum()) {
                ui->horizontalSlider->setValue(newPosition);
                loadAndDisplayImage(newPosition, imageState,systemState);
                systemState.stringData.labelSize1 = ui->label->size();
                loadscoreImage(newPosition, imageState,systemState.stringData.labelSize1,systemState);
                systemState.flags.saved = false;
            }
        }
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_next10Image_clicked() {
    try {
        // Get the current slider position
        int currentPosition = ui->horizontalSlider->value();

        // Increment the slider position by 10 to move to the next 10 images
        int newPosition = currentPosition + 10;

        // Ensure the new position is within the valid range
        if (newPosition <= ui->horizontalSlider->maximum() + 1) {
            // Set the new slider position
            ui->horizontalSlider->setValue(newPosition);

            // Update the displayed image based on the new slider position
            loadAndDisplayImage(newPosition, imageState,systemState);
            systemState.stringData.labelSize1 = ui->label->size();
            loadscoreImage(newPosition, imageState,systemState.stringData.labelSize1,systemState);
        }
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_previous10Image_clicked() {
    try {
        // Get the current slider position
        int currentPosition = ui->horizontalSlider->value();

        // Decrement the slider position by 10 to move to the previous 10 images
        int newPosition = currentPosition - 10;

        // Ensure the new position is within the valid range
        if (newPosition >= ui->horizontalSlider->minimum()) {
            // Set the new slider position
            ui->horizontalSlider->setValue(newPosition);

            // Update the displayed image based on the new slider position
            // Update the displayed image based on the new slider position
            loadAndDisplayImage(newPosition, imageState,systemState);
            systemState.stringData.labelSize1 = ui->label->size();
            loadscoreImage(newPosition, imageState,systemState.stringData.labelSize1,systemState);
        }
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}




void MainWindow::readConfigFile(const QString &filePath) {
    QFile configFile(filePath);
    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open config file for reading.";
        return;
    }

    QTextStream in(&configFile);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split('=');
        if (parts.size() == 2) {
            QString key = parts[0].trimmed();
            QString value = parts[1].trimmed();
            if (key == "imgFolderName") {
                imageState.images.imgFolderName = value;
            } else if (key == "scoreImgsName") {
                imageState.images.scoreImgsName = value;
            }
              else if (key == "solutionsFolderName") {
                systemState.stringData.solutions = value;
                systemState.stringData.tempForLoad = systemState.stringData.solutions;

            }
        }
    }
    configFile.close();
}

void MainWindow::on_openFolder_clicked()
{
    try {
        systemState.flags.load=false;
        // Open a folder dialog to select a directory
        QString folderPath = QFileDialog::getExistingDirectory(this, tr("Select Folder"), QDir::homePath());
        readConfigFile("ratios.config");

        if (!folderPath.isEmpty()) {
            // Change the working directory to the selected folder
            QDir::setCurrent(folderPath);

            // Check if the "imgs" subfolder exists
            QDir imgsFolder(folderPath + QDir::separator() + imageState.images.imgFolderName);
            if (imgsFolder.exists()) {
                // Clear the imageFileList before adding new items
                systemState.stringData.imageFileList.clear();

                // Set the directory to the "imgs" subfolder
                QDir directory(imgsFolder);

                // Filter JPG files
                QStringList filters;
                filters << "*.jpg" << "*.png";
                directory.setNameFilters(filters);

                // Get the list of JPG files in the directory
                QFileInfoList fileList = directory.entryInfoList();

                // Add each JPG file name to the imageFileList
                foreach (const QFileInfo &fileInfo, fileList) {
                    systemState.stringData.imageFileList.append(fileInfo.fileName());
                }

                // Set the maximum value of the slider based on the number of images
                ui->horizontalSlider->setMaximum(systemState.stringData.imageFileList.size() - 1);

                // Update the selected folder paths
                systemState.stringData.selectedFolderPath = folderPath + QDir::separator() + imageState.images.imgFolderName;
                systemState.stringData.selectedFolderPath2 = folderPath + QDir::separator() + imageState.images.scoreImgsName;

                // Load and display the first image
                loadAndDisplayImage(0, imageState,systemState);
                systemState.stringData.labelSize1 = ui->label->size();
                loadscoreImage(0,imageState,systemState.stringData.labelSize1,systemState);
                updateSliderSize(imageState);
            } else {
                qDebug() << "The 'imgs' subfolder does not exist in the selected folder.";
            }
        }
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::on_reset_clicked() {
    try {
        // Get the size of the label
        systemState.stringData.labelSize = ui->label->size();
        systemState.stringData.label2Size = ui->label_2->size();
        loadPointsFromTextFile(systemState.stringData.selectedFolderPath, imageState,systemState);
        imageState.points.centerTrap = calculateCenter(imageState.points.TrapTopLeft, imageState.points.TrapTopRight, imageState.points.TrapBottomRight, imageState.points.TrapBottomLeft); // centerTrap
        imageState.points.centerRect = calculateCenter(imageState.points.RectTopLeft, imageState.points.RectTopRight, imageState.points.RectBottomRight, imageState.points.RectBottomLeft); // centerRect
        applyPerspectiveTransformation(imageState.images.originalImageTrap, imageState.images.originalImageRect, imageState,systemState);

        drawPerspctive(imageState);
        drawImageWithPoints2(imageState);
        drawImageWithPoints(imageState);
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
    }
}

void MainWindow::on_load_clicked()
{
    systemState.flags.load=!systemState.flags.load;
    qDebug() << "load mode is" <<systemState.flags.load ;

    if( systemState.flags.load == true){

        ui->label_6->setStyleSheet("background-color: blue; color: white;");
        ui->label_6->setText("Loaded Imges From The Output Folder");
        ui->load->setText("Return To Default Mood");
        ui->actionLoad_Mode->setText("Default Mode");


    }else{

        ui->label_6->setStyleSheet("");
        ui->label_6->setText("Default Mood");
        ui->load->setText("Load From Output Folder");
        ui->actionLoad_Mode->setText("Load Mode");

    }




}





void MainWindow::on_pushButton_5_clicked()
{

    removeLastPoint();

}

