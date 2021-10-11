#ifndef THREADWORKER_H
#define THREADWORKER_H

#include <QObject>

#include <opencv2/core/core.hpp>

class ThreadWorker : public QObject
{
    Q_OBJECT
public:
    enum GradientDirection {
        Both,
        Vertical,
        Horizontal
    };
    explicit ThreadWorker(QObject *parent = nullptr);

    QImage process(QImage& input, int kernelSize, int gradOrder, cv::BorderTypes borderType, GradientDirection direction);

signals:
    void completed();


protected:
    cv::Mat imageToMat(QImage& image);
    QImage matToImage(const cv::Mat& mat);
};

#endif // THREADWORKER_H
