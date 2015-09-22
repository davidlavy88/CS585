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
Mat MatchingMethod(Mat &img_bw, Mat &img_color, Mat &templ, string name);
void mySkinDetect(Mat& src, Mat& dst);
int myMax(int a, int b, int c);
int myMaxD(double* array, int size);
int myMin(int a, int b, int c);
vector<Point> myContour(Mat &src, vector < vector< Point > > &contours, string name, int &largest, vector<Vec4i> &hierarchy);

char* image_window = "Source Image";
char* result_window = "Result window";

int rmin=45, rmax=192, bmin=18, bmax=147, gmin=76, gmax=151;

double shape_similarity[3];


int main(int argc, char** argv)
{
    //Images for the templates
    Mat templ, templ1, templ2;

    //Read the different templates
    templ = imread("openhand1.png", CV_32FC1);
    templ1 = imread("peace1.png", CV_32FC1);
    templ2 = imread("right1.png", CV_32FC1);


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

    erode(templDest,templDest,Mat());
    erode(templDest1,templDest1,Mat());
    erode(templDest2,templDest2,Mat());

    dilate(templDest,templDest,Mat());
    dilate(templDest1,templDest1,Mat());
    dilate(templDest2,templDest2,Mat());

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
    Scalar color = Scalar( 255,255, 255 );
    Mat drawing = Mat::zeros( templDest.size(), CV_8UC1 ); //changed from 8UC3 to 8UC1
    drawContours(drawing, cont, max_contour_idx, color, -3, 8, hier, 0, Point());
    imshow("Contour Template 1 ",drawing); //Show the contour of SDT 1

    Rect r = boundingRect(max_cont);
    Mat nuvo = drawing(r);
    imshow("Bbox 1",nuvo);

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
    Mat drawing1 = Mat::zeros( templDest1.size(), CV_8UC1 );
    drawContours(drawing1, cont1, max_contour_idx, color, -3, 8, hier, 0, Point());
    imshow("Contour Template 2 ",drawing1); //Show the contour of SDT 2

    Rect r1 = boundingRect(max_cont1);
    Mat nuvo1 = drawing1(r1);
    imshow("Bbox 2",nuvo1);

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
    Mat drawing2 = Mat::zeros( templDest2.size(), CV_8UC1 );
    drawContours(drawing2, cont2, max_contour_idx, color, -3, 8, hier, 0, Point());
    imshow("Contour Template 3 ",drawing2); //Show the contour of SDT 3

    Rect r2 = boundingRect(max_cont2);
    Mat nuvo2 = drawing2(r2);
    imshow("Bbox 3",nuvo2);

    //With this now we have all 3 contours called max_cont, max_cont1 and max_cont2

    //Testing with a random pic

    //Create 3 vector<Point> that will store the contour for the camera using matching template w the 3 templates
    //vector<Point> cam_cont, cam_cont1, cam_cont2;
    Mat cam_cont, cam_cont1, cam_cont2;

    Mat test = imread("test_right.jpg");
    Mat templtest = Mat::zeros(test.rows, test.cols, CV_8UC1);
    //Perform skinDetection
    mySkinDetect(test,templtest);
    erode(templtest,templtest,Mat());
    dilate(templtest,templtest,Mat());

    //Perform matching method through all the shapes
    //cam_cont = MatchingMethod(templtest,test,templDest,"Open Hand"); //Original function w full size template
    cam_cont = MatchingMethod(templtest,test,templDest,"Open Hand");
    cam_cont1 = MatchingMethod(templtest,test,templDest1,"Peace");
    cam_cont2 = MatchingMethod(templtest,test,templDest2,"Rock");

    /*Rect cr = boundingRect(cam_cont); Rect cr1 = boundingRect(cam_cont1); Rect cr2 = boundingRect(cam_cont2);
    Mat btest = templtest(cr); Mat btest1 = templtest(cr1); Mat btest2 = templtest(cr2);
    imshow("CBbox 1",btest); imshow("CBbox 2",btest1); imshow("CBbox 3",btest2);

    //Resize the boxes to match the templates
    Mat res_btest, res_btest1, res_btest2;
    resize(btest,res_btest,nuvo.size());
    resize(btest1,res_btest1,nuvo1.size());
    resize(btest2,res_btest2,nuvo2.size());
    imshow("Resized 1",res_btest); imshow("Resized 2",res_btest1); imshow("Resized 3",res_btest2);*/

    //Perform template matching once again

    // Wait until keypress
    waitKey(0);
    return(0);
}



Mat MatchingMethod(Mat &img_bw, Mat &img_color, Mat &templ, string name)
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
    imshow(name, img_display);

    Rect r(matchLoc,Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows));
    //Mat ROI = img_bw(r);
    Mat ROI = img_bw(r);
    Mat ROI2 = img_color(r);

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
    Rect rr = boundingRect(contours[max_contour_idx]);
    Mat nn = ROI2(rr);

    Mat res_nn;
    resize(nn,res_nn,templ.size());
    //		5. Draw the object with the largest contour as well as its outline on a new image
    Scalar color = Scalar( 255, 255, 255 );
    Mat drawing = Mat::zeros( ROI.size(), CV_8UC1 );
    drawContours(drawing, contours, max_contour_idx, color, -3, 8, hierarchy, 0, Point());
    //drawContours(ROI, contours, max_contour_idx, color, 3, 8, hierarchy, 0, Point());
    //imshow(name,drawing);

    //imshow(name,res_nn);
    cout << max_contour_idx << endl; //THIS NUMBER IS WAAAY TO HIGH WHEN initiazling camera

    //imshow(image_window, img_display);
    //imshow(name, ROI2);
    //imshow(result_window, result);
    //imshow("ROI", ROI);

    //return contours[max_contour_idx];
    return ROI;
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
    inRange(src, Scalar(bmin,gmin,rmin), Scalar(bmax,gmax,rmax), dst);
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
