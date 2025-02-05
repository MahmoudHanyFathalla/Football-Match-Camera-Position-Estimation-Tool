#ifndef REALTOPRED_H
#define REALTOPRED_H

#include "opencv2/core/mat.hpp"
#include <QDialog>
#include <QMainWindow>
#include <QLabel>
#include <QString>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
namespace Ui {
class realtopred;
}

class realtopred : public QDialog
{
    Q_OBJECT

public:
    explicit realtopred(const cv::Mat &image,QWidget *parent = nullptr);
    ~realtopred();
    void updateRealToPredImage(const cv::Mat &image);
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::realtopred *ui;
    QSize initialLabelSize; // Store the initial size of the QLabel
};

#endif // REALTOPRED_H
