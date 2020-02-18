#ifndef CONVERSION2QIMAGE_H
#define CONVERSION2QIMAGE_H

#include <QtGUI>
#include "urbi\uimage.hh"
#include "urbi\uconversion.hh"
#include "opencv/cv.hpp"

using namespace urbi;

inline UImage decompressUImage(UImage *rawUImg)
{
    UImage uncompressedUImg = UImage::make();
    // uncompressedUImg.imageFormat = IMAGE_JPEG;  
    // color clutter
    uncompressedUImg.imageFormat = IMAGE_RGB;
    uncompressedUImg.height = rawUImg->height;
    uncompressedUImg.width = rawUImg->width;
    convert(*rawUImg, uncompressedUImg);
    //qDebug() << "Decompress succeeded!" << uncompressedUImg.imageFormat 
    //    << uncompressedUImg.size << uncompressedUImg.width << uncompressedUImg.height;

    return uncompressedUImg;
}

/*!
 *
 */
inline IplImage* uncompressedUImage2IplImage(const UImage& uimg)
{
    int width = uimg.width;
    int height = uimg.height;

    // Creates a iplImage with 3 channels
    IplImage *img = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    char * imgBuffer = img->imageData;

    // Remove alpha channel
    int jump = 3;

    for (int y=0; y<height; y++)
    {
        char* a = (char*)uimg.data + y*jump*width;

        for (int x=0; x<jump*width; x+=jump)
        {
            //Swap from RGB to BGR
            imgBuffer[2] = a[x];
            imgBuffer[1] = a[x+1];
            imgBuffer[0] = a[x+2];
            imgBuffer+=3;
        }
    }

    return img;
}

inline IplImage* compressedUImage2IplImage(UImage *rawUImg)
{
    UImage uncompressedUImage = decompressUImage(rawUImg);

    IplImage* iplImage = uncompressedUImage2IplImage(uncompressedUImage);

    return iplImage;
}

// Nxt Tower Defense version
inline QImage Ipl2QImage(const IplImage *newImage)
{
    QImage qtemp;
    if (newImage && cvGetSize(newImage).width>0)
    {
        int x;
        int y;
        char* data = newImage->imageData;

        qtemp= QImage(newImage->width, newImage->height, QImage::Format_RGB32);
        for( y = 0; y < newImage->height; y++, data +=newImage->widthStep )
        {
            for( x = 0; x < newImage->width; x++)
            {
                uint *p = (uint*)qtemp.scanLine (y) + x;
                *p = qRgb(data[x * newImage->nChannels+2],
                    data[x * newImage->nChannels+1],data[x * newImage->nChannels]);
            }
        }
    }
    return qtemp;	
}

inline IplImage* QImage2Ipl(const QImage& qImage)
{
    int width = qImage.width();
    int height = qImage.height();

    // Creates a iplImage with 3 channels
    IplImage *img = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    char * imgBuffer = img->imageData;

    // Remove alpha channel
    int jump = (qImage.hasAlphaChannel()) ? 4 : 3;

    for (int y=0; y<height; y++)
    {
        char* a = (char*)qImage.scanLine(y);
        
        for (int x=0; x<jump*width; x+=jump)
        {
            //Swap from RGB to BGR
            imgBuffer[2] = a[x];
            imgBuffer[1] = a[x+1];
            imgBuffer[0] = a[x+2];
            imgBuffer+=3;
        }
    }

    return img;
}

/*
 * OFFICIAL VERSION
 * Converting iplImage to QImage
 * This snippet will convert a iplImage with depth 8 and 1 or 3 channels into a 8/24-bit QImage.
 * NOTE:
 *     This code won't work for images with different depth and number of channels.
 */
inline QImage IplImage2QImage(const IplImage *iplImage)
{
    int height = iplImage->height;
    int width = iplImage->width;

    if  (iplImage->depth == IPL_DEPTH_8U && iplImage->nChannels == 3)
    {
        const uchar *qImageBuffer = (const uchar*)iplImage->imageData;
        QImage img(qImageBuffer, width, height, QImage::Format_RGB888);
        return img.rgbSwapped();
    } 
    else if  (iplImage->depth == IPL_DEPTH_8U && iplImage->nChannels == 1)
    {
        const uchar *qImageBuffer = (const uchar*)iplImage->imageData;
        QImage img(qImageBuffer, width, height, QImage::Format_Indexed8);

        QVector<QRgb> colorTable;
        for (int i = 0; i < 256; i++)
        {
            colorTable.push_back(qRgb(i, i, i));
        }
        img.setColorTable(colorTable);
        return img;
    }
    else
    {
        qWarning() << "Image cannot be converted.";
        return QImage();
    }
}

/*
 * OFFICIAL VERSION
 * Converting QImage to iplImage
 * This snippet will convert QImages into iplImage with depth 8 and 3 channels. QImage can store data in
 *  several formats. This code accepts just 24-bit QImage::Format_RGB888 and the QImage::Format_RGB32.
 * Alpha values in the 32-bit format will be removed during the conversion.
 */
inline IplImage* qImage2IplImage(const QImage& qImage)
{
    int width = qImage.width();
    int height = qImage.height();

    // Creates a iplImage with 3 channels
    IplImage *img = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    char * imgBuffer = img->imageData;

    // Remove alpha channel
    int jump = (qImage.hasAlphaChannel()) ? 4 : 3;

    for (int y=0;y<img->height;y++)
    {
        QByteArray a((const char*)qImage.scanLine(y), qImage.bytesPerLine());
        for (int i=0; i<a.size(); i+=jump)
        {
            //Swap from RGB to BGR
            imgBuffer[2] = a[i];
            imgBuffer[1] = a[i+1];
            imgBuffer[0] = a[i+2];
            imgBuffer+=3;
        }
    }

    return img;
}


//static QImage IplImageToQImage(const IplImage  *iplImage, uchar **data , bool mirroRimage = true )
//{
//    uchar *qImageBuffer = NULL;
//    int    width        = iplImage->width;
//
//    /*
//    * Note here that OpenCV image is stored so that each lined is 32-bits aligned thus
//    * explaining the necessity to "skip" the few last bytes of each line of OpenCV image buffer.
//    */
//    int widthStep = iplImage->widthStep;
//    int height    = iplImage->height;
//
//    switch (iplImage->depth)
//    {
//    case IPL_DEPTH_8U:
//        if (iplImage->nChannels == 1)
//        {
//            /* OpenCV image is stored with one byte grey pixel. We convert it to an 8 bit depth QImage. */
//            qImageBuffer = (uchar *)malloc(width * height * sizeof(uchar));
//            uchar *QImagePtr = qImageBuffer;
//            const uchar *iplImagePtr = (const uchar *)iplImage->imageData;
//            for (int y = 0; y < height; ++y)
//            {
//                // Copy line by line
//                memcpy(QImagePtr, iplImagePtr, width);
//                QImagePtr += width;
//                iplImagePtr += widthStep;
//            }
//        }
//        else if (iplImage->nChannels == 3)
//        {
//            /* OpenCV image is stored with 3 byte color pixels (3 channels). We convert it to a 32 bit depth QImage. */
//            qImageBuffer = (uchar *)malloc(width * height * 4 * sizeof(uchar));
//            uchar *QImagePtr = qImageBuffer;
//            const uchar *iplImagePtr = (const uchar *)iplImage->imageData;
//
//            for (int y = 0; y < height; ++y)
//            {
//                for (int x = 0; x < width; ++x)
//                {
//                    // We cannot help but copy manually.
//                    QImagePtr[0] = iplImagePtr[0];
//                    QImagePtr[1] = iplImagePtr[1];
//                    QImagePtr[2] = iplImagePtr[2];
//                    QImagePtr[3] = 0;
//
//                    QImagePtr += 4;
//                    iplImagePtr += 3;
//                }
//                iplImagePtr += widthStep - 3 * width;
//            }
//        }
//        else
//            qDebug("IplImageToQImage: image format is not supported : depth=8U and %d channels\n", iplImage->nChannels);
//
//        break;
//
//    case IPL_DEPTH_16U:
//        if (iplImage->nChannels == 1)
//        {
//            /* OpenCV image is stored with 2 bytes grey pixel. We convert it to an 8 bit depth QImage. */
//            qImageBuffer = (uchar *)malloc(width * height * sizeof(uchar));
//            uchar *QImagePtr = qImageBuffer;
//            const uint16_t *iplImagePtr = (const uint16_t *)iplImage->imageData;
//
//            for (int y = 0; y < height; ++y)
//            {
//                for (int x = 0; x < width; ++x)
//                    *QImagePtr++ = ((*iplImagePtr++) >> 8); // We take only the highest part of the 16 bit value. It is similar to dividing by 256.
//                iplImagePtr += widthStep / sizeof(uint16_t) - width;
//            }
//        }
//        else
//            qDebug("IplImageToQImage: image format is not supported : depth=16U and %d channels\n", iplImage->nChannels);
//
//        break;
//
//    case IPL_DEPTH_32F:
//        if (iplImage->nChannels == 1)
//        {
//            /* OpenCV image is stored with float (4 bytes) grey pixel. We convert it to an 8 bit depth QImage. */
//            qImageBuffer = (uchar *)malloc(width * height * sizeof(uchar));
//            uchar *QImagePtr = qImageBuffer;
//            const float *iplImagePtr = (const float *)iplImage->imageData;
//
//            for (int y = 0; y < height; ++y)
//            {
//                for (int x = 0; x < width; ++x)
//                    *QImagePtr++ = (uchar)(255 * ((*iplImagePtr++)));
//                iplImagePtr += widthStep / sizeof(float) - width;
//            }
//        }
//        else
//            qDebug("IplImageToQImage: image format is not supported : depth=32F and %d channels\n", iplImage->nChannels);
//
//        break;
//
//    case IPL_DEPTH_64F:
//        if (iplImage->nChannels == 1)
//        {
//            /* OpenCV image is stored with double (8 bytes) grey pixel. We convert it to an 8 bit depth QImage. */
//            qImageBuffer = (uchar *) malloc(width * height * sizeof(uchar));
//            uchar *QImagePtr = qImageBuffer;
//            const double *iplImagePtr = (const double *) iplImage->imageData;
//
//            for (int y = 0; y < height; ++y)
//            {
//                for (int x = 0; x < width; ++x)
//                    *QImagePtr++ = (uchar)(255 * ((*iplImagePtr++)));
//                iplImagePtr += widthStep / sizeof(double) - width;
//            }
//        }
//        else
//            qDebug("IplImageToQImage: image format is not supported : depth=64F and %d channels\n", iplImage->nChannels);
//
//        break;
//
//    default:
//        qDebug("IplImageToQImage: image format is not supported : depth=%d and %d channels\n", iplImage->depth, iplImage->nChannels);
//    }
//
//    QImage *qImage;
//    if (iplImage->nChannels == 1)
//    {
//        QVector<QRgb> colorTable;
//        for (int i = 0; i < 256; i++)
//            colorTable.push_back(qRgb(i, i, i));
//
//        qImage = new QImage(qImageBuffer, width, height, QImage::Format_Indexed8);
//        qImage->setColorTable(colorTable);
//    }
//    else
//        qImage = new QImage(qImageBuffer, width, height, QImage::Format_RGB32);
//    QImage gd0 = qImage->mirrored(false,mirroRimage);
//    *data = qImageBuffer;
//    QColor textColor = Qt::black;
//    QColor fillrectcolor = Qt::red;
//    QColor shapepicture = Qt::white;
//    QImage gd = gd0.scaledToWidth(350);
//    QDateTime now = QDateTime::currentDateTime ();
//    QString selectionText = now.toString("dd.MM.yyyy hh:mm:ss");
//    QPainter p(&gd);
//    p.setRenderHint(QPainter::Antialiasing, true);
//
//    QFontMetrics fm( qApp->font() );
//    int stringWidth = fm.width(selectionText);
//    int stringHeight = fm.ascent();
//    const int sx = gd.width() - stringWidth - 5;
//    QPen pen;
//    pen.setStyle( Qt::SolidLine );
//    pen.setWidth( 2 );
//    pen.setColor( textColor );
//    p.setPen( pen);
//    p.drawText(QPointF(sx - 1 ,gd.height() - 2 - stringHeight - 1),selectionText);
//    pen.setColor( fillrectcolor );
//    p.setPen( pen);
//    p.drawText(QPointF(sx,gd.height() - 2 - stringHeight),selectionText);
//
//    return gd;
//}


#endif // CONVERSION2QIMAGE_H