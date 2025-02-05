#ifndef EDIT_H
#define EDIT_H


#include "systemState.h"
#include <QDialog>

namespace Ui {
class edit;
}

class edit : public QDialog
{
    Q_OBJECT

public:
    explicit edit(SystemState &stateRef,QWidget *parent = nullptr);

    ~edit();

signals:
    void SVerticalRotationToLeft();
    void SVerticalRotationToRight();
    void SHorizontalRotationToLeft();
    void SHorizontalRotationToRight();
    void SincreaseVRotationAngle();
    void SdecreaseVRotationAngle();
    void SincreaseMovingRatio();
    void SdecreaseMovingRatio();
    void SincreaseZoomRatio();
    void SdecreaseZoomRatio();
    void SincreaseHRotationAngle();
    void SdecreaseHRotationAngle();

private slots:
    void on_VerticalRotationToLeft_clicked();

    void on_VerticalRotationToRight_clicked();

    void on_HorizontalRotationToLeft_clicked();

    void on_HorizontalRotationToRight_clicked();

    void on_increaseVRotationAngle_clicked();

    void on_decreaseVRotationAngle_clicked();

    void on_increaseMovingRatio_clicked();

    void on_decreaseMovingRatio_clicked();

    void on_increaseZoomRatio_clicked();

    void on_decreaseZoomRatio_clicked();

    void on_increaseHRotationAngle_clicked();

    void on_decreaseHRotationAngle_clicked();

private:
    Ui::edit *ui;
    SystemState &systemStateRef;
};

#endif // EDIT_H
