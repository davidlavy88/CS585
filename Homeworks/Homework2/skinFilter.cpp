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
    int rmin=0, rmax=0, bmin=0, bmax=0, gmin=0, gmax=0;
    namedWindow("Skin filter values");
    createTrackbar("Blue low", "Skin filter values", &bmin, 255);
    createTrackbar("Blue high", "Skin filter values", &bmax, 255);
    createTrackbar("Green low", "Skin filter values", &gmin, 255);
    createTrackbar("Green high", "Skin filter values", &gmax, 255);
    createTrackbar("Red low", "Skin filter values", &rmin, 255);
    createTrackbar("Red high", "Skin filter values", &rmax, 255);
    Mat output;
    Mat image;
    image = imread(argv[1],1);
    imshow("input", image);
    //VideoCapture cap(0);
    while(1)
    {
        //cap.read(image);
        //imshow("input", image);
        inRange(image, Scalar(bmin,gmin,rmin), Scalar(bmax,gmax,rmax), output);

        imshow("Result Window", output);
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
