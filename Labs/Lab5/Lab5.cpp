/**
CS585_Lab5.cpp

CS585 Image and Video Computing Fall 2014
Lab 5
--------------
This program introduces the following concepts:
a) Applying adaptive thresholding on images, in order to better segment images
b) Finding a better segmentation of images by first finding image edges
--------------
*/

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;



// Function headers
void threshold_callback1(int, void*);
void threshold_callback2(int, void*);

struct callback_data {
	Mat src_gray;
	int thresh;
	int max_thresh;
};

Mat src;


// main function
int main()
{

	callback_data cbd;
	cbd.thresh = 128;
	cbd.max_thresh = 255;


	// Load source image and convert it to gray
    src = imread("bats.ppm", 1);
    resize(src,src,Size(640,480));

	// Create Window and display source image
	namedWindow("Source", CV_WINDOW_AUTOSIZE);
	imshow("Source", src);

	// Convert image to gray
	// Documentation for cvtColor: http://docs.opencv.org/modules/imgproc/doc/miscellaneous_transformations.html
	cvtColor(src, cbd.src_gray, CV_BGR2GRAY);
	// Blur the image
	// Documentation for blur: http://docs.opencv.org/modules/imgproc/doc/filtering.html?highlight=blur#blur
	blur(cbd.src_gray, cbd.src_gray, Size(3, 3));

	// Create Trackbar
	// Documentation for createTrackbar: http://docs.opencv.org/modules/highgui/doc/user_interface.html?highlight=createtrackbar#createtrackbar
	// Example of adding a trackbar: http://docs.opencv.org/doc/tutorials/highgui/trackbar/trackbar.html

	// Method 1: Adaptive thresholding
	createTrackbar("Threshold:", "Source", &cbd.thresh, cbd.max_thresh, threshold_callback1,&cbd);
	threshold_callback1(0,&cbd);

	// Method 2: Edge thresholding
    //createTrackbar( "Threshold:", "Source", &thresh, max_thresh, threshold_callback2);
    //threshold_callback2(0, 0);

	// Wait until keypress
	waitKey(0);
	return(0);
}

// function threshold_callback 
void threshold_callback1(int, void* x)
{
	callback_data cbd = *(callback_data *)x;
	Mat thres_output;
	Mat a_thres_output;
    vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	// Convert into binary image using thresholding
	// Documentation for threshold: http://docs.opencv.org/modules/imgproc/doc/miscellaneous_transformations.html?highlight=threshold#threshold
	// Example of thresholding: http://docs.opencv.org/doc/tutorials/imgproc/threshold/threshold.html
	threshold(cbd.src_gray, thres_output, cbd.thresh, cbd.max_thresh, 0);
	// Create Window and display thresholded image
	namedWindow("Threshold", CV_WINDOW_AUTOSIZE);
    //imshow("Threshold", thres_output);

	//TODO
	//Adaptive thresholding documentation: http://docs.opencv.org/modules/imgproc/doc/miscellaneous_transformations.html#adaptivethreshold
    adaptiveThreshold(cbd.src_gray, a_thres_output,255,ADAPTIVE_THRESH_GAUSSIAN_C,CV_THRESH_BINARY,5,-2);
    imshow("Gaussian Adaptive Threshold", a_thres_output);

    adaptiveThreshold(cbd.src_gray, a_thres_output,255,ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,5,-2);
    imshow("Mean Adaptive Threshold", a_thres_output);
}

// function threshold_callback 
void threshold_callback2(int, void*)
{
	//TODO
	//Sobel edge detector tutorial: http://docs.opencv.org/doc/tutorials/imgproc/imgtrans/sobel_derivatives/sobel_derivatives.html
    Mat gray;
    cvtColor(src,gray,CV_BGR2GRAY);
    Mat sobel_x, sobel_y, abssobel_x, abssobel_y, grad;
    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;

    //Gradient X
    Sobel(gray,sobel_x,ddepth,1,0,3,scale,delta, BORDER_DEFAULT);
    convertScaleAbs(sobel_x,abssobel_x);

    //Gradient X
    Sobel(gray,sobel_y,ddepth,0,1,3,scale,delta, BORDER_DEFAULT);
    convertScaleAbs(sobel_y,abssobel_y);

    //Total gradient approximate
    addWeighted( abssobel_x, 0.5, abssobel_y, 0.5, 0, grad );

    imshow("Sobel Operator",grad);

}

