#ifndef REAL_H
#define REAL_H

#include "opencv2/core/mat.hpp"
#include <QDialog>
#include <QMainWindow>
#include <QLabel>
#include <QString>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace Ui {
class real;
}

class real : public QDialog
{
    Q_OBJECT

public:
    explicit real(const cv::Mat &image, QWidget *parent = nullptr);
    ~real();
    void updateRealImage(const cv::Mat &image);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::real *ui;
    QSize initialLabelSize; // Store the initial size of the QLabel
};

#endif // REAL_H
