//opencv libraries
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//C++ standard libraries
#include <iostream>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;

//Global variables
int thresh = 55;
int max_thresh = 255;
RNG rng(12345);

vector<Mat> spl;

// Function header
void threshold_callback(int, void*);

int main(int argc, char** argv)
{
    //int hmin=20, hmax=150, smin=77, smax=153, vmin=36, vmax=211;
    int hmin=57, hmax=217, smin=140, smax=219, vmin=175, vmax=255;
    namedWindow("Skin filter values");
    createTrackbar("Hue low", "Skin filter values", &hmin, 180);
    createTrackbar("Hue high", "Skin filter values", &hmax, 255);
    createTrackbar("Saturation low", "Skin filter values", &smin, 255);
    createTrackbar("Saturation high", "Skin filter values", &smax, 255);
    createTrackbar("Value low", "Skin filter values", &vmin, 255);
    createTrackbar("Value high", "Skin filter values", &vmax, 255);
    Mat output;
    Mat image, hsv;
    image = imread(argv[1],1);
    resize(image,image,Size(800,600));
    cvtColor(image,hsv, CV_BGR2HSV);
    imshow("input", image);

    split(image,spl);
    imshow("Hue",spl[0]);//h
    imshow("Saturation",spl[1]);//s
    imshow("Value",spl[2]);//v

    createTrackbar( "Threshold:", "Skin filter values", &thresh, max_thresh, threshold_callback);
    threshold_callback(0, 0);

    //VideoCapture cap(argv[1]);
    //VideoCapture cap(0);
    while(1)
    {
        //cap.read(image);
        //imshow("input", image);
        //cvtColor(image,hsv, CV_RGB2HSV);
        inRange(hsv, Scalar(hmin,smin,vmin), Scalar(hmax,smax,vmax), output);
        //erode(output,output,Mat());
        //dilate(output,output,Mat());

        imshow("Result Window", output);
        //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        if (waitKey(30) == 27)
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }
    }
    //cap.release();
    image.release();
    return 0;
}

// function threshold_callback
void threshold_callback(int, void*)
{
    Mat thres_output;
    vector<vector<Point> > contours, handContour;
    vector<Vec4i> hierarchy;
    Mat src_bw;

    thres_output = Mat::zeros(spl[2].rows, spl[2].cols, CV_8UC1);

    cout << "Threshold: " << thresh << endl;

    //		1. Convert into binary image using thresholding
    double thr = threshold(spl[2], thres_output, thresh, max_thresh, THRESH_BINARY);
    //erode(thres_output,thres_output,Mat());
    //dilate(thres_output,thres_output,Mat());
    //		2. Create Window and display thresholded image
    imshow("Thresholded image",thres_output);
}
