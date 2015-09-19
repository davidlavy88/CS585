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
vector<Point> MatchingMethod(Mat &img_bw, Mat &img_color, Mat &templ, string name);
void mySkinDetect(Mat& src, Mat& dst);
int myMax(int a, int b, int c);
int myMaxD(double* array, int size);
int myMin(int a, int b, int c);
vector<Point> myContour(Mat &src, vector < vector< Point > > &contours, string name, int &largest, vector<Vec4i> &hierarchy);

char* image_window = "Source Image";
char* result_window = "Result window";

double shape_similarity[3];


int main(int argc, char** argv)
{

    // Reading the camera
	VideoCapture cap(0);

	// if not successful, exit program
	if (!cap.isOpened())
	{
		cout << "Cannot open the video cam" << endl;
		return -1;
	}

    //Images for the templates
    Mat templ, templ1, templ2;

    /*
	// read a new frame from video
	bool bSuccess0 = cap.read(frame0);

	//if not successful, break loop
	if (!bSuccess0)
	{
		cout << "Cannot read a frame from video stream" << endl;
    } */
	
    //Read the different templates
    templ = imread("hands.png", CV_32FC1);
    templ1 = imread("peace.png", CV_32FC1);
    templ2 = imread("rock.png", CV_32FC1);


    Mat templDest, templDest1, templDest2; //Images for skinDetection of templates (SDT)
    vector<vector<Point> > cont, cont1, cont2; //Vector of contours
    vector<Point> max_cont, max_cont1, max_cont2; //Maximum contour (defines the shape of the hand)
    //Initialize the SDT's
    templDest = Mat::zeros(templ.rows, templ.cols, CV_8UC1);
    templDest1 = Mat::zeros(templ1.rows, templ1.cols, CV_8UC1);
    templDest2 = Mat::zeros(templ2.rows, templ2.cols, CV_8UC1);
    //Perform skinDetection
    mySkinDetect(templ,templDest);
    mySkinDetect(templ1,templDest1);
    mySkinDetect(templ2,templDest2);

    vector<Vec4i> hier, hier1, hier2; //to be used in the findContour function

    //COPIED FROM LAB 3

    //Finding contour for template 1
    findContours(templDest, cont, hier, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    //Find the largest contour
    int idx=0;
    double max_area = 0; int max_contour_idx;
    for( ; idx < cont.size() ; idx++ )
    {
        double area = contourArea(cont[idx]);
        if (area > max_area)
        {
            max_area = area;
            max_contour_idx = idx;
        }
    }
    max_cont = cont[max_contour_idx];
    //Draw the object with the largest contour as well as its outline on a new image
    Scalar color = Scalar( 255,0, 0 );
    Mat drawing = Mat::zeros( templDest.size(), CV_8UC3 );
    drawContours(drawing, cont, max_contour_idx, color, 3, 8, hier, 0, Point());
    imshow("Contour Template 1 ",drawing); //Show the contour of SDT 1

    //Finding contour for template 2
    findContours(templDest1, cont1, hier1, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    //Find the largest contour
    idx=0;
    max_area = 0;
    for( ; idx < cont1.size() ; idx++ )
    {
        double area = contourArea(cont1[idx]);
        if (area > max_area)
        {
            max_area = area;
            max_contour_idx = idx;
        }
    }
    max_cont1 = cont1[max_contour_idx];
    //Draw the object with the largest contour as well as its outline on a new image
    Mat drawing1 = Mat::zeros( templDest1.size(), CV_8UC3 );
    drawContours(drawing1, cont1, max_contour_idx, color, 3, 8, hier, 0, Point());
    imshow("Contour Template 2 ",drawing1); //Show the contour of SDT 2

    //Finding contour for template 3
    findContours(templDest2, cont2, hier2, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    //Find the largest contour
    idx=0;
    max_area = 0;
    for( ; idx < cont2.size() ; idx++ )
    {
        double area = contourArea(cont2[idx]);
        if (area > max_area)
        {
            max_area = area;
            max_contour_idx = idx;
        }
    }
    max_cont2 = cont2[max_contour_idx];
    //Draw the object with the largest contour as well as its outline on a new image
    Mat drawing2 = Mat::zeros( templDest2.size(), CV_8UC3 );
    drawContours(drawing2, cont2, max_contour_idx, color, 3, 8, hier, 0, Point());
    imshow("Contour Template 3 ",drawing2); //Show the contour of SDT 3

    //With this now we have all 3 contours called max_cont, max_cont1 and max_cont2

    //Showing the Skin Detection of each template
    //imshow("Open Hand Skin Detection",templDest);
    //imshow("Peace Skin Detection",templDest1);
    //imshow("Rock Skin Detection",templDest2);
	
    //Create 3 vector<Point> that will store the contour for the camera using matching template w the 3 templates
    vector<Point> cam_cont, cam_cont1, cam_cont2;
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

        //Skin Detection of the streaming
        mySkinDetect(frame, frameDest);
        imshow("MyVideo", frameDest);

        //Perform matching method through all the shapes
        cam_cont = MatchingMethod(frameDest,frame,templDest,"Open Hand");
        cam_cont1 = MatchingMethod(frameDest,frame,templDest1,"Peace");
        cam_cont2 = MatchingMethod(frameDest,frame,templDest2,"Rock");

        //Now calculate shape similarity between each correspondent template contour and camera contour
        shape_similarity[0] = matchShapes(cam_cont, max_cont, 1, 1);
        shape_similarity[1] = matchShapes(cam_cont1, max_cont1, 1, 1);
        shape_similarity[2] = matchShapes(cam_cont2, max_cont2, 1, 1);

        //Find the maximum value of shape similarity array
        int max_index;
        max_index = myMaxD(shape_similarity,3);

        //Output of the program. Here you can visualize the values of the matchShapes and output what shape is recognizing
        cout << "Coefficients: " << shape_similarity[0] << " , " << shape_similarity[1] << " , " << shape_similarity[2] << endl;
        if (max_index == 0) cout << "OPEN HAND" << endl;
        if (max_index == 1) cout << "PEACE" << endl;
        if (max_index == 2) cout << "ROCK" << endl;

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



vector<Point> MatchingMethod(Mat &img_bw, Mat &img_color, Mat &templ, string name)
{
	/// Source image to display
	Mat result;
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

    Rect r(matchLoc,Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows));
    Mat ROI = img_bw(r);

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    findContours(ROI, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
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
    Scalar color = Scalar( 255,0, 0 );
    Mat drawing = Mat::zeros( ROI.size(), CV_8UC3 );
    drawContours(drawing, contours, max_contour_idx, color, 3, 8, hierarchy, 0, Point());
    imshow(name,drawing);

    //imshow(image_window, img_display);
    //imshow(result_window, result);
    //imshow("ROI", ROI);

    return contours[max_contour_idx];
}

int myMax(int a, int b, int c) {
    int m = a;
    (void)((m < b) && (m = b));
    (void)((m < c) && (m = c));
     return m;
}

///Calculates the maximum index of the array
int myMaxD(double* array, int size)
{
    double max = array[0];
    int max_pos = 0;
    int i;
    for (i=0; i<size; i++)
    {
        if (max<array[i])
        {
            max = array[i];
            max_pos = i;
        }
    }
    return max_pos;
}

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

///Returns the maximum contour
vector<Point> myContour(Mat &src, vector < vector< Point > > &contours, string name, int &largest, vector<Vec4i> &hierarchy)
{
    double area;
    double largest_area = 0;
    largest = 0;
    findContours(src, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    Mat drawing = Mat::zeros(src.size(), CV_8UC3);
    for (int i = 0; i < contours.size(); i++){
        area = contourArea(contours[i], false);
        if (area > largest_area){
            largest = i;
            largest_area = area;
        }
    }

    //drawContours(src, contours, largest_contour_index, Scalar(255, 255, 255), 2, 8, hierarchy, 0, Point());
    //imshow(name, src);

    return contours[largest];
}
