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
Mat MatchingMethod(Mat &img_bw, Mat &img_color, Mat &templ, string name, double &mmax, Point &matchLoc);
void mySkinDetect(Mat& src, Mat& dst);
int myMaxD(double* array, int size);

char* image_window = "Source Image";
char* result_window = "Result window";

int hmin=0, hmax=66, smin=43, smax=195, vmin=55, vmax=210;
int downsample_step = 1;
double shape_similarity[9];

int main(int argc, char** argv)
{
    //Images for the templates
    Mat templ, templ1, templ2;

    //Read the different templates
    templ = imread("TM_openhand.jpg", CV_32FC1);
    templ1 = imread("TM_peace.jpg", CV_32FC1);
    templ2 = imread("TM_up.jpg", CV_32FC1);


    Mat templDest, templDest1, templDest2; //Images for skinDetection of templates (SDT)
    vector<vector<Point> > cont, cont1, cont2; //Vector of contours
    vector<Point> max_cont, max_cont1, max_cont2; //Maximum contour (defines the shape of the hand)
    //Initialize the SDT's
    templDest = Mat::zeros(templ.rows, templ.cols, CV_8UC1);
    templDest1 = Mat::zeros(templ1.rows, templ1.cols, CV_8UC1);
    templDest2 = Mat::zeros(templ2.rows, templ2.cols, CV_8UC1);

    mySkinDetect(templ, templDest);
    mySkinDetect(templ1, templDest1);
    mySkinDetect(templ2, templDest2);

    //With this now we have all 3 contours called max_cont, max_cont1 and max_cont2

    //Showing the Skin Detection of each template
    imshow("Open Hand Skin Detection",templDest);
    imshow("Peace Skin Detection",templDest1);
    imshow("Rock Skin Detection",templDest2);

    //Create 3 vector<Point> that will store the contour for the camera using matching template w the 3 templates
    Mat cam_cont, cam_cont1, cam_cont2;
    /*Mat frame;
    Mat frameDest, temp_frame;
    frame = imread(argv[1],1);
    temp_frame = Mat::zeros(frame.rows, frame.cols, CV_8UC1);
    mySkinDetect(frame, temp_frame);
    double max[3];
    //Perform matching method through all the shapes
    cam_cont = MatchingMethod(temp_frame, frame, templDest, "Open Hand",max[0]);
    cam_cont1 = MatchingMethod(temp_frame, frame, templDest1, "Peace",max[1]);
    cam_cont2 = MatchingMethod(temp_frame, frame, templDest2, "Rock",max[2]);
    cout << "OH:" << max[0] << " , " << "Spock:" << max[1] << " , " << "Extremes:" << max[2] << endl;
    int r = myMaxD(max, 3);
    imshow("OpenHand", cam_cont);
    imshow("Spock", cam_cont1);
    imshow("Extreme", cam_cont2);*/
    //VideoCapture cap(0);
    VideoCapture cap(argv[1]);
    while (1)
    {
        // read a new frame from video
        Mat frame0, frame;
        //bool bSuccess = cap.read(frame0);
        bool bSuccess = cap.read(frame);

        //if not successful, break loop
        if (!bSuccess)
        {
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }
        //bilateralFilter(frame0, frame, 9, 17, 17);
        Mat frameDest, temp_frame;
        temp_frame = Mat::zeros(frame.rows, frame.cols, CV_8UC1);
        mySkinDetect(frame, temp_frame);
        double max[3];
        Point pt1, pt2, pt3;
        //Perform matching method through all the shapes
        cam_cont = MatchingMethod(temp_frame, frame, templDest, "Open Hand",max[0],pt1);
        cam_cont1 = MatchingMethod(temp_frame, frame, templDest1, "Peace",max[1],pt2);
        cam_cont2 = MatchingMethod(temp_frame, frame, templDest2, "Rock",max[2],pt3);
        //cout << "OH:" << max[0] << " , " << "Spock:" << max[1] << " , " << "Extremes:" << max[2] << endl;
        string txt;
        int r = myMaxD(max, 3);
         cout<< r<<endl;
         cout << "Point" << pt1.x <<"," << pt1.y << endl;
         if (r == 0) { txt = "This is a OPEN HAND"; cout << "OPEN HAND" << endl;}
         if (r == 1) { txt = "This is a PEACE";cout << "PEACE" << endl;}
         if (r == 2) { txt = "This is a RIGHT";cout << "RIGHT" << endl;}
        putText(cam_cont,txt,pt1,1,2,Scalar(255,255,0),3);
        imshow("Matching",cam_cont);

        //Output of the program. Here you can visualize the values of the matchShapes and output what shape is recognizing
        //cout << "Coefficients: " << shape_similarity[0] << " , " << shape_similarity[1] << " , " << shape_similarity[2] << endl;
        //if (max_index % 3 == 0) cout << "OPEN HAND" << endl;
        //if (max_index % 3 == 1) cout << "PEACE" << endl;
        //if (max_index % 3 == 2) cout << "ROCK" << endl;

        //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        if (waitKey(30) == 27)
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }
    }
    //cap.release();
    waitKey(0);
    return 0;
}



Mat MatchingMethod(Mat &img_bw, Mat &img_color, Mat &templ, string name, double &maxVal, Point& matchLoc)
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
    //normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

    /// Localizing the best match with minMaxLoc
    double minVal; Point minLoc; Point maxLoc;
    //Point matchLoc;

    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
    //cout << maxVal<<endl;
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

    //imshow(name, img_display);
    Rect r(matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows));
    Mat ROI = img_bw(r);

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    findContours(ROI, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    //		4. Find the largest contour
    int idx = 0;
    double max_area = 0; int max_contour_idx;
    for (; idx < contours.size(); idx++)
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
    Scalar color = Scalar(255, 0, 0);
    Mat drawing = Mat::zeros(ROI.size(), CV_8UC3);

    drawContours(img_display, contours, max_contour_idx, color, 3, 8, hierarchy, 0, matchLoc);
    //pt.x = matchLoc.x; pt.y = matchLoc.y;
    //pt = matchLoc;
    //string txt = "This is a " + name;
    //putText(img_display,txt,matchLoc,1,2,Scalar(255,255,0),3);
    //if (flag==1)
        //imshow(name, img_display);


    //imshow(image_window, img_display);
    //imshow(result_window, result);
    //imshow("ROI", ROI);

    //return contours[max_contour_idx];
    return img_display;
}

///Calculates the maximum index of the array
int myMaxD(double* array, int size)
{
    double max = array[0];
    int max_pos = 0;
    int i;
    for (i = 0; i<size; i++)
    {
        if (max<array[i])
        {
            max = array[i];
            max_pos = i;
        }
    }
    return max_pos;
}


//Function that detects whether a pixel belongs to the skin based on RGB values
void mySkinDetect(Mat& src, Mat& dst) {
    Mat hsv;
    Vec3b intensity, intensity2;
    cvtColor(src,hsv,CV_BGR2HSV);
    uchar blue, green, red;
    uchar hue, sat, value;
    for (int i = 0; i<hsv.rows; i++)
    {
        for (int j = 0; j<hsv.cols; j++)
        {
            intensity2 = src.at<Vec3b>(i, j);
            blue = intensity2.val[0];
            green = intensity2.val[1];
            red = intensity2.val[2];
            intensity = hsv.at<Vec3b>(i, j);
            hue = intensity.val[0];
            sat = intensity.val[1];
            value = intensity.val[2];
            if (hue>hmin && hue<hmax && sat>smin && sat<smax && value>vmin && value<vmax)
            {
                dst.at<uchar>(i, j) = 0.299*red + 0.587*green + 0.114*blue;
            }
            else dst.at<uchar>(i, j) = 0;
        }
    }
}
