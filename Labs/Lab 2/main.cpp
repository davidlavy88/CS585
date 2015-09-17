//opencv libraries
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//C++ standard libraries
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

//function declarations

/**
	Function that returns the maximum of 3 integers
	@param a first integer
	@param b second integer
	@param c third integer
*/
int myMax(int a, int b, int c);

/**
	Function that returns the minimum of 3 integers
	@param a first integer
	@param b second integer
	@param c third integer
*/
int myMin(int a, int b, int c);

/**
	Function that detects whether a pixel belongs to the skin based on RGB values
	@param src The source color image
	@param dst The destination grayscale image where skin pixels are colored white and the rest are colored black
*/
void mySkinDetect(Mat& src, Mat& dst);

/**
	Function that does frame differencing between the current frame and the previous frame
	@param src The current color image
	@param prev The previous color image
	@param dst The destination grayscale image where pixels are colored white if the corresponding pixel intensities in the current
				and previous image are not the same
*/
void myFrameDifferencing(Mat& prev, Mat& curr, Mat& dst);

/**
	Function that accumulates the frame differences for a certain number of pairs of frames
	@param mh Vector of frame difference images
	@param dst The destination grayscale image to store the accumulation of the frame difference images
*/
void myMotionEnergy(vector<Mat> mh, Mat& dst);

int main()
{

//----------------
//a) Reading a stream of images from a webcamera, and displaying the video
//----------------
	// For more information on reading and writing video: http://docs.opencv.org/modules/highgui/doc/reading_and_writing_images_and_video.html
	// open the video camera no. 0
	VideoCapture cap(0);

	// if not successful, exit program
    if (!cap.isOpened())
    {
        cout << "Cannot open the video cam" << endl;
        return -1;
    }

	//create a window called "MyVideoFrame0"
    namedWindow("MyVideo0",WINDOW_AUTOSIZE);
	Mat frame0;

	// read a new frame from video
	bool bSuccess0 = cap.read(frame0);

	//if not successful, break loop
    if (!bSuccess0)
	{
			cout << "Cannot read a frame from video stream" << endl;
	}

	//show the frame in "MyVideo" window
	imshow("MyVideo0", frame0);

    //show Skin detection of first frame
    Mat frameSkin;
    frameSkin = Mat::zeros(frame0.rows, frame0.cols, CV_8UC1); //Returns a zero array of same size as src mat, and of type CV_8UC1

    mySkinDetect(frame0, frameSkin);
    imshow("MyVideo", frameSkin);

	//create a window called "MyVideo"
	namedWindow("MyVideo",WINDOW_AUTOSIZE);
	namedWindow("MyVideoMH",WINDOW_AUTOSIZE);

	/*vector<Mat> myMotionHistory;
	Mat fMH1, fMH2, fMH3;
	fMH1 = Mat::zeros(frame0.rows, frame0.cols, CV_8UC1);
	fMH2 = fMH1.clone();
	fMH3 = fMH1.clone();
	myMotionHistory.push_back(fMH1);
	myMotionHistory.push_back(fMH2);
	myMotionHistory.push_back(fMH3);*/

	while (1)
    {
		// read a new frame from video
        Mat frame;
        bool bSuccess = cap.read(frame);

		//if not successful, break loop
        if (!bSuccess)
        {
             cout << "Cannot read a frame from video stream" << endl;
             break;
        }

		// destination frame
		Mat frameDest;
		frameDest = Mat::zeros(frame.rows, frame.cols, CV_8UC1); //Returns a zero array of same size as src mat, and of type CV_8UC1
//----------------
//	b) Skin color detection
//----------------
        //mySkinDetect(frame, frameDest);
        //imshow("MyVideo", frameDest);

//----------------
//	c) Background differencing
//----------------


		//call myFrameDifferencing function
		/*myFrameDifferencing(frame0, frame, frameDest);
		imshow("MyVideo", frameDest);
		myMotionHistory.erase(myMotionHistory.begin());
		myMotionHistory.push_back(frameDest);
		Mat myMH = Mat::zeros(frame0.rows, frame0.cols, CV_8UC1);*/

//----------------
//  d) Visualizing motion history
//----------------

		//call myMotionEnergy function
		/*myMotionEnergy(myMotionHistory, myMH);


        imshow("MyVideoMH", myMH); //show the frame in "MyVideo" window
		frame0 = frame;*/
		//wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        if (waitKey(30) == 27)
       {
            cout << "esc key is pressed by user" << endl;
            break;
       }

    }
	cap.release();
	return 0;
}

//Function that returns the maximum of 3 integers
int myMax(int a, int b, int c) {
	int m = a;
    (void)((m < b) && (m = b));
    (void)((m < c) && (m = c));
     return m;
}

//Function that returns the minimum of 3 integers
int myMin(int a, int b, int c) {
	int m = a;
    (void)((m > b) && (m = b));
    (void)((m > c) && (m = c));
     return m;
}

//Function that detects whether a pixel belongs to the skin based on RGB values
void mySkinDetect(Mat& src, Mat& dst) {
	//Surveys of skin color modeling and detection techniques:
	//Vezhnevets, Vladimir, Vassili Sazonov, and Alla Andreeva. "A survey on pixel-based skin color detection techniques." Proc. Graphicon. Vol. 3. 2003.
	//Kakumanu, Praveen, Sokratis Makrogiannis, and Nikolaos Bourbakis. "A survey of skin-color modeling and detection methods." Pattern recognition 40.3 (2007): 1106-1122.

	//TODO:
	//Use the following test for skin color detection
	//Red > 95 and Blue > 20 and Green > 40, and
	//max(Red, Green, Blue) - min(Red, Green, Blue) > 15, and
	//abs(Red - Green) > 15, and
	//Red > Green, and
	//Red > Blue
	//cv::Mat* BGRchannels;
	Vec3b intensity;
	uchar blue, green, red;
	//cv::split(src,BGRchannels);
	//cout << BGRchannels[0].at<uchar>(3,2) << endl;
	for (int i=0;i<src.rows;i++)
	{
        for (int j=0;j<src.cols;j++)
        {
            intensity = src.at<Vec3b>(i,j);
            blue = intensity.val[0];
            green = intensity.val[1];
            red = intensity.val[2];
            if (blue>20 && green>40 && red>95 && (myMax((int)blue,(int)green,(int)red)-myMin((int)blue,(int)green,(int)red)>15) && abs(red-green) && red>green && red>blue)
            {
                dst.at<uchar>(i,j) = 255;
            }
            else dst.at<uchar>(i,j) = 0;
        }
	}

}

//Function that does frame differencing between the current frame and the previous frame
void myFrameDifferencing(Mat& prev, Mat& curr, Mat& dst) {
	//For more information on operation with arrays: http://docs.opencv.org/modules/core/doc/operations_on_arrays.html
	//For more information on how to use background subtraction methods: http://docs.opencv.org/trunk/doc/tutorials/video/background_subtraction/background_subtraction.html

	//TODO:
	//Take the pixel-wise absolute difference of the previous and current frames
	//If the absolute differences are greater than a threshold, color the pixel white, else leave it black

}

//Function that accumulates the frame differences for a certain number of pairs of frames
void myMotionEnergy(vector<Mat> mh, Mat& dst) {

	//TODO:
	//For all the images in the vector mh, color the pixel in image dst white, if at least one of the corresponding pixels in the
	//	images are white, else leave it black

}
