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
void DetectShape(Mat &img_bw, Mat &img_color, vector<Mat> &vec_templ);
void MatchingMethod(Mat &img_bw, Mat &img_color, Mat &templ);
void mySkinDetect(Mat& src, Mat& dst);
int myMax(int a, int b, int c);
int myMin(int a, int b, int c);

char* image_window = "Source Image";
char* result_window = "Result window";


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

    Mat templ, templ1, templ2;
	//create a window called "MyVideoFrame0"
	namedWindow("MyVideo0", WINDOW_AUTOSIZE);
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
	//Save the output image
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY); // compression technique
	compression_params.push_back(98); //compression quality
    //bool bSuccess = imwrite("template4.jpg", frame0, compression_params);
	
    //Read the different templates
    templ = imread("hands.png", CV_32FC1);
    templ1 = imread("fist.png", CV_32FC1);
    templ2 = imread("thumbsup.png", CV_32FC1);
    cout << "Size of template: (" << templ.rows << "," << templ.cols << ")" << endl;


    Mat templDest, templDest1, templDest2;
    templDest = Mat::zeros(templ.rows, templ.cols, CV_8UC1);
    templDest1 = Mat::zeros(templ1.rows, templ1.cols, CV_8UC1);
    templDest2 = Mat::zeros(templ2.rows, templ2.cols, CV_8UC1);
    mySkinDetect(templ,templDest);
    mySkinDetect(templ1,templDest1);
    mySkinDetect(templ2,templDest2);

    vector<Mat> vec_shapes;
    vec_shapes.push_back(templDest);
    vec_shapes.push_back(templDest1);
    vec_shapes.push_back(templDest2);

    imshow("Open Hand Skin Detection",templDest);
    imshow("Fist Skin Detection",templDest1);
    imshow("Thumbs up Skin Detection",templDest2);
    //imshow("Open Hand",templ);
	
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
        mySkinDetect(frame, frameDest);
        imshow("MyVideo", frameDest);

        //Perform matching method through all the shapes
        //DetectShape(frameDest, frame, vec_shapes);
        MatchingMethod(frameDest,frame,templDest);

		//wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		if (waitKey(30) == 27)
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
		//imshow(image_window, templ);
	}
	cap.release();
	return 0;
}



void MatchingMethod(Mat &img_bw, Mat &img_color, Mat &templ)
{
	/// Source image to display
	Mat result;
	//Mat img = imread("pen.jpg", CV_32FC1);
	Mat img_display;
    img_color.copyTo(img_display);
    int match_method = CV_TM_CCORR_NORMED;
	/// Create the result matrix
    int result_cols = img_bw.cols - templ.cols + 1;
    int result_rows = img_bw.rows - templ.rows + 1;

	result.create(result_cols, result_rows, CV_32FC1);

	/// Do the Matching and Normalize
    matchTemplate(img_bw, templ, result, match_method);
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

	/// Localizing the best match with minMaxLoc
	double minVal; double maxVal; Point minLoc; Point maxLoc;
	Point matchLoc;

	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

	/// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
	if (match_method == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED)
	{
		matchLoc = minLoc;
	}
	else
	{
		matchLoc = maxLoc;
	}

	/// Show me what you got
	rectangle(img_display, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
	rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);

	imshow(image_window, img_display);
	imshow(result_window, result);

	return;
}

void DetectShape(Mat &img_bw, Mat &img_color, vector<Mat> &vec_templ)
{
    //TODO: Loop through all the elements in vec_templ and perform MatchingMethod, then
    //calculate the max value in the result matrix and compare them
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
