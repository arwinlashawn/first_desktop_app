#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>


using namespace cv;
//using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void imhist(Mat image, int histogram[])
{

    // initialize all intensity values to 0
    for(int i = 0; i < 256; i++)
    {
        histogram[i] = 0;
    }

    // calculate the no of pixels for each intensity values
    for(int y = 0; y < image.rows; y++)
        for(int x = 0; x < image.cols; x++)
            histogram[(int)image.at<uchar>(y,x)]++;

}

void cumhist(int histogram[], int cumhistogram[])
{
    cumhistogram[0] = histogram[0];

    for(int i = 1; i < 256; i++)
    {
        cumhistogram[i] = histogram[i] + cumhistogram[i-1];
    }
}

void histDisplay(int histogram[], const char* name)
{
    int hist[256];
    for(int i = 0; i < 256; i++)
    {
        hist[i]=histogram[i];
    }
    // draw the histograms
    int hist_w = 512; int hist_h = 400;
    int bin_w = cvRound((double) hist_w/256);

    Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(255, 255, 255));

    // find the maximum intensity element from histogram
    int max = hist[0];
    for(int i = 1; i < 256; i++){
        if(max < hist[i]){
            max = hist[i];
        }
    }

    // normalize the histogram between 0 and histImage.rows

    for(int i = 0; i < 256; i++){
        hist[i] = ((double)hist[i]/max)*histImage.rows;
    }


    // draw the intensity line for histogram
    for(int i = 0; i < 256; i++)
    {
        line(histImage, Point(bin_w*(i), hist_h),
                              Point(bin_w*(i), hist_h - hist[i]),
             Scalar(0,0,0), 1, 8, 0);
    }

    // display histogram
    namedWindow(name, WINDOW_AUTOSIZE);
    imshow(name, histImage);
}

// first way to convert cvmat to qimage, result in color though
QImage Mat2QImage(cv::Mat const& src)
{
     cv::Mat temp; // make the same cv::Mat
     cvtColor(src, temp, COLOR_RGB2BGR); // cvtColor Makes a copy, that what i need
     QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     dest.bits(); // enforce deep copy, see documentation
     // of QImage::QImage ( const uchar * data, int width, int height, Format format )
     return dest;
}



void MainWindow::on_btn_image_clicked()
{

//    QString filename = QFileDialog::getOpenFileName(this, tr("Choose"), "", tr("Images(*.png *.jpg *.jpeg *.bmp *.gif)"));

//    if (QString::compare(filename, QString()) != 0)
//    {
//        QImage image;
//        bool valid = image.load(filename);

//        if (valid)
//        {
//            image = image.scaledToWidth(ui->lbl_image->width(), Qt::SmoothTransformation);
//            ui->lbl_image->setPixmap(QPixmap::fromImage(image));


//        }
//        else
//        {
//            //error handling
//        }
//    }
    // Load the image
    QString filename = QFileDialog::getOpenFileName(this, tr("Choose"), "", tr("Images(*.png *.jpg *.jpeg *.bmp *.gif)"));

    // Convert QString to std::string for imread()
    std::string filepath = filename.toLocal8Bit().constData();

    // Save to Mat image
    Mat image = imread(filepath, IMREAD_GRAYSCALE);


    // back to QImage to view in Qt
    //QImage qimage = Mat2QImage(image);

    // qimage is now colored, convert back to grayscale
    //qimage = qimage.convertToFormat(QImage::Format_Grayscale16);

        if (QString::compare(filename, QString()) != 0)
            {
                QImage image2 = Mat2QImage(image);
                //QImage image2 = QImage((uchar*) image.data, image.cols, image.rows, image.step, QImage::Format_Grayscale8);
                //image2 = image.convertToFormat(QImage::Format_Grayscale16);
                image2 = image2.scaledToWidth(ui->lbl_image->width(), Qt::SmoothTransformation);
                ui->lbl_image->setPixmap(QPixmap::fromImage(image2));
            }


    // Generate the histogram
    int histogram[256];
    imhist(image, histogram);

    // Caluculate the size of image
    int size = image.rows * image.cols;
    float alpha = 255.0/size;

    // Calculate the probability of each intensity
    float PrRk[256];
    for(int i = 0; i < 256; i++)
    {
        PrRk[i] = (double)histogram[i] / size;
    }

    // Generate cumulative frequency histogram
    int cumhistogram[256];
    cumhist(histogram,cumhistogram );

    // Scale the histogram
    int Sk[256];
    for(int i = 0; i < 256; i++)
    {
        Sk[i] = cvRound((double)cumhistogram[i] * alpha);
    }


    // Generate the equlized histogram
    float PsSk[256];
    for(int i = 0; i < 256; i++)
    {
        PsSk[i] = 0;
    }

    for(int i = 0; i < 256; i++)
    {
        PsSk[Sk[i]] += PrRk[i];
    }

    int final[256];
    for(int i = 0; i < 256; i++)
        final[i] = cvRound(PsSk[i]*255);


    // Generate the equlized image
    Mat new_image = image.clone();

    for(int y = 0; y < image.rows; y++)
        for(int x = 0; x < image.cols; x++)
            new_image.at<uchar>(y,x) = saturate_cast<uchar>(Sk[image.at<uchar>(y,x)]);

//   // Display the original Image, not needed
//    namedWindow("Original Image");
//    imshow("Original Image", image);

    // Display the original Histogram
    histDisplay(histogram, "Original Histogram");

    // Display equalized image
    namedWindow("Equalized Image");
    imshow("Equalized Image",new_image);

    // Display the equalized histogram
    histDisplay(final, "Equalized Histogram");

    waitKey();
    // return 1;
}


// how to access variables from another function?
//void MainWindow::on_process_image_clicked()
//{
//    namedWindow("Original Histogram");
//    imshow("Equalized Image", new_image);

//    waitKey();
//    histo = new Histo(this);
//    histo -> show();
//}
