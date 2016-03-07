/**
CS585_Lab10.cpp

CS585 Image and Video Computing Fall 2014
Lab 10
--------------
This program computers SIFT features for the input images and draws them.
You will be required to write an algorithm that matches the keypoints for the two images.
Please refer to David Lowe's seminal paper on SIFT (Distinctive Image Features from Scale-Invariant keypoints)
--------------
*/

#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"


using namespace std;
using namespace cv;

/** @function main */
int main(int argc, char** argv)
{

	Mat img_1 = imread("image.jpg", 0);
	Mat img_2 = imread("image2.jpg", 0);

	if (!img_1.data || !img_2.data)
	{
		cout << " --(!) Error reading images " << endl; return -1;
	}

	vector<KeyPoint> keypoints_1, keypoints_2;
	Mat descriptors_1, descriptors_2;
	Mat mask_1, mask_2;

	//-- Step 1: Detect keypoints using <insert type of feature> Detector
    FASTX(img_1, keypoints_1, 100, true, FastFeatureDetector::TYPE_9_16);
    FASTX(img_2, keypoints_2, 100, true, FastFeatureDetector::TYPE_9_16);

    FREAK f(true, true, 22.0f, 4, vector<int>());
    f.compute(img_1, keypoints_1, descriptors_1);
    f.compute(img_2, keypoints_2, descriptors_2);

    //Ptr<BRISK> b = BRISK::create(100, 3, 1.0f);
    //b->detectAndCompute(img_1, mask_1, keypoints_1, descriptors_1, false);
    //b->detectAndCompute(img_2, mask_2, keypoints_2, descriptors_2, false);
    BRISK b(100, 3, 1.0f);
    //Number of rows in descriptors is equal to the number of points in keypoints
    b.detect(img_1, keypoints_1, mask_1);
    b.compute(img_1, keypoints_1, descriptors_1);
    b.detect(img_2, keypoints_2, mask_2);
    b.compute(img_2, keypoints_2, descriptors_2);

    /*ORB o(500, 1.2f, 8, 31, 0, 2, ORB::HARRIS_SCORE, 31);
    o.detect(img_1, keypoints_1, mask_1);
    o.compute(img_1, keypoints_1, descriptors_1);
    o.detect(img_2, keypoints_2, mask_2);
    o.compute(img_2, keypoints_2, descriptors_2);*/

    BFMatcher matcher(NORM_HAMMING);
    std::vector< DMatch > matches;
    matcher.match( descriptors_1, descriptors_2, matches );

    //-- Draw matches
    Mat img_matches;
    drawMatches( img_1, keypoints_1, img_2, keypoints_2, matches, img_matches );

    //-- Show detected matches
    imshow("Matches", img_matches );

	//-- Draw keypoints
	Mat img_keypoints_1; Mat img_keypoints_2;

	drawKeypoints(img_1, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	drawKeypoints(img_2, keypoints_2, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	//-- Show detected (drawn) keypoints
	namedWindow("Keypoints 1", WINDOW_NORMAL);
	namedWindow("Keypoints 2", WINDOW_NORMAL);

	imshow("Keypoints 1", img_keypoints_1);
	imshow("Keypoints 2", img_keypoints_2);



	

	//-- Step 2: Matching descriptor vectors 
	//TODO: Implement a method to match the <insert feature type> descriptors of the two images and visualize them by drawing lines between the two images
	//	Try to implement your own algorithm (or you can use openCV's BFMatcher class, along with the drawMatches function to complete this task)


	waitKey(0);

	return 0;
}

