#ifndef SYSTEMSTATE_H
#define SYSTEMSTATE_H

#include <opencv2/core.hpp>
#include <QString>
#include <QStringList>
#include <QPointer>
#include "real.h"
#include "realToPred.h"
#include "predToReal.h"
#include "playGround.h"
#include "qlabel.h"
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

struct StringData {

    QString selectedFolderPath;
    QString selectedFolderPath2;
    QStringList imageFileList;
    QSize labelSize1;
    QSize labelSize ;
    QSize label2Size ;
    QString solutions;
    QString tempForLoad;
};

struct UIDimensions {
    int splitterWidth;
    int splitterHeight;
    int splitter2Width;
    int splitter2Height;
    int splitter3Width;
    int splitter3Height;
    int splitter4Width;
    int splitter4Height;
    int splitter5Width;
    int splitter5Height;
    int splitter6Width;
    int splitter6Height;
    int splitter7Width;
    int splitter7Height;
    int labelWidth;
    int label2Width;
    int label3Width;
    int label4Width;
    int widget4Height;
    int widget6Height;
    int widget7Height;
    int widget8Height;
    double labelWidthRatio;
    double label2WidthRatio;
    double label3WidthRatio;
    double label4WidthRatio;
    double widget4HeightRatio;
    double widget6HeightRatio;
    double widget7HeightRatio;
    double widget8HeightRatio;
    double splitter6HeightRatio;
    double splitter5HeightRatio;
    double splitterHeightRatio;
    double splitter2HeightRatio;
    double splitter4WidthRatio;
    double splitter3WidthRatio;

    // remove the rest
    QPoint initialMousePos;
    QPoint initialMousePos1;
};

struct Flags {
    bool sliderMovingAllowed;
    bool isMouseHeldOnPoint = false;
    bool VerticalRotationToLeft = false; // modifyPointsOnButtonClick
    bool VerticalRotationToRight = false; // Click
    bool saved = false; // flag
    bool HorizontalRotationToRight = false; // Cl
    bool HorizontalRotationToLeft = false; // rl

    bool zoomed = false; // z
    bool lock = false;
    bool load = false;
    bool nightMoodFlag= false;
    bool Edited = false;


};

struct Ratios {

    int previousSliderPosition;

    double accumulatedRotationAnglecopy = 0.0;
    double rectanglecopy = 0.0;
    double movingratio = 2;
    double rotatingratio = 0.2;
    double zoomratio = 2.0;

    double rectratio = 0.1;

};

struct Labels {
    QLabel* movingRatioLabel;
    QLabel* inscore;
    QLabel* rotatingRatioLabel;
    QLabel* HrotatingRatioLabel;
    QLabel* zoomingRatioLabel;
    QLabel* imgscorelable;
};

struct Pointers {
    QPointer<real> RealW;
    QPointer<realtopred> RealToPredW;
    QPointer<predtoreal> PredToRealW;
    QPointer<playground> PlayGroundW;
};
struct SystemState {
    StringData stringData;
    UIDimensions uiDimensions;
    Flags flags;
    Ratios ratios;
    Labels labels;
    Pointers pointers;


};
#endif // SYSTEMSTATE_H
