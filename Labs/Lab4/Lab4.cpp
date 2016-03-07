/**
CS585_Lab4.cpp

CS585 Image and Video Computing Fall 2014
Lab 4
--------------
This program introduces the following concepts:
a) Understanding and applying basic morphological operations on images
b) Finding and labeling blobs in a binary image
--------------
*/

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

/**
Function that detects blobs from a binary image
@param binaryImg The source binary image (binary image contains pixels labeled 0 or 1 (not 255))
@param blobs Vector to store all blobs, each of which is stored as a vector of 2D x-y coordinates
*/
void FindBinaryLargeObjects(const Mat &binaryImg, vector <vector<Point2i> > &blobs);

int main(int argc, char **argv)
{
	// read image as grayscale
	Mat img = imread("blob.png", 0);
	if (!img.data) {
		cout << "File not found" << std::endl;
		return -1;
	}

	// create windows
	namedWindow("Original");
	namedWindow("Morphologically Operated");
	namedWindow("Labelled");
	imshow("Original", img);
	// perform morphological operations to remove small objects (noise) and fill black spots inside objects
	// %TODO
	// Documentation on erode: http://docs.opencv.org/modules/imgproc/doc/filtering.html?highlight=erode#erode
	// Documentation on dilate: http://docs.opencv.org/modules/imgproc/doc/filtering.html?highlight=erode#dilate
    Mat element = getStructuringElement( MORPH_RECT,
                                           Size( 7, 7 ));
    erode(img,img,element);
    dilate(img,img,element);

	//convert thresholded image to binary image
	Mat binary;
    threshold( img, binary, 0, 255, THRESH_BINARY);

	//initialize vector to store all blobs
    vector <vector<Point2i> > blobs;

	//call function that detects blobs and modifies the input binary image to store blob info
	FindBinaryLargeObjects(binary, blobs);

	//display the output
	Mat output = Mat::zeros(img.size(), CV_8UC3);

    // UNCOMMENT AFTER COMPLETING FindBinaryLargeObjects FUNCTION
	// Randomly color the blobs
	for(size_t i=0; i < blobs.size(); i++) {
	unsigned char r = 255 * (rand()/(1.0 + RAND_MAX));
	unsigned char g = 255 * (rand()/(1.0 + RAND_MAX));
	unsigned char b = 255 * (rand()/(1.0 + RAND_MAX));

	for(size_t j=0; j < blobs[i].size(); j++) {
	int x = blobs[i][j].x;
	int y = blobs[i][j].y;

	output.at<Vec3b>(y,x)[0] = b;
	output.at<Vec3b>(y,x)[1] = g;
	output.at<Vec3b>(y,x)[2] = r;
	}
    }

	//show the binary image, as well as the labelled image
	imshow("Morphologically Operated", img);
    imshow("Labelled", output);
    //imshow("Labelled", binary);
	waitKey(0);

	return 0;
}

void FindBinaryLargeObjects(const Mat &binary, vector <vector<Point2i> > &blobs)
{
	//	%TODO
    blobs.clear();

        // Using labels from 2+ for each blob
        Mat label_image;
        //binary.convertTo(label_image, CV_32FC1);
        binary.convertTo(label_image, CV_8UC1);

        int label_count = 2; // starts at 2 because 0,1 are used already

        for(int y=0; y < binary.rows; y++) {
            for(int x=0; x < binary.cols; x++) {
                if((int)label_image.at<double>(y,x) != 255) {
                    continue;
                }

                Rect rect;
                floodFill(label_image, Point(x,y), Scalar(label_count), &rect, Scalar(0), Scalar(0), 4);

                vector<Point2i>  blob;

                for(int i=rect.y; i < (rect.y+rect.height); i++) {
                    for(int j=rect.x; j < (rect.x+rect.width); j++) {
                        if((int)label_image.at<double>(i,j) != label_count) {
                            continue;
                        }

                        blob.push_back(cv::Point(j,i));
                    }
                }


                blobs.push_back(blob);

                label_count++;
            }
        }

}

