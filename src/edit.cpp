#include "edit.h"
#include "ui_edit.h"


edit::edit(SystemState &stateRef, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::edit),systemStateRef(stateRef)
{
    ui->setupUi(this);
    ui->label_20->setText(QString("%1").arg(systemStateRef.ratios.rotatingratio));
    ui->label_19->setText(QString("%1").arg(systemStateRef.ratios.movingratio));
    ui->label_8->setText(QString("%1").arg(systemStateRef.ratios.rectratio));
    ui->label_28->setText(QString("%1").arg(systemStateRef.ratios.zoomratio));
}

edit::~edit()
{
    delete ui;
}

void edit::on_VerticalRotationToLeft_clicked()
{

    emit SVerticalRotationToLeft();
}


void edit::on_VerticalRotationToRight_clicked()
{
    emit SVerticalRotationToRight();

}


void edit::on_HorizontalRotationToLeft_clicked()
{
    emit SHorizontalRotationToLeft();
}


void edit::on_HorizontalRotationToRight_clicked()
{
    emit SHorizontalRotationToRight();
}


void edit::on_increaseVRotationAngle_clicked()
{
    emit SincreaseVRotationAngle();
    ui->label_20->setText(QString("%1").arg(systemStateRef.ratios.rotatingratio));

}


void edit::on_decreaseVRotationAngle_clicked()
{
    emit SdecreaseVRotationAngle();
    ui->label_20->setText(QString("%1").arg(systemStateRef.ratios.rotatingratio));
}


void edit::on_increaseMovingRatio_clicked()
{
    emit SincreaseMovingRatio();
    ui->label_19->setText(QString("%1").arg(systemStateRef.ratios.movingratio));
}


void edit::on_decreaseMovingRatio_clicked()
{
   emit SdecreaseMovingRatio();
   ui->label_19->setText(QString("%1").arg(systemStateRef.ratios.movingratio));
}


void edit::on_increaseZoomRatio_clicked()
{
    emit SincreaseZoomRatio();
    ui->label_28->setText(QString("%1").arg(systemStateRef.ratios.zoomratio));
}


void edit::on_decreaseZoomRatio_clicked()
{
   emit SdecreaseZoomRatio();
   ui->label_28->setText(QString("%1").arg(systemStateRef.ratios.zoomratio));
}


void edit::on_increaseHRotationAngle_clicked()
{
   emit SincreaseHRotationAngle();
   ui->label_8->setText(QString("%1").arg(systemStateRef.ratios.rectratio));
}


void edit::on_decreaseHRotationAngle_clicked()
{
   emit SdecreaseHRotationAngle();
   ui->label_8->setText(QString("%1").arg(systemStateRef.ratios.rectratio));
}

