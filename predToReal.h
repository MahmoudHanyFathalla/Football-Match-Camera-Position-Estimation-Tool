#ifndef PREDTOREAL_H
#define PREDTOREAL_H

#include "opencv2/core/mat.hpp"
#include <QDialog>
#include <QMainWindow>
#include <QLabel>
#include <QString>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace Ui {
class predtoreal;
}

class predtoreal : public QDialog
{
    Q_OBJECT

public:
    explicit predtoreal(const cv::Mat &image,QWidget *parent = nullptr);
    ~predtoreal();
    void updatePredToRealImage(const cv::Mat &image);
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::predtoreal *ui;
    QSize initialLabelSize; // Store the initial size of the QLabel


};

#endif // PREDTOREAL_H
