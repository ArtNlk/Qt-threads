#include "threadworker.h"

#include <QImage>
#include <QDebug>
#include <QThread>

#include <opencv2/imgproc.hpp>

ThreadWorker::ThreadWorker(QObject *parent) : QObject(parent)
{

}

void ThreadWorker::process(QImage input, int kernelSize, int borderType, int direction, bool normalize)
{
    qDebug() << "Worker processing thread id: " << QThread::currentThreadId();
    cv::Mat mat = imageToMat(input);
    cv::Mat vMat;
    cv::Mat hMat;
    //QThread::sleep(5);
    qDebug() << "kSize = " << kernelSize <<
                "borderType = " << borderType <<
                "direction = " << direction;
    switch(direction)
    {
        case Both:
            cv::Sobel(mat, vMat, CV_32F, 1, 0, kernelSize, 1, 0, borderType);
            cv::Sobel(mat, hMat, CV_32F, 0, 1, kernelSize, 1, 0, borderType);
            cv::magnitude(vMat,hMat,mat);
            if(normalize)   cv::normalize(mat,mat,0,255,cv::NORM_MINMAX);

            emit completed(matToImage(mat));
            return;
        break;

        case Vertical:
            cv::Sobel(mat, vMat, CV_32F, 1, 0, kernelSize, 1, 0, borderType);
            if(normalize)   cv::normalize(vMat,vMat,0,255,cv::NORM_MINMAX);

            emit completed(matToImage(vMat));
            return;
        break;

        case Horizontal:
            cv::Sobel(mat, hMat, CV_32F, 0, 1, kernelSize, 1, 0, borderType);
            if(normalize)   cv::normalize(hMat,hMat,0,255,cv::NORM_MINMAX);

            emit completed(matToImage(hMat));
            return;
        break;

        default:
            throw std::invalid_argument("ThreadWorker::process Invalid gradient dorection");
        break;
    }
}

cv::Mat ThreadWorker::imageToMat(const QImage& image)
{
//    cv::Mat mat;
//    QImage convImage;
//    if(image->format() != QImage::Format_RGB888)
//    {
//        convImage = image->convertToFormat(QImage::Format_RGB888);
//        mat = cv::Mat(convImage.height(),
//                      convImage.width(),
//                      CV_8UC3,
//                      convImage.bits(),
//                      convImage.bytesPerLine());
//    } else {
//        mat = cv::Mat(image->height(),
//                      image->width(),
//                      CV_8UC3,
//                      image->bits(),
//                      image->bytesPerLine());
//    }

//    mat.convertTo(mat,CV_32F);

//    return mat;

    switch ( image.format() )
          {
             // 8-bit, 4 channel
             case QImage::Format_ARGB32:
             case QImage::Format_ARGB32_Premultiplied:
             {
                cv::Mat  mat( image.height(), image.width(),
                              CV_8UC4,
                              const_cast<uchar*>(image.bits()),
                              static_cast<size_t>(image.bytesPerLine())
                              );

                return mat.clone();
             }

             // 8-bit, 3 channel
             case QImage::Format_RGB32:
             {

                cv::Mat  mat( image.height(), image.width(),
                              CV_8UC4,
                              const_cast<uchar*>(image.bits()),
                              static_cast<size_t>(image.bytesPerLine())
                              );

                cv::Mat  matNoAlpha;

                cv::cvtColor( mat, matNoAlpha, cv::COLOR_BGRA2BGR );   // drop the all-white alpha channel

                return matNoAlpha;
             }

             // 8-bit, 3 channel
             case QImage::Format_RGB888:
             {

                QImage swapped = image.rgbSwapped();

                return cv::Mat( swapped.height(), swapped.width(),
                                CV_8UC3,
                                const_cast<uchar*>(swapped.bits()),
                                static_cast<size_t>(swapped.bytesPerLine())
                                ).clone();
             }

             // 8-bit, 1 channel
             case QImage::Format_Indexed8:
             {
                cv::Mat  mat( image.height(), image.width(),
                              CV_8UC1,
                              const_cast<uchar*>(image.bits()),
                              static_cast<size_t>(image.bytesPerLine())
                              );

                return mat.clone();
             }

             default:
                qWarning() << "ASM::QImageToCvMat() - QImage format not handled in switch:" << image.format();
                break;
          }

          return cv::Mat();
}

QImage ThreadWorker::matToImage(const cv::Mat& mat)
{
//    QImage output;

//    if(mat->depth() != CV_8U)
//    {
//        cv::Mat convMat = cv::Mat();
//        convMat.convertTo(convMat,CV_8U);
//        output = QImage( (const unsigned char *)(convMat.data),
//                                    convMat.cols,
//                                    convMat.rows,
//                                    QImage::Format_RGB888 );
//    } else {
//        output = QImage( (const unsigned char *)(mat->data),
//                                    mat->cols,
//                                    mat->rows,
//                                    QImage::Format_RGB888 );
//    }

//    return output;
    cv::Mat convMat;
    switch ( mat.type() )
          {
             // 8-bit, 4 channel
             case CV_8UC4:
             {
                QImage image( mat.data,
                              mat.cols, mat.rows,
                              static_cast<int>(mat.step),
                              QImage::Format_ARGB32 );

                return image;
             }

            case CV_32FC3:
            {
                qDebug() << "Float 32 3";
                mat.convertTo(convMat,CV_8U);
                QImage image( convMat.data,
                             convMat.cols, convMat.rows,
                             static_cast<int>(convMat.step),
                             QImage::Format_RGB888 );

                return image.rgbSwapped();
            }

            case CV_32FC4:
            {
                qDebug() << "Float 32 4";
                mat.convertTo(convMat,CV_8U);
                cv::cvtColor(convMat, convMat, cv::COLOR_BGRA2BGR);
                QImage image( convMat.data,
                             convMat.cols, convMat.rows,
                             static_cast<int>(convMat.step),
                             QImage::Format_RGB888 );

                return image.rgbSwapped();
            }

             // 8-bit, 3 channel
             case CV_8UC3:
             {
                QImage image( mat.data,
                              mat.cols, mat.rows,
                              static_cast<int>(mat.step),
                              QImage::Format_RGB888 );

                return image.rgbSwapped();
             }

             // 8-bit, 1 channel
             case CV_8UC1:
             {
    #if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
                QImage image( mat.data,
                              mat.cols, mat.rows,
                              static_cast<int>(mat.step),
                              QImage::Format_Grayscale8 );
    #else
                static QVector<QRgb>  sColorTable;

                // only create our color table the first time
                if ( sColorTable.isEmpty() )
                {
                   sColorTable.resize( 256 );

                   for ( int i = 0; i < 256; ++i )
                   {
                      sColorTable[i] = qRgb( i, i, i );
                   }
                }

                QImage image( mat.data,
                              mat.cols, inMat.rows,
                              static_cast<int>(mat.step),
                              QImage::Format_Indexed8 );

                image.setColorTable( sColorTable );
    #endif

                return image;
             }

             default:
                qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << mat.type();
                break;
          }

          return QImage();
}
