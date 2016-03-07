/**
CS585_Lab8.cpp

derived from OpenCV sample code : /opencv/samples/cpp/fback.cpp

CS585 Image and Video Computing Fall 2014
Lab 8
--------------
This program introduces the following concepts:
a) Applying dense optical flow using Gunnar Farneback's algorithm
b) Visualizing the optical flow
--------------
*/

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;

/**
Function that prints out what the program does
*/
static void help();

/**
Function that visualizes the optical flow, as lines drawn on subsampled pixels
@param flow 2	channel matrix of the same size of the input image, contains the flow vector for each pixel
@param cflowmap image on which you will draw the sub-sampled flow vectors
@param step		stepsize to subsample the original image
@param color	color with which the flow vectors are drawn
*/
static void drawOptFlowMap(const Mat& flow, Mat& cflowmap, int step, const Scalar& color);

int main()
{
	//Read video
	VideoCapture cap(0);
	help();
	if (!cap.isOpened())
		return -1;

	//Initialize matrix objects
	Mat flow, cflow, frame;
	Mat gray, prevgray, uflow;

	//Create window to display output
	namedWindow("Optical Flow", 1);

	for (;;)
	{
		//Read frame into Mat object
		cap >> frame;
		//convert to grayscale
		cvtColor(frame, gray, COLOR_BGR2GRAY);

		if (!prevgray.empty())
		{
			//call Farneback's optical flow algorithm
			//Documentation: http://docs.opencv.org/modules/video/doc/motion_analysis_and_object_tracking.html#calcopticalflowfarneback
			//Link to Farneback's paper: http://www.diva-portal.org/smash/get/diva2:273847/FULLTEXT01.pdf
            calcOpticalFlowFarneback(prevgray, gray, uflow, 0.5, 3, 15, 3, 5, 1.2, OPTFLOW_USE_INITIAL_FLOW);

			//cflow is the Mat where you will draw your optical flow vectors
			cvtColor(prevgray, cflow, COLOR_GRAY2BGR);
            drawOptFlowMap(uflow, cflow, 4, Scalar(0, 255, 0));
            imshow("Optical Flow", cflow);
            //imshow("Optical Flow", uflow);
		}
		if (waitKey(30) >= 0)
			break;
		swap(prevgray, gray);
	}
	return 0;
}

static void help()
{
	cout <<
		"\nThis program demonstrates the dense optical flow algorithm by Gunnar Farneback\n"
		"This reads from video camera 0\n" << endl;
}
static void drawOptFlowMap(const Mat& flow, Mat& cflowmap, int step, const Scalar& color)
{
	//TODO:
    for(int y = 0; y < cflowmap.rows; y += step)
        for(int x = 0; x < cflowmap.cols; x += step)
        {
            Point2f flow_xy = flow.at<Point2f>(y, x);
            line(cflowmap, Point(x,y), Point(cvRound(x+flow_xy.x), cvRound(y+flow_xy.y)),
                 color);
            circle(cflowmap, Point(x,y), 2, color, -1);
        }
}
