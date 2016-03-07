/**
CS585_Assignment4.cpp

CS585 Image and Video Computing Fall 2015
Assignment 4: AQUARIUM
*/

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

/**
TODO: Read the segmented images of the aquarium dataset and create the following:
a) A binary image where background pixels are labelled 0, and object pixels are
labelled 255
b) A labelled image where background pixels are labelled 0, and object pixels are
labelled numerically
c) A 3-channel image where the object centroids are drawn as red dots on the binary image
*/

Mat img;
Mat channel[3];

int main()
{
    int rmin=83, rmax=255, bmin=0, bmax=73, gmin=0, gmax=139;
    namedWindow("Red filter values");
    createTrackbar("Blue low", "Red filter values", &bmin, 255);
    createTrackbar("Blue high", "Red filter values", &bmax, 255);
    createTrackbar("Green low", "Red filter values", &gmin, 255);
    createTrackbar("Green high", "Red filter values", &gmax, 255);
    createTrackbar("Red low", "Red filter values", &rmin, 255);
    createTrackbar("Red high", "Red filter values", &rmax, 255);
    img = imread("Segmentation_Aqua/2014_aq_segmented 01.jpg",1);
    split(img,channel);
    Mat output;

    while(1)
       {
           imshow("Red channel", channel[2]);
           inRange(img, Scalar(bmin, gmin, rmin), Scalar(bmax, gmax, rmax), output);

           imshow("Red filter values", output);
           //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
           if (waitKey(30) == 27)
           {
               cout << "esc key is pressed by user" << endl;
               break;
           }
       }

    //inRange(channel[2], Scalar(rmin), Scalar(rmax), output);
    //imshow("Red Channel",channel[2]);
    //imshow("Red filter Values",output);
    waitKey(0);
	return 0;
}

