/**
	CS585_Lab3.cpp
	@author: Ajjen Joshi
	@version: 1.0 9/17/2014

	CS585 Image and Video Computing Fall 2014
	Lab 3
	--------------
	This program introduces the following concepts:
		a) Finding objects in a binary image
		b) Filtering objects based on size
		c) Obtaining information about the objects described by their contours
	--------------
*/

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

//Global variables
int thresh = 71;
int max_thresh = 255;
Mat src; Mat src_gray;
RNG rng(12345);

// Function header
void threshold_callback(int, void*);

// main function
int main()
{
	// Load source image and convert it to gray
	src = imread("hand.jpg", 1);

	// Create Window and display source image
	namedWindow("Source", CV_WINDOW_AUTOSIZE );
	imshow( "Source", src );

	// Convert image to gray
	// Documentation for cvtColor: http://docs.opencv.org/modules/imgproc/doc/miscellaneous_transformations.html
	cvtColor( src, src_gray, CV_BGR2GRAY );
	// Blur the image
	// Documentation for blur: http://docs.opencv.org/modules/imgproc/doc/filtering.html?highlight=blur#blur
	blur( src_gray, src_gray, Size(3,3) );

	// Create Trackbar
	// Documentation for createTrackbar: http://docs.opencv.org/modules/highgui/doc/user_interface.html?highlight=createtrackbar#createtrackbar
	// Example of adding a trackbar: http://docs.opencv.org/doc/tutorials/highgui/trackbar/trackbar.html
	createTrackbar( "Threshold:", "Source", &thresh, max_thresh, threshold_callback);
	threshold_callback(0, 0);

	// Wait until keypress
	waitKey(0);
	return(0);
}

// function threshold_callback 
void threshold_callback(int, void*)
{
	Mat thres_output;
    vector<vector<Point> > contours, handContour;
	vector<Vec4i> hierarchy;
    Mat src_bw;

	cout << "Threshold: " << thresh << endl;

	// TODO
	// PseudoCode:
	//		1. Convert into binary image using thresholding
					// Documentation for threshold: http://docs.opencv.org/modules/imgproc/doc/miscellaneous_transformations.html?highlight=threshold#threshold
	//		2. Create Window and display thresholded image
	//		3. Find all contours in the thresholded image
					// Documentation for finding contours: http://docs.opencv.org/modules/imgproc/doc/structural_analysis_and_shape_descriptors.html?highlight=findcontours#findcontours
	//		4. Find the largest contour
					// Documentation on contourArea: http://docs.opencv.org/modules/imgproc/doc/structural_analysis_and_shape_descriptors.html
	//		5. Draw the object with the largest contour as well as its outline on a new image
					// Documentation on drawContours: http://docs.opencv.org/modules/imgproc/doc/structural_analysis_and_shape_descriptors.html?highlight=drawcontours#drawcontours
	//		6. Draw the bounding rectange of the largest object on the same output image
					// Documentation on finding a bounding box: http://docs.opencv.org/modules/imgproc/doc/structural_analysis_and_shape_descriptors.html
					// Documentation on drawing rectangles: http://docs.opencv.org/modules/core/doc/drawing_functions.html
	//		7. Print out an interesting property of the object

    //		1. Convert into binary image using thresholding
    double thr = threshold(src_gray, src_bw, thresh, max_thresh, THRESH_BINARY);
    //		2. Create Window and display thresholded image
    imshow("Thresholded image",src_bw);
    //		3. Find all contours in the thresholded image
    findContours(src_bw, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    //		4. Find the largest contour
    int idx=0;
    double max_area = 0; int max_contour_idx;
    for( ; idx < contours.size() ; idx++ )
    {
        double area = contourArea(contours[idx]);
        if (area > max_area)
        {
            max_area = area;
            max_contour_idx = idx;
        }
    }
    //handContour.push_back(contours[max_contour_idx]);
    //		5. Draw the object with the largest contour as well as its outline on a new image
    Scalar color = Scalar( 255,255,255 );
    drawContours(src, contours, max_contour_idx, color, 2, 8, hierarchy, 0, Point());
    imshow("Contour image",src);
}
