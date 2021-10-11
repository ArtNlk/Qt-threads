#include "threadworker.h"

#include <QImage>

#include <opencv2/imgproc.hpp>

ThreadWorker::ThreadWorker(QObject *parent) : QObject(parent)
{

}

QImage ThreadWorker::process(QImage &input, int kernelSize, int gradOrder, cv::BorderTypes borderType, GradientDirection direction)
{
    cv::Mat mat = imageToMat(input);
    cv::Mat vMat;
    cv::Mat hMat;
    switch(direction)
    {
        case Both:
            cv::Sobel(mat, vMat, CV_8S, gradOrder, 0, kernelSize, 1, 0, borderType);
            cv::Sobel(mat, hMat, CV_8S, 0, gradOrder, kernelSize, 1, 0, borderType);
            cv::magnitude(vMat,hMat,mat);

            emit completed();
            return matToImage(mat);
        break;

        case Vertical:
            cv::Sobel(mat, vMat, CV_8S, gradOrder, 0, kernelSize, 1, 0, borderType);
            cv::magnitude(vMat,hMat,mat);

            emit completed();
            return matToImage(hMat);
        break;

        case Horizontal:
            cv::Sobel(mat, vMat, CV_8S, gradOrder, 0, kernelSize, 1, 0, borderType);

            emit completed();
            return matToImage(vMat);
        break;

        default:
            throw std::invalid_argument("ThreadWorker::process Invalid gradient dorection");
        break;
    }
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
