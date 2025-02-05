#ifndef TXTOPERATIONS_H
#define TXTOPERATIONS_H

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
#include "calcPointsPositions.h"
#include "ImageState.h"
#include "qregularexpression.h"
#include "systemState.h"

void readPointValues(QTextStream &stream, const QRegularExpression &pattern, double &x, double &y)
{
    QString line = stream.readLine();
    QRegularExpressionMatch match = pattern.match(line);
    if (match.hasMatch()) {
        x = match.captured(1).toDouble();
        y = match.captured(2).toDouble();

    } else {
        qDebug() << "Failed to match values in line: " << line;
    }
}

void loadPointsFromTextFile(const QString &folderPath, ImageState& imageState,SystemState& systemState)
{
    if(systemState.flags.load == true){
        systemState.stringData.solutions = "output";
    }
    else{

        systemState.stringData.solutions = systemState.stringData.tempForLoad;
    }

    // Assuming imageName is the name of the currently selected image
    QDir parentDir(folderPath);
    parentDir.cdUp();
    QString textFilePath = parentDir.absolutePath() + QDir::separator() + systemState.stringData.solutions + QDir::separator() + imageState.images.imageName + ".txt";


    QFile textFile(textFilePath);
    if (textFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&textFile);

        // Define a regular expression pattern to match integer values in the format (x, y)
        //QRegularExpression integerPattern("\\((-?\\d+), (-?\\d+)\\)");
        QRegularExpression integerPattern("\\((-?\\d+\\.?\\d*), (-?\\d+\\.?\\d*)\\)");



        double pointTrap1X; // point1x
        double pointTrap1Y; // point1y
        double pointTrap2X; // point2x
        double pointTrap2Y; // point2y
        double pointTrap3X; // point3x
        double pointTrap3Y; // point3y
        double pointTrap4X; // point4x
        double pointTrap4Y; // point4y

        double pointRect1X; // point1_2x
        double pointRect1Y; // point1_2y
        double pointRect2X; // point2_2x
        double pointRect2Y; // point2_2y
        double pointRect3X; // point3_2x
        double pointRect3Y; // point3_2y
        double pointRect4X; // point4_2x
        double pointRect4Y; // point4_2y

        // Read and set values for Trapezoidal points
        stream.readLine(); // Skip "Trapezoidal points:"
        readPointValues(stream, integerPattern, pointTrap1X, pointTrap1Y);
        readPointValues(stream, integerPattern, pointTrap2X, pointTrap2Y);
        readPointValues(stream, integerPattern, pointTrap3X, pointTrap3Y);
        readPointValues(stream, integerPattern, pointTrap4X, pointTrap4Y);

        // Read and set values for Rectangle points
        stream.readLine(); // Skip "Rectangle points:"
        readPointValues(stream, integerPattern, pointRect1X, pointRect1Y);
        readPointValues(stream, integerPattern, pointRect2X, pointRect2Y);
        readPointValues(stream, integerPattern, pointRect3X, pointRect3Y);
        readPointValues(stream, integerPattern, pointRect4X, pointRect4Y);

        stream.readLine(); // Skip "Rotation:"
        QString rotationLine = stream.readLine().trimmed();



     //   imageState.ratios.score = stream.readLine().trimmed().toDouble();
        imageState.score = 0;

        imageState.VerticalAngleT = rotationLine.toDouble();
        systemState.ratios.accumulatedRotationAnglecopy = imageState.VerticalAngleT;


         stream.readLine(); // Skip "Rotation rect:"
        if(systemState.flags.load==true){

            QString rotationRect = stream.readLine().trimmed();
            imageState.HorizontalAngleR =  rotationRect.toDouble();
             stream.readLine(); // Skip "img score:"
            imageState.imgscore = stream.readLine().trimmed().toDouble();

             if (imageState.imgscore < 85) {
                 systemState.labels.inscore->setStyleSheet("background-color: red;");
             } else if (imageState.imgscore < 90 && imageState.imgscore >= 85) {
                 systemState.labels.inscore->setStyleSheet("background-color: orange;");
             } else if (imageState.imgscore >= 90) {
                 systemState.labels.inscore->setStyleSheet("background-color: green;");
             }

             systemState.labels.inscore->setText(QString("%1").arg(imageState.imgscore));

              stream.readLine(); // Skip "img score:"
               imageState.score = stream.readLine().trimmed().toDouble();


        }else{
             imageState.HorizontalAngleR = 0.0;
        }
      //  imageState.ratios.rectangle = 0.0;



        systemState.labels.imgscorelable->setText(QString(" %1").arg(imageState.score));
        if (imageState.score < 85) {
           systemState.labels.imgscorelable->setStyleSheet("background-color: red;");
        } else if (imageState.score < 90 && imageState.score >= 85) {
            systemState.labels.imgscorelable->setStyleSheet("background-color: orange;");
        } else if (imageState.score >= 90) {
            systemState.labels.imgscorelable->setStyleSheet("background-color: green;");
        }

        // Original sizes
        const int originalTrapWidth = 1280;
        const int originalTrapHeight = 720;
        const int originalRectWidth = 1024;
        const int originalRectHeight = 682;


        imageState.points.TrapTopLeft = cv::Point2f(pointTrap1X, pointTrap1Y); // point1Trap
        imageState.points.TrapTopRight = cv::Point2f(pointTrap2X, pointTrap2Y); // point2Trap
        imageState.points.TrapBottomRight = cv::Point2f(pointTrap3X, pointTrap3Y); // point3Trap
        imageState.points.TrapBottomLeft = cv::Point2f(pointTrap4X, pointTrap4Y); // point4Trap

        // Initialize the positions of the points for label_2
        imageState.points.RectTopLeft = cv::Point2f(pointRect1X, pointRect1Y); // point1Rect
        imageState.points.RectTopRight = cv::Point2f(pointRect2X, pointRect2Y); // point2Rect
        imageState.points.RectBottomRight = cv::Point2f(pointRect3X, pointRect3Y); // point3Rect
        imageState.points.RectBottomLeft = cv::Point2f(pointRect4X, pointRect4Y); // point4Rect

       // addPointsToHash(imageState.points, 2, imageState.points.hash);
       // std::cout << "Hash: " << imageState.points.hash.toStdString() << std::endl;

        // Close the file
        textFile.close();
    } else {
        qDebug() << "Failed to open the text file: " << textFilePath;
        imageState.VerticalAngleT = 0.0;
        imageState.HorizontalAngleR = 0.0;

        initializePoints(imageState);



    }
}

void saveImageData(ImageState& imageState,SystemState& systemState) {
    if (imageState.images.imageName.isEmpty()) {
        qDebug() << "No image selected.";
        return;
    }

    // Get the parent directory of the "lolo" folder
    QDir parentDir(systemState.stringData.selectedFolderPath);
    parentDir.cdUp(); // Move up one level

    // Construct the output file path
    QString outputFolderPath = parentDir.absolutePath() + QDir::separator() + "output";
    QDir outputFolder(outputFolderPath);
    if (!outputFolder.exists()) {
        outputFolder.mkpath(".");
    }

    QString outputFilePath = outputFolderPath + QDir::separator() + imageState.images.imageName + ".txt";

    QFile outputFile(outputFilePath);
    if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // Write points positions to the file
        QTextStream stream(&outputFile);
        stream << "Trapezoidal points[real]: \n";
        stream << "Point 1: (" << imageState.points.TrapTopLeft.x << ", " << imageState.points.TrapTopLeft.y << ")\n"; // point1Trap
        stream << "Point 2: (" << imageState.points.TrapTopRight.x << ", " << imageState.points.TrapTopRight.y << ")\n"; // point2Trap
        stream << "Point 3: (" << imageState.points.TrapBottomRight.x << ", " << imageState.points.TrapBottomRight.y << ")\n"; // point3Trap
        stream << "Point 4: (" << imageState.points.TrapBottomLeft.x << ", " << imageState.points.TrapBottomLeft.y << ")\n"; // point4Trap

        stream << "Rectangle points[play]: \n";
        stream << "Point 1_2: (" << imageState.points.RectTopLeft.x << ", " << imageState.points.RectTopLeft.y << ")\n"; // point1Rect
        stream << "Point 2_2: (" << imageState.points.RectTopRight.x << ", " << imageState.points.RectTopRight.y << ")\n"; // point2Rect
        stream << "Point 3_2: (" << imageState.points.RectBottomRight.x << ", " << imageState.points.RectBottomRight.y << ")\n"; // point3Rect
        stream << "Point 4_2: (" << imageState.points.RectBottomLeft.x << ", " << imageState.points.RectBottomLeft.y << ")\n"; // point4Rect

        // ... add more points if needed

        stream << "Rotation Angle: \n";
        stream << imageState.VerticalAngleT << "\n";

        stream << "Rect Rotation Angle: \n";
        stream << imageState.HorizontalAngleR << "\n";

        stream << "Image score was: \n";
        stream << imageState.imgscore << "\n";

        stream << "His score is: \n";
        stream << imageState.score << "\n";

        // Further processing code

        outputFile.close();
        qDebug() << "Saved successfully";
    } else {
        qDebug() << "Failed to create or open the output file: " << outputFilePath;
    }
}

void saveRatiosToConfig(const ImageState &imageState, const QString &filePath,SystemState& systemState) {
    // Open the file for reading and writing
    QFile file(filePath);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug() << "Failed to open config file for reading and writing.";
        return;
    }

    // Read the existing content of the file
    QTextStream in(&file);
    QString content;
    while (!in.atEnd()) {
        content += in.readLine() + "\n";
    }

    // Modify the lines related to the ratios
    QMap<QString, QString> ratios;
    ratios["labelWidthRatio"] = QString::number(systemState.uiDimensions.labelWidthRatio);
    ratios["label2WidthRatio"] = QString::number(systemState.uiDimensions.label2WidthRatio);
    ratios["label3WidthRatio"] = QString::number(systemState.uiDimensions.label3WidthRatio);
    ratios["label4WidthRatio"] = QString::number(systemState.uiDimensions.label4WidthRatio);
    ratios["widget4HeightRatio"] = QString::number(systemState.uiDimensions.widget4HeightRatio);
    ratios["widget6HeightRatio"] = QString::number(systemState.uiDimensions.widget6HeightRatio);
    ratios["widget7HeightRatio"] = QString::number(systemState.uiDimensions.widget7HeightRatio);
    ratios["widget8HeightRatio"] = QString::number(systemState.uiDimensions.widget8HeightRatio);
    ratios["splitter6HeightRatio"] = QString::number(systemState.uiDimensions.splitter6HeightRatio);
    ratios["splitter5HeightRatio"] = QString::number(systemState.uiDimensions.splitter5HeightRatio);
    ratios["splitterHeightRatio"] = QString::number(systemState.uiDimensions.splitterHeightRatio);
    ratios["splitter2HeightRatio"] = QString::number(systemState.uiDimensions.splitter2HeightRatio);
    ratios["splitter4WidthRatio"] = QString::number(systemState.uiDimensions.splitter4WidthRatio);
    ratios["splitter3WidthRatio"] = QString::number(systemState.uiDimensions.splitter3WidthRatio);

    QStringList lines = content.split("\n", Qt::SkipEmptyParts);
    for (auto it = ratios.constBegin(); it != ratios.constEnd(); ++it) {
        QRegularExpression regex("^" + it.key() + "=");
        for (int i = 0; i < lines.size(); ++i) {
            if (regex.match(lines[i]).hasMatch()) {
                lines[i] = it.key() + "=" + it.value();
                break;
            }
        }
    }

    // Write the modified content back to the file
    file.resize(0); // Clear the file content
    QTextStream out(&file);
    for (const QString &line : lines) {
        out << line << "\n";
    }

    // Close the file
    file.close();

    qDebug() << "Ratios saved to config file.";
}

#endif // TXTOPERATIONS_H

/*
std::string floatToString(float value, int precision) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    return stream.str();
}
void addPointsToHash(const Points& points, int precision, QString& hash) {
    // Convert each coordinate to string and append to hash
    hash += QString::fromStdString(floatToString(points.TrapTopLeft.x, precision)) + ",";
    hash += QString::fromStdString(floatToString(points.TrapTopLeft.y, precision)) + ",";

    hash += QString::fromStdString(floatToString(points.TrapTopRight.x, precision)) + ",";
    hash += QString::fromStdString(floatToString(points.TrapTopRight.y, precision)) + ",";

    hash += QString::fromStdString(floatToString(points.TrapBottomRight.x, precision)) + ",";
    hash += QString::fromStdString(floatToString(points.TrapBottomRight.y, precision)) + ",";

    hash += QString::fromStdString(floatToString(points.TrapBottomLeft.x, precision)) + ",";
    hash += QString::fromStdString(floatToString(points.TrapBottomLeft.y, precision)) + ",";

    hash += QString::fromStdString(floatToString(points.centerTrap.x, precision)) + ",";
    hash += QString::fromStdString(floatToString(points.centerTrap.y, precision)) + ",";

    hash += QString::fromStdString(floatToString(points.RectTopLeft.x, precision)) + ",";
    hash += QString::fromStdString(floatToString(points.RectTopLeft.y, precision)) + ",";

    hash += QString::fromStdString(floatToString(points.RectTopRight.x, precision)) + ",";
    hash += QString::fromStdString(floatToString(points.RectTopRight.y, precision)) + ",";

    hash += QString::fromStdString(floatToString(points.RectBottomRight.x, precision)) + ",";
    hash += QString::fromStdString(floatToString(points.RectBottomRight.y, precision)) + ",";

    hash += QString::fromStdString(floatToString(points.RectBottomLeft.x, precision)) + ",";
    hash += QString::fromStdString(floatToString(points.RectBottomLeft.y, precision)) + ",";

    hash += QString::fromStdString(floatToString(points.centerRect.x, precision)) + ",";
    hash += QString::fromStdString(floatToString(points.centerRect.y, precision)) + ",";


   // hash += points.hash; // Append the existing hash string
}
*/
