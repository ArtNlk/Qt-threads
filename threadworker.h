#ifndef THREADWORKER_H
#define THREADWORKER_H

#include <QObject>

#include <opencv2/core/core.hpp>

class ThreadWorker : public QObject
{
    Q_OBJECT
public:
    explicit ThreadWorker(QObject *parent = nullptr);

public slots:
    void process(const QImage& input, QImage& output, int dx, int dy, int kernelSize, cv::BorderTypes borderType);

signals:
    void completed();


protected:
    cv::Mat imageToMat(QImage& image);
    QImage matToImage(const cv::Mat& mat);
};

#endif // THREADWORKER_H
