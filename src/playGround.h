#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#include "opencv2/core/mat.hpp"
#include <QDialog>
#include <QMainWindow>
#include <QLabel>
#include <QString>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace Ui {
class playground;
}

class playground : public QDialog
{
    Q_OBJECT

public:
    explicit playground(const cv::Mat &image,QWidget *parent = nullptr);
    ~playground();
    void updatePlayGroundImage(const cv::Mat &image);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::playground *ui;
     QSize initialLabelSize; // Store the initial size of the QLabel
};

#endif // PLAYGROUND_H
