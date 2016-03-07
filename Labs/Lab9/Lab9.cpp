/**
CS585_Lab9.cpp

CS585 Image and Video Computing Fall 2014
Lab 9
--------------
This program detects a face in a webcam videostream. You will be required to calculate
the total amount of optical flow in the face region (using both dense and sparse methods)
and save the number to a file (which will be read by the demo program)
--------------
*/

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/tracking.hpp"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>

using namespace std;
using namespace cv;

/** Function headers */

/**
Function to detect faces and display a bounding box
@param frame		frame in which to detect face

@return				Rect object denoting the bounding box containing the frame
*/
Rect detectAndDisplay(Mat frame);

/**
Function to draw dense optical flow vectors in the face as well as aggregate them into a single float
@param flow			2-channel matrix representing the optical flow
@param cflowmap		frame in which to draw the optical flow vectors
@param curr			the image ROI of the current frame containing the face
@param step			step or interval at which optical flow is sampled
@param color		color used to draw the optical flow vector

@return				float representing the aggregated optical flow in the bounding box containing the face
*/
float drawOptFlowMapDense(const Mat& flow, Mat& cflowmap, Rect curr, int step, const Scalar& color);

/**
Function to draw sparse optical flow vectors in the face as well as aggregate them into a single float
@param prevpoints	vector of points that was tracked from the previous frame
@param currpoints	vector of points on the current frame associated with the points of the previous frame
@param frame		frame in which to draw the optical flow vectors
@param curr			the image ROI of the current frame containing the face
@param color		color used to draw the optical flow vector
*/
float drawOptFlowMapSparse(vector<Point2f> prevpoints, vector<Point2f> currpoints, Mat& frame, Rect curr, const Scalar& color);

/** Global variables */
String face_cascade_name = "haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);
const int MAX_COUNT = 500;
vector<Point2f> points[2];
Size subPixWinSize(10, 10), winSize(31, 31);
TermCriteria termcrit(TermCriteria::COUNT | TermCriteria::EPS, 20, 0.03);

/** @function main */
int _tmain(int argc, const char** argv[])
{
	Mat frame, grayframe, prevgrayframe;
	Rect face_rect_curr, face_rect_prev;
	Mat curr, prev, uflow;

	//-- 1. Load the cascades
	if (!face_cascade.load(face_cascade_name)) { cout << "Error loading" << endl; return -1; }
	if (!eyes_cascade.load(eyes_cascade_name)) { cout << "Error loading" << endl; return -1; }

	//-- 2. Read the video stream
	VideoCapture capture("video1.avi");
	if (!capture.isOpened())
		return -1;

	int frame_count = 0;
	ofstream myfiledense, myfilesparse;

	//-- *These will be the files you write to
	myfiledense.open("OFdense.txt");
	myfilesparse.open("OFsparse.txt");
	for (;;)
	{
		capture >> frame;
		//-- 3. Apply the classifier to the frame
		if (!frame.empty())
		{
			face_rect_curr = detectAndDisplay(frame);
		}
		else
		{
			cout << "No captured frame -- Break! " << endl; break;
		}

		//-- 4. Draw rectangle to denote face
		rectangle(frame, face_rect_curr, Scalar(255, 0, 255), 3, 8, 0);
		cvtColor(frame, grayframe, COLOR_BGR2GRAY);

		//-- 5. Calculate dense or sparse optical flow
		//-- case opticalflow 0:dense, 1:sparse
		int opticalflow = 1;

		//-- starting from the second frame
		if (frame_count > 0)
		{
			curr = grayframe(face_rect_curr);
			prev = prevgrayframe(face_rect_curr);

			float totalofmag;
			switch (opticalflow)
			{
			case 0:
				//TODO:
				break;

			case 1:
				//TODO:
				break;
			}
		}
		frame_count++;

		imshow(window_name, frame);

		swap(prevgrayframe, grayframe);
		swap(points[1], points[0]);
		int c = waitKey(10);
		if ((char)c == 'c') { break; }
	}
	myfiledense.close();
	myfilesparse.close();
	return 0;
}

/** @function detectAndDisplay */
Rect detectAndDisplay(Mat frame)
{
	vector<Rect> faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));
	//cout << "Number of faces detected: " << faces.size() << endl;

	size_t big_rect_ind = 0;
	int max_area = 0;
	int area;
	for (size_t i = 0; i < faces.size(); i++)
	{
		area = faces[i].height * faces[i].width;
		if (area > max_area)
		{
			max_area = area;
			big_rect_ind = i;
		}
	}

	return faces[big_rect_ind];
}

/** @function drawOptFlowMapDense */
float drawOptFlowMapDense(const Mat& flow, Mat& cflowmap, Rect curr, int step, const Scalar& color)
{
	float totalmag = 0;
	//TODO:
	return totalmag;
}

/** @function drawOptFlowMapSparse */
float drawOptFlowMapSparse(vector<Point2f> prevpoints, vector<Point2f> currpoints, Mat& frame, Rect curr, const Scalar& color)
{
	float totalmag = 0;
	//TODO:
	return totalmag;
}
