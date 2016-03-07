//opencv libraries
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//C++ standard libraries
#include <iostream>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;

//FOR USING THE CAMERA COMMENT LINES 26,27, UNCOMMENT 28,31,32,43

int main(int argc, char** argv)
{
    //int hmin=20, hmax=150, smin=77, smax=153, vmin=36, vmax=211;
    //int hmin=25, hmax=115, smin=40, smax=170, vmin=83, vmax=245; used with inRange function
    int hmin=0, hmax=62, smin=34, smax=195, vmin=72, vmax=185;
    namedWindow("Skin filter values");
    createTrackbar("Hue low", "Skin filter values", &hmin, 180);
    createTrackbar("Hue high", "Skin filter values", &hmax, 255);
    createTrackbar("Saturation low", "Skin filter values", &smin, 255);
    createTrackbar("Saturation high", "Skin filter values", &smax, 255);
    createTrackbar("Value low", "Skin filter values", &vmin, 255);
    createTrackbar("Value high", "Skin filter values", &vmax, 255);
    Mat output;
    Mat image, hsv;
    image = imread(argv[1],1);
    //cvtColor(image,hsv, CV_RGB2HSV);
    //imshow("input", image);
    //VideoCapture cap(argv[1]);
    //VideoCapture cap(0);
    while(1)
    {
        //cap.read(image);
        imshow("input", image);
        output.create(image.rows, image.cols, CV_8UC1);
        //cvtColor(image,hsv, CV_RGB2HSV);
        //inRange(hsv, Scalar(hmin,smin,vmin), Scalar(hmax,smax,vmax), output);
        //erode(output,output,Mat());
        //dilate(output,output,Mat());

        Vec3b intensity, intensity2;
        cvtColor(image,hsv,CV_BGR2HSV);
        uchar blue, green, red;
        uchar hue, sat, value;
        for (int i = 0; i<hsv.rows; i++)
        {
            for (int j = 0; j<hsv.cols; j++)
            {
                intensity2 = image.at<Vec3b>(i, j);
                blue = intensity2.val[0];
                green = intensity2.val[1];
                red = intensity2.val[2];
                intensity = hsv.at<Vec3b>(i, j);
                hue = intensity.val[0];
                sat = intensity.val[1];
                value = intensity.val[2];
                if (hue>hmin && hue<hmax && sat>smin && sat<smax && value>vmin && value<vmax)
                {
                    output.at<uchar>(i, j) = 0.299*red + 0.587*green + 0.114*blue;
                }
                else output.at<uchar>(i, j) = 0;
            }
        }
        //cout << "h" << endl;
        imshow("Result Window", output);
        //cout << "h" << endl;
        //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        if (waitKey(30) == 27)
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }
    }
    //cap.release();
    image.release();
    return 0;
}
