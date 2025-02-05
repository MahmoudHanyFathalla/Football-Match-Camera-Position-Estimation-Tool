#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
#include "edit.h"
#include "ui_mainwindow.h"
#include "ImageState.h"
#include "systemState.h"
#include <QStack>
#include <QPair>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    ImageState imageState;
    SystemState systemState;

    void Hsplitter2ControlLabelAndLabel2(int pos, int index);
    void HsplitterControlLabel3AndLabel4(int pos, int index);
    void Hsplitter5ControlSplitter3AndSplitter4(int pos, int index);
    void Vsplitter3ControlSplitterAndSplitter2(int pos, int index);
    void Vsplitter4ControlWidget6AndWidget7(int pos, int index);
    void Vsplitter6ControlSplitter5AndWidget4(int pos, int index);
    void Vsplitter7ControlSplitter6AndWidget8(int pos, int index);
    void syncSplittersWithSplitter5(int pos, int index);
    void getAllSplittersWidthAndHeight();
    void adjustLabelsBySplitter5(int handlePosition);
    void adjustLabelsBySplitter6(int handlePosition);
    void adjustLabelsBySplitter7(int handlePosition);
    void updateHandlersOfSplitter34(int pos, int index);
    void getAllSplittersWidthAndHeight(ImageState &imageState);
    void adjustSplitterWidths(int pos, int index);
    void moveSplitter5HandleBy5Pixels();
    void setComponentsBasedOnRatios(ImageState &imageState);
    void readConfigFile(const QString &filePath);
    void connectActions();
    void setStyleSheet();
    void buildUI();
    void statusBarBuild();

    void removeLabel();
    void removeLabel2();
    void removeLabel3();
    void removeLabel4();
    void resetLabels();
    void removeWiget8();
    void resetWiget8();
    void removeSplitter4();
    void resetSplitter4();
    void resetUI();
    void saveDesign();
    void labelsSize();
    void adjustLabelSizes(QLabel *label);
    void initializeMenuBar();
    void openShortCutsDialog();
    void openEditDialog();
    void exitApplication();

    //draw.h
    void drawPerspctive(ImageState& imageState);
    void drawImageWithPoints(ImageState& imageState);
    void drawImageWithPoints2(ImageState& imageState);
    void loadAndDisplayImage(int position,ImageState& imageState,SystemState& systemState);

    //horizontalSlider.h
    void onHorizontalSliderPressed();
    void onHorizontalSliderReleased();
    void onHorizontalSliderMoved(int position);
    void updateSliderSize(ImageState& imageState);

    //events.h
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event) override;


    QPixmap convertMatToPixmap(const cv::Mat &mat);
    void drawLinesAndCircles();
    void drawCircles(QPainter &painter);

    //gui.h
    void setFocusPolicies();
    void loadFirstImage(ImageState& imageState);
    void setInitialLabelsValues(ImageState& imageState);

    QMap<QString, QList<int>> initialSizes;
    QMap<QString, QPoint> initialPositions;
    void removeLastPoint() ;

    QPixmap pixmap1, pixmap2;
    QList<QPoint> clickedPoints;

    // Declare variables to store video capture objects and frame indices
    cv::VideoCapture videoCapture1;
    cv::VideoCapture videoCapture2;
    int frameIndex1 = 0;
    int frameIndex2 = 0;

    QString videoFilePath1; // Member variable to store the file path of the first video
    QString videoFilePath2;
    void saveLinesToFile(const QString& videoName, int frameNumber, const QList<QPoint>& points);



    void updateLabel();
    QPixmap getFirstFrame(const QString &filePath);

     QPixmap combinedPixmap; // Combined pixmap
    void zoomInn();
     float  zoomFactor =1.2;
    void moveZoomVertically(int deltaY);
    int  deltaY =0;

    int centerY = 500; // Y coordinate of the center
     void wheelEvent(QWheelEvent *event);


     void zooom(bool zoomIn,float zoomFactor);
     void updateZoomedImage();
     QStack<QPair<QPoint, QPoint>> drawnLines;
     void redraw();
     void removeLastLine();
     QPixmap originalPixmap;





protected:

public slots:
    //draw.h
    void on_openFolder_clicked(); // open folder
    void on_reset_clicked(); // reset

    //txtOperations.h
    void on_save_clicked(); //save and write txt in output
    void on_load_clicked();

    //moveButtons.h
    void on_RectTopSideToUp_clicked();  //moving

    void on_RectTopSideToDown_clicked();

    void on_RectBottomSideToUp_clicked();

    void on_RectBottomSideToDown_clicked();

    void on_RectLeftSideToRight_clicked();

    void on_RectLeftSideToLeft_clicked();

    void on_RectRightSideToLeft_clicked();

    void on_RectRightSideToRight_clicked();

    void on_AllRectToUp_clicked();

    void on_AllRectToDown_clicked();

    void on_AllRectToRight_clicked();

    void on_AllRectToLeft_clicked();

    void on_TrapTopSideToUp_clicked();

    void on_TrapTopSideToDown_clicked();

    void on_TrapBottomSideToUp_clicked();

    void on_TrapBottomSideToDown_clicked();

    //rotation
    void on_VerticalRotationToLeft_clicked();

    void on_VerticalRotationToRight_clicked();

    void on_HorizontalRotationToLeft_clicked();

    void on_HorizontalRotationToRight_clicked();

    //ratio.h
    void on_increaseVRotationAngle_clicked();

    void on_decreaseVRotationAngle_clicked();

    void on_increaseMovingRatio_clicked();

    void on_decreaseMovingRatio_clicked();

    void on_increaseZoomRatio_clicked();

    void on_decreaseZoomRatio_clicked();

    void on_increaseHRotationAngle_clicked();

    void on_decreaseHRotationAngle_clicked();


    //horizontalSlider.h
    void on_nextImage_clicked();

    void on_previousImage_clicked();

    void on_next10Image_clicked();

    void on_previous10Image_clicked();

    //viewWindows.h
    void on_pushButton_34_clicked();

    void on_pushButton_35_clicked();

    void on_pushButton_36_clicked();

    void on_pushButton_37_clicked();








private slots:



    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

private:
    Ui::MainWindow *ui;
    edit *editWindowPointer;


};

#endif // MAINWINDOW_H
