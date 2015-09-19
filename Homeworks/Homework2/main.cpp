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
//void DetectShape(Mat &img_bw, Mat &img_color, vector<Mat> &vec_templ);
int detectShape(vector< Mat > &src, Mat &templ1, Mat &templ2, Mat &templ3);
Mat MatchingMethod(Mat &img_bw, Mat &img_color, Mat &templ);
void mySkinDetect(Mat& src, Mat& dst);
int myMax(int a, int b, int c);
int myMin(int a, int b, int c);

vector<Point> myContour(Mat &src, vector < vector< Point > > &contours, string name);

char* image_window = "Source Image";
char* result_window = "Result window";

int shape_similarity[3];
double temp = 0;
int max_index = 0;

int main(int argc, char** argv)
{
	VideoCapture cap(0);

	// if not successful, exit program
	if (!cap.isOpened())
	{
		cout << "Cannot open the video cam" << endl;
		return -1;
	}

    Mat templ, templ1, templ2;
	
    //Read the different templates
    templ = imread("hands.png", CV_32FC1); //CHANGE THIS VALUE FOR THE IMAGE YOU WANT
    //templ = imread(argv[1], CV_32FC1);
    templ1 = imread("peace.png", CV_32FC1);
    templ2 = imread("thumbsup2.png", CV_32FC1);
    cout << "Size of template: (" << templ.rows << "," << templ.cols << ")" << endl;


    Mat templDest, templDest1, templDest2;
    templDest = Mat::zeros(templ.rows, templ.cols, CV_8UC1);
    templDest1 = Mat::zeros(templ1.rows, templ1.cols, CV_8UC1);
    templDest2 = Mat::zeros(templ2.rows, templ2.cols, CV_8UC1);
    mySkinDetect(templ,templDest);
    mySkinDetect(templ1,templDest1);
    mySkinDetect(templ2,templDest2);

    erode(templDest,templDest,Mat());
    erode(templDest1,templDest1,Mat());
    erode(templDest2,templDest2,Mat());

    dilate(templDest,templDest,Mat());
    dilate(templDest1,templDest1,Mat());
    dilate(templDest2,templDest2,Mat());

    //imshow("Open Hand Skin Detection",templDest);
    //imshow("Fist Skin Detection",templDest1);
    //imshow("Thumbs up Skin Detection",templDest2);
	
    vector< Mat > extractedROI;
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
        Mat tt, tt1, tt2;
        tt = MatchingMethod(frameDest,frame,templDest); extractedROI.push_back(tt);
        tt1 = MatchingMethod(frameDest,frame,templDest1); extractedROI.push_back(tt1);
        tt2 = MatchingMethod(frameDest,frame,templDest2); extractedROI.push_back(tt2);

        imshow("TM Hand", tt);
        imshow("TM Piece", tt1);
        imshow("TM Thumbsup", tt2);

        int what = detectShape(extractedROI, templDest, templDest1, templDest2);

        //cout << what << endl;

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



Mat MatchingMethod(Mat &img_bw, Mat &img_color, Mat &templ)
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

    Rect r(matchLoc,Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows));
    Mat ROI = img_bw(r);

    /*vector<vector<Point> > contours;
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
    imshow("Contour image",drawing);

	imshow(image_window, img_display);
    //imshow(result_window, result);
    //imshow("ROI", ROI);*/

    return ROI;
}

/*void DetectShape(Mat &img_bw, Mat &img_color, vector<Mat> &vec_templ)
{
    //TODO: Loop through all the elements in vec_templ and perform MatchingMethod, then
    //calculate the max value in the result matrix and compare them
    for (vector<Mat>::iterator it = vec_templ.begin(); it!=vec_templ.end(); ++it)
    {

    }
}*/

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

vector<Point> myContour(Mat &src, vector < vector< Point > > &contours, string name)
{
    double area;
    double largest_area = 0;
    int largest_contour_index = 0;
    vector<Vec4i> hierarchy;
    findContours(src, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    Mat drawing = Mat::zeros(src.size(), CV_8UC3);
    for (int i = 0; i < contours.size(); i++){
        area = contourArea(contours[i], false);
        if (area > largest_area){
            largest_contour_index = i;
            largest_area = area;
        }
    }

    drawContours(src, contours, largest_contour_index, Scalar(255, 255, 255), 2, 8, hierarchy, 0, Point());
    imshow(name, src);

    return contours[largest_contour_index];
}

int detectShape(vector<Mat> &src, Mat &templ1, Mat &templ2, Mat &templ3)
{

    vector <Mat> templ;
    templ.push_back(templ1);
    templ.push_back(templ2);
    templ.push_back(templ3);

    vector<vector<Point> > src_contours, templ_contours;


    int method = 1;
    vector<Point> contour_src, contour_templ;

    for (int j = 0; j < templ.size(); j++)
    {

        //Find all contours in the thresholded image
        contour_src = myContour(src[j],src_contours, "Source Contour");
        contour_templ = myContour(templ[j], templ_contours, "Template contour");
        //cout << "Hello" << endl;
        shape_similarity[j]=matchShapes(contour_src, contour_templ, method,1);
        cout << "Hello" << endl;
        if (shape_similarity[j]>temp){
            max_index = j;
            temp = shape_similarity[j];
        }
        return max_index;
    }
}



