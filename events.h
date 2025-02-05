#ifndef EVENTS_H
#define EVENTS_H

#include "qpainter.h"
#include "qtimer.h"
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QMouseEvent>
#include <iostream>
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
#include "matrix.h"
#include <QFile>
#include <QTextStream>

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    try {
        // Check if the left mouse button is pressed
        if (event->buttons() == Qt::LeftButton) {
            // Get the current position of the mouse
          //  QPoint currentMousePos = event->pos();

            QPoint labelPos2 = ui->label_2->mapToGlobal(QPoint(0, 0));
            QPoint labelPos = ui->label->mapToGlobal(QPoint(0, 0));

            // Get the mouse position relative to the main window
            QPoint globalMousePos = event->globalPos();

            // Get the mouse position relative to the label
            QPoint mousePos2 = globalMousePos - labelPos2;
            QPoint mousePos = globalMousePos - labelPos;

            // Calculate the scaling factor between the label size and the image size
            QSize image2Size(1024, 682); // Fixed image size
            QSize imageSize(1280, 720);
            //QSize imageSize(1920, 1080);

            QSize label2Size = ui->label_2->size();
            QSize labelSize = ui->label->size();

            qreal scale2X = static_cast<qreal>(image2Size.width()) / label2Size.width();
            qreal scale2Y = static_cast<qreal>(image2Size.height()) / label2Size.height();

            qreal scaleX = static_cast<qreal>(imageSize.width()) / labelSize.width();
            qreal scaleY = static_cast<qreal>(imageSize.height()) / labelSize.height();


            // Map the mouse position to the image coordinates
            QPoint image2Pos(mousePos2.x() * scale2X, mousePos2.y() * scale2Y);
            QPoint imagePos(mousePos.x() * scaleX, mousePos.y() * scaleY);
            qDebug() << "yaraab: " << image2Pos;




            QPoint currentMousePos2 = image2Pos;
            QPoint currentMousePos = imagePos;


            // Calculate the distance between each point and the initial mouse click position for label
            int distance1 = (currentMousePos - QPoint(imageState.points.TrapTopLeft.x, imageState.points.TrapTopLeft.y)).manhattanLength(); // point1Trap
            int distance2 = (currentMousePos - QPoint(imageState.points.TrapTopRight.x, imageState.points.TrapTopRight.y)).manhattanLength(); // point2Trap
            int distance3 = (currentMousePos - QPoint(imageState.points.TrapBottomRight.x, imageState.points.TrapBottomRight.y)).manhattanLength(); // point3Trap
            int distance4 = (currentMousePos - QPoint(imageState.points.TrapBottomLeft.x, imageState.points.TrapBottomLeft.y)).manhattanLength(); // point4Trap

            int distanceC2 = (currentMousePos2 - QPoint(imageState.points.centerRect.x , imageState.points.centerRect.y)).manhattanLength();
            int distanceC12 = (currentMousePos - QPoint(imageState.points.centerTrap.x , imageState.points.centerTrap.y)).manhattanLength();

            // Determine the minimum distance among the points for label
            int minDistance = std::min({distance1, distance2, distance3, distance4});

            // Determine the minimum distance among the points for label_2
            int minDistance_3 = std::min({distanceC2});
            int minDistance_4 = std::min({distanceC12});

            // Check if the initial mouse click was close to any point (within a threshold, e.g., 15 pixels) for label
            if (minDistance < 15) {
                // Calculate the offset between the current mouse position and the initial mouse click position
                QPoint offset(currentMousePos.x() - systemState.uiDimensions.initialMousePos1.x(), 0);  // Only horizontal movement

                // Update the position of the appropriate point based on the offset for label
                if (minDistance == distance1) {
                    if (systemState.flags.lock) {
                        updatePointPosition(imageState.points.TrapTopLeft, offset);
                        updatePointPosition(imageState.points.TrapTopRight, -offset);
                    } else {
                        updatePointPosition(imageState.points.TrapTopLeft, offset);
                    }
                } else if (minDistance == distance2) {
                    if (systemState.flags.lock) {
                        updatePointPosition(imageState.points.TrapTopLeft, -offset);
                        updatePointPosition(imageState.points.TrapTopRight, offset);
                    } else {
                        updatePointPosition(imageState.points.TrapTopRight, offset);
                    }
                } else if (minDistance == distance3) {
                    if (systemState.flags.lock) {
                        updatePointPosition(imageState.points.TrapBottomRight, offset);
                        updatePointPosition(imageState.points.TrapBottomLeft, -offset);
                    } else {
                        updatePointPosition(imageState.points.TrapBottomRight, offset);
                    }
                } else if (minDistance == distance4) {
                    if (systemState.flags.lock) {
                        updatePointPosition(imageState.points.TrapBottomLeft, offset);
                        updatePointPosition(imageState.points.TrapBottomRight, -offset);
                    } else {
                        updatePointPosition(imageState.points.TrapBottomLeft, offset);
                    }
                }

                imageState.points.centerTrap = calculateCenter(imageState.points.TrapTopLeft, imageState.points.TrapTopRight, imageState.points.TrapBottomRight, imageState.points.TrapBottomLeft);
                // Redraw the image with updated points for label
                drawImageWithPoints(imageState);
                applyPerspectiveTransformation(imageState.images.originalImageTrap, imageState.images.originalImageRect, imageState,systemState);
                 drawPerspctive(imageState);

            }

            if (minDistance_3 < 15) {
                // Calculate the offset between the current mouse position and the initial mouse click position for label_2
                QPoint offset_2 = currentMousePos2 - systemState.uiDimensions.initialMousePos;

                // Update the position of the appropriate point based on the offset for label_2
                updatePointPosition(imageState.points.RectTopLeft, offset_2); // point1Rect
                updatePointPosition(imageState.points.RectTopRight, offset_2); // point2Rect
                updatePointPosition(imageState.points.RectBottomRight, offset_2); // point3Rect
                updatePointPosition(imageState.points.RectBottomLeft, offset_2); // point4Rect

                updatePointPosition(imageState.points.centerRect, offset_2);
                // Redraw the image with updated points for label_2
                drawImageWithPoints2(imageState);
                applyPerspectiveTransformation(imageState.images.originalImageTrap, imageState.images.originalImageRect, imageState,systemState);
                drawPerspctive(imageState);

            }
            if (minDistance_4 < 15) {
                // Calculate the offset between the current mouse position and the initial mouse click position for label_2
                QPoint offset_5 = currentMousePos - systemState.uiDimensions.initialMousePos1;

                // Update the position of the appropriate point based on the offset for label_2
                updatePointPosition(imageState.points.TrapTopLeft, offset_5); // point1Trap
                updatePointPosition(imageState.points.TrapTopRight, offset_5); // point2Trap
                updatePointPosition(imageState.points.TrapBottomRight, offset_5); // point3Trap
                updatePointPosition(imageState.points.TrapBottomLeft, offset_5); // point4Trap

                updatePointPosition(imageState.points.centerTrap, offset_5);
                // Redraw the image with updated points for label_2
                drawImageWithPoints(imageState);
                applyPerspectiveTransformation(imageState.images.originalImageTrap, imageState.images.originalImageRect, imageState,systemState);
                drawPerspctive(imageState);

            }

            // Update the initial mouse position for the next move event
            systemState.uiDimensions.initialMousePos1 = currentMousePos;
            systemState.uiDimensions.initialMousePos = currentMousePos2;
        } else {
            systemState.flags.isMouseHeldOnPoint = false;  // Reset the flag if the mouse button is released
        }
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

// Function to save points and their associated frame numbers and video names into a CSV file
void MainWindow::mousePressEvent(QMouseEvent *event) {
    try {
        // Check if the left mouse button is pressed
        if (event->button() == Qt::LeftButton) {
            // Get the global mouse position
            QPoint globalMousePos = event->globalPos();

            // Get the position of the concatenated image label
            QPoint labelPos = ui->label->mapToGlobal(QPoint(0, 0));

            // Get the size of the concatenated image
            QSize combinedSize = ui->label->pixmap().size();

            // Calculate the scale factors for both axes
            qreal scaleX = static_cast<qreal>(combinedSize.width()) / ui->label->width();
            qreal scaleY = static_cast<qreal>(combinedSize.height()) / ui->label->height();

            // Calculate the scaled mouse position relative to the concatenated image label
            QPoint scaledMousePos((globalMousePos.x() - labelPos.x()) * scaleX,
                                  (globalMousePos.y() - labelPos.y()) * scaleY);

            // Check if the clicked point is within the bounds of the concatenated image
            if (scaledMousePos.x() >= 0 && scaledMousePos.y() >= 0 &&
                scaledMousePos.x() < combinedSize.width() && scaledMousePos.y() < combinedSize.height()) {

                // Print the position of the clicked point on the concatenated image
                qDebug() << "Clicked Position (Concatenated Image): " << scaledMousePos;

                // Add the clicked point to the list
                clickedPoints.append(scaledMousePos);

                // Draw the lines and circles
                drawLinesAndCircles();

            } else {
                qDebug() << "Clicked outside the bounds of the concatenated image";
            }
        }
    } catch (const std::exception &ex) {
        qDebug() << "Exception caught: " << ex.what();
        // You can add further actions in case of an exception, such as showing an error message
    }
}

void MainWindow::drawCircles(QPainter &painter) {
    painter.setPen(Qt::red);
    for (const QPoint &point : clickedPoints) {
        painter.drawEllipse(point, 5, 5);
    }
}

void MainWindow::drawLinesAndCircles() {
    // Create a copy of the concatenated image pixmap
    QPixmap combinedPixmap = ui->label->pixmap().copy();

    // Draw circles and lines
    QPainter painter(&combinedPixmap);
    painter.setPen(Qt::red);

    for (int i = 0; i < clickedPoints.size(); i += 2) {
        if (i + 1 < clickedPoints.size()) {
            QPoint startPoint = clickedPoints.at(i);
            QPoint endPoint = clickedPoints.at(i + 1);
            painter.drawEllipse(startPoint, 5, 5);
            painter.drawEllipse(endPoint, 5, 5);
            painter.drawLine(startPoint, endPoint);
        } else {
            QPoint point = clickedPoints.at(i);
            painter.drawEllipse(point, 5, 5);
        }
    }

    painter.end();

    // Save the points to the file
    saveLinesToFile("V", frameIndex1, clickedPoints);

    // Update the concatenated image label with the modified pixmap
    ui->label->setPixmap(combinedPixmap);
}

void MainWindow::saveLinesToFile(const QString& videoName, int frameNumber, const QList<QPoint>& points) {
    // Open the CSV file
    QFile file("points.csv");
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        qDebug() << "Error: Unable to open CSV file for writing";
        return;
    }

    // Create a QTextStream to write to the file
    QTextStream out(&file);

    // Write the header if the file is empty
    if (file.size() == 0) {
        out << "Video Name,Frame Number,Point 1 X,Point 1 Y,Point 2 X,Point 2 Y\n";
    }

    // Write each pair of points along with their frame number and video name to the CSV file
    for (int i = 0; i < points.size(); i += 2) {
        QPoint p1 = points.at(i);
        if (i + 1 < points.size()) {
            QPoint p2 = points.at(i + 1);
            out << videoName << "," << frameNumber << "," << p1.x() << "," << p1.y() << "," << p2.x() << "," << p2.y() << "\n";
        } else {
            out << videoName << "," << frameNumber << "," << p1.x() << "," << p1.y() << ",,\n";
        }
    }

    // Close the file
    file.close();

    qDebug() << "Points saved to CSV file successfully";
}

void MainWindow::removeLastPoint() {
    if (!clickedPoints.isEmpty()) {
        updateLabel();
        clickedPoints.removeLast();
        drawLinesAndCircles();
    } else {
        qDebug() << "No points to remove.";
    }
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (combinedPixmap.isNull()) {
        qDebug() << "Combined pixmap is null.";
        return;
    }

    switch (event->key()) {
    case Qt::Key_Q:
        centerY -= 10; // Move up by 10 pixels
        qDebug() << "Center Y after moving up: " << centerY;
        break;
    case Qt::Key_Y:
        centerY += 10; // Move down by 10 pixels
        qDebug() << "Center Y after moving down: " << centerY;
        break;

    default:
        QWidget::keyPressEvent(event);
        return;
    }

    // Ensure centerY stays within the image boundaries
    centerY = std::max(0, centerY);
    centerY = std::min(combinedPixmap.height() - 1, centerY); // Ensure it does not exceed the image height

    zoomInn();
}


#endif // EVENTS_H
