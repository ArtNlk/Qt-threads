#ifndef THREADWORKER_H
#define THREADWORKER_H

#include <QObject>

#include <opencv2/core/core.hpp>

class ThreadWorker : public QObject
{
    Q_OBJECT
public:
    enum GradientDirection {
        None = 0,
        Vertical = 1,
        Horizontal = 2,
        Both = 3
    };
    explicit ThreadWorker(QObject *parent = nullptr);

public slots:
    void process(QImage input, int kernelSize, int borderType, int direction, bool normalize);

signals:
    void completed(QImage result);

protected:
    cv::Mat imageToMat(const QImage& image);
    QImage matToImage(const cv::Mat& mat);
};

#endif // THREADWORKER_H
