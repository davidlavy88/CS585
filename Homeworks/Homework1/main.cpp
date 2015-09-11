/*#include <stdlib.h>
#include <cv.hpp>
#include <cxcore.hpp>
#include <highgui.h>

int main(int argc, char** argv)
{
    cv::Mat frame;
    int c;
    CvCapture *capture = cvCaptureFromCAM(0);
    while(1)
    {
        frame = cvQueryFrame(capture);
        cv::imshow("OpenCV",frame);

        c = cv::waitKey(10);
        if(c==27)
            break;
    }
    return 0;
}*/


#include <stdlib.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>

int main(int argc, char** argv)
{
    cv::Mat img, img_gray, img_canny;
    img = cv::imread("photo.jpg");
    cv::Canny(img, img_canny,20,60);
    cv::cvtColor(img,img_gray,CV_RGB2GRAY);
    cv::namedWindow( "Original Image", CV_WINDOW_AUTOSIZE );
    cv::imshow( "Original Image", img );
    cv::namedWindow( "Canny Image", CV_WINDOW_AUTOSIZE );
    cv::imshow( "Canny Image", img_canny );
    cv::imshow( "Grayscale Image", img_gray );
    cv::waitKey(0);
    return 0;
}

