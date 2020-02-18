/*!********************************************************
 *  Some little functions to use.
 *  --- Junchao Xu, MMI, EWI, TUDelft / 2011.03.31
 **********************************************************/
#pragma once
#ifndef MY_CV_FUNCTIONS_H
#define MY_CV_FUNCTIONS_H

#include "opencv/cv.hpp"
#include "opencv2/core/core.hpp"
#include <QString>
#include <QDebug>

inline void printMatDouble(QString name, cv::Mat M)
{
    qDebug() << name << ":";

    for (int i=0;i<M.rows;i++)
    {
        QString row = 0;
        for (int j=0;j<M.cols;j++)
        {
            row += QString::number( M.at<double>(i,j) ) + ' ';
        }
        qDebug() << row;
    }
    qDebug() << "\n";
}

// Print out a CvMat
inline void printCvMat(QString name, CvMat *cmat)
{
    qDebug() << name << ":";

    for (int i=0;i<cmat->rows;i++)
    {
        QString row = 0;
        for (int j=0;j<cmat->cols;j++)
        {
            row += QString::number( CV_MAT_ELEM( *cmat, float, i, j) ) + ' ';
        }
        qDebug() << row;
    }
    qDebug() << "\n";
}

// Print out a CvMat
inline void printCvMatDouble(QString name, CvMat *cmat)
{
    qDebug() << name << ":";

    for (int i=0;i<cmat->rows;i++)
    {
        QString row = 0;
        for (int j=0;j<cmat->cols;j++)
        {
            row += QString::number( CV_MAT_ELEM( *cmat, double, i, j) ) + ' ';
        }
        qDebug() << row;
    }
    qDebug() << "\n";
}

// Show text and float value at a certain position of the image
inline void showText(IplImage *frame, char *name, float val, int x, int y)
{  
    // show a float on image
    char cd[100];
    strcpy(cd, name);
    char sv[100];
    _gcvt(val, 10, sv);
    strcat(cd, sv);
    cvPutText( frame, cd, cvPoint(x, y), &cvFont(1), CV_RGB( 255, 0, 0 ));
}

inline void drawAxes(IplImage *img)
{
    CvFont cf;
    cvInitFont( &cf,CV_FONT_HERSHEY_PLAIN, 1, 1, 0, 1, CV_AA );
    CvPoint org = cvPoint(20,20);
    CvPoint org_x = cvPoint(80,20);
    CvPoint org_y = cvPoint(20,80);
    cvCircle ( img, org,   1, CV_RGB( 0, 0, 255 ) );
    cvCircle ( img, org_x, 1, CV_RGB( 0, 0, 255 ) );
    cvCircle ( img, org_y, 1, CV_RGB( 0, 0, 255 ) );
    cvPutText( img, "x", org_x, &cf, CV_RGB( 0, 0, 255 ) );
    cvPutText( img, "O", org,   &cf, CV_RGB( 0, 0, 255 ) );
    cvPutText( img, "y", org_y, &cf, CV_RGB( 0, 0, 255 ) );
    cvLine   ( img, org, org_x, CV_RGB( 0, 0, 255 ), 1, 8, 0 );
    cvLine   ( img, org, org_y, CV_RGB( 0, 0, 255 ), 1, 8, 0 );
    return;
}

inline void drawAxesMat( cv::Mat img )
{
    CvPoint org = cvPoint(20,20);
    CvPoint org_x = cvPoint(80,20);
    CvPoint org_y = cvPoint(20,80);
    cv::circle ( img, org,   1, CV_RGB( 0, 0, 255 ) );
    cv::circle ( img, org_x, 1, CV_RGB( 0, 0, 255 ) );
    cv::circle ( img, org_y, 1, CV_RGB( 0, 0, 255 ) );
    cv::putText( img, "x", org_x, cv::FONT_HERSHEY_SIMPLEX, 1, CV_RGB( 0, 0, 255 ) );
    cv::putText( img, "O", org,   cv::FONT_HERSHEY_SIMPLEX, 1, CV_RGB( 0, 0, 255 ) );
    cv::putText( img, "y", org_y, cv::FONT_HERSHEY_SIMPLEX, 1, CV_RGB( 0, 0, 255 ) );
    cv::line   ( img, org, org_x, CV_RGB( 0, 0, 255 ), 1, 8, 0 );
    cv::line   ( img, org, org_y, CV_RGB( 0, 0, 255 ), 1, 8, 0 );
    return;
}

#endif