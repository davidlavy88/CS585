/**
CS585_Lab11.cpp

CS585 Image and Video Computing Fall 2014
Lab 11
--------------
This program combines multiple images to form a panorama/mosaic image.
You will be required to write an algorithm that matches the keypoints for two images, find a homography between them,
and combine them into a single image
--------------
*/
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/stitching/stitcher.hpp"
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

/**
Function that detects keypoints using the SIFT Detector and extracts descriptors for the keypoints
@param image the source image from which to extract features
@param keypoints vector of keypoints that will be calculated
@param descriptors Mat of descriptors each row corresponding to vector description of a keypoint
*/
void ExtractSIFTDescriptors(const Mat image, vector<KeyPoint> &keypoints, Mat &descriptors);

/**
Function that finds good matches between the keypoints of two images
@param descriptors1 Mat of keypoint descriptors for 1st image
@param descriptors2 Mat of keypoint descriptors for 2nd image
@param keypoints1	vector of keypoints for 1st image
@param keypoints2	vector of keypoints for 2nd image
@param goodmatches1 vector of keypoints for 1st image that matches nicely with keypoints for 2nd image
@param goodmatches1 vector of keypoints for 2nd image that matches nicely with keypoints for 3rd image
*/
void FindGoodMatches(Mat &descriptors1, Mat &descriptosr2, vector<KeyPoint> &keypoints1, vector<KeyPoint> &keypoints2, vector<Point2f> &goodmatches1, vector<Point2f> &goodmatches2);

/**
Function that calculates the homography between the two images and returns a combined mosaic
@param image1 the first image
@param image2 the second iamge
@param goodpoints1 the set of points to match from the first image
@param goodpoints2 the set of points to match in the second image
@return result the combined image
*/
Mat combineTwoImages(Mat &image1, Mat &image2, vector<Point2f> &goodmatches1, vector<Point2f> &goodmatches2);

/** @function main */
int main(int argc, char** argv)
{
	// Load the images
	Mat image1 = imread("image1.jpg");
	Mat image2 = imread("image2.jpg");
	Mat image3 = imread("image3.jpg");

	//Display the images
	imshow("first image", image1);
	imshow("second image", image2);
	imshow("third image", image3);

	if (!image1.data || !image2.data || !image3.data)
	{
		std::cout << " --(!) Error reading images " << std::endl; return -1;
	}

	//For the first two images
	vector<KeyPoint> keypoints1, keypoints2;
	Mat descriptors1, descriptors2;
	ExtractSIFTDescriptors(image1, keypoints1, descriptors1);
	ExtractSIFTDescriptors(image2, keypoints2, descriptors2);

    if ( descriptors1.empty() )
       cvError(0,"MatchFinder","1st descriptor empty",__FILE__,__LINE__);
    if ( descriptors2.empty() )
       cvError(0,"MatchFinder","2nd descriptor empty",__FILE__,__LINE__);

	vector<Point2f> goodmatches1, goodmatches2;
	FindGoodMatches(descriptors1, descriptors2, keypoints1, keypoints2, goodmatches1, goodmatches2);

	Mat result;
	//TODO:
    result = combineTwoImages(image1, image2, goodmatches1, goodmatches2);
    imshow( "Result", result );

    vector<Mat> imgs;
    imgs.push_back(image1);
    imgs.push_back(image2);
    imgs.push_back(image3);

    Mat pano;
    Stitcher stitcher = Stitcher::createDefault(false);
    stitcher.stitch(imgs, pano);

    imshow("Stitched", pano);

	waitKey(0);
	return 0;
}

void ExtractSIFTDescriptors(const Mat image, vector<KeyPoint> &keypoints, Mat &descriptors)
{
	//-- Detect the keypoints using SIFT Detector

	//Documentation for SiftFeatureDetector: http://docs.opencv.org/2.3/modules/features2d/doc/common_interfaces_of_feature_detectors.html#siftfeaturedetector
    //SiftFeatureDetector detector(0.05, 5.0); //Contrast threshold and Edge threshold

	//Documentation for SiftDescriptorExtractor: http://docs.opencv.org/2.3/modules/features2d/doc/common_interfaces_of_descriptor_extractors.html#siftdescriptorextractor
    //SiftDescriptorExtractor extractor(3.0); //Magnification(used to compute the radius of the circle that wraps a square patch (by multiplying scale, magnification factor and width of descriptor histogram))

	//Detector documentation: http://docs.opencv.org/modules/features2d/doc/common_interfaces_of_feature_detectors.html
    //detector.detect(image, keypoints);

	// Feature descriptor computation
    //extractor.compute(image, keypoints, descriptors);

    /*
    Mat mask;
    BRISK b(30, 3, 1.0f);
    //Number of rows in descriptors is equal to the number of points in keypoints
    b.detect(image, keypoints, mask);
    b.compute(image, keypoints, descriptors);
    */


    Mat mask;
    ORB o(500, 1.2f, 8, 31, 0, 2, ORB::HARRIS_SCORE, 31);
    o.detect(image, keypoints, mask);
    o.compute(image, keypoints, descriptors);


}

void FindGoodMatches(Mat &descriptors1, Mat &descriptors2, vector<KeyPoint> &keypoints1, vector<KeyPoint> &keypoints2, vector<Point2f> &goodmatches1, vector<Point2f> &goodmatches2)
{
	//-- Matching descriptor vectors using FLANN matcher

    /*
	//Documentation for FlannBasedMatches: http://docs.opencv.org/modules/features2d/doc/common_interfaces_of_descriptor_matchers.html#flannbasedmatcher
	FlannBasedMatcher matcher;
	vector<DMatch> matches;
	matcher.match(descriptors1, descriptors2, matches);
    */


    BFMatcher matcher(NORM_HAMMING);
    std::vector< DMatch > matches;
    matcher.match( descriptors1, descriptors2, matches );


	double max_dist = 0; double min_dist = 100;

	//-- Calculation of max and min distances between keypoints
	for (int i = 0; i < descriptors2.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	//-- Use only "good" matches (i.e. whose distance is less than 3*min_dist )
	vector< DMatch > good_matches;
	for (int i = 0; i < descriptors2.rows; i++)
	{
		if (matches[i].distance < 3 * min_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}

	for (int i = 0; i < good_matches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		goodmatches1.push_back(keypoints1[good_matches[i].queryIdx].pt);
		goodmatches2.push_back(keypoints2[good_matches[i].trainIdx].pt);
	}

    cout << "GMatch1: " << goodmatches1.size() << endl;
    cout << "GMatch2: " << goodmatches2.size() << endl;
}

Mat combineTwoImages(Mat &image1, Mat &image2, vector<Point2f> &goodmatches1, vector<Point2f> &goodmatches2)
{
	//TODO: 
	//Functions you will need: findHomography, warpPerspective
    Mat result, H;
    H = findHomography(goodmatches1,goodmatches2,0);
    warpPerspective(image2,result, H, Size(image2.cols, image2.rows));
	return result;
}

