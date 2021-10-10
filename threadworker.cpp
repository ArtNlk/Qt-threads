#include "threadworker.h"

#include <QImage>

#include <opencv2/imgproc.hpp>

ThreadWorker::ThreadWorker(QObject *parent) : QObject(parent)
{

}

cv::Mat ThreadWorker::imageToMat(QImage &image)
{
    cv::Mat mat;
    if(image.format() != QImage::Format_RGB888)
    {
        QImage convImage = image.convertToFormat(QImage::Format_RGB888);
        mat = cv::Mat(convImage.height(),
                      convImage.width(),
                      CV_8UC3,
                      convImage.bits(),
                      convImage.bytesPerLine());
    } else {
        mat = cv::Mat(image.height(),
                      image.width(),
                      CV_8UC3,
                      image.bits(),
                      image.bytesPerLine());
    }

    mat.convertTo(mat,CV_8S);

    return mat;
}

QImage ThreadWorker::matToImage(const cv::Mat &mat)
{
    QImage output;

    if(mat.depth() != CV_8U)
    {
        cv::Mat convMat = cv::Mat();
        convMat.convertTo(convMat,CV_8U);
        output = QImage( (const unsigned char *)(convMat.data),
                                    convMat.cols,
                                    convMat.rows,
                                    QImage::Format_RGB888 );
    } else {
        output = QImage( (const unsigned char *)(mat.data),
                                    mat.cols,
                                    mat.rows,
                                    QImage::Format_RGB888 );
    }

    return output;
}
