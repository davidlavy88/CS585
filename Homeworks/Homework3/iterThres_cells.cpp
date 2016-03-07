//opencv libraries
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//C++ standard libraries
#include <iostream>
#include <vector>
#include <typeinfo>
#include <stack>
#include <stdlib.h>

using namespace cv;
using namespace std;

Mat img, img1, img2, hsv1, hsv2;
Mat th1, color_th1;
vector<Mat> spl, spl2;
Mat out_v,out_h,out_s;
Mat aout_v_mean, aout_v_gaus,aout_h_mean,aout_h_gaus,aout_s_mean,aout_s_gaus;
int max_thresh = 255;

struct labelled_pixel {
    bool isForeground;
    int i;
    int j;
    int label;
};

struct color {
    int r;
    int g;
    int b;
};

std::vector<color> colors;

void meanHistValues(Mat &histog, Mat &binMatrix, Scalar &avg, Scalar &avg1, Scalar &avg2);
labelled_pixel** label_pixels(Mat &src);
void color_regions(Mat& src, Mat& dst);
string type2str(int type);
void filterC(Mat& src, Mat &dst);

int main(int argc, char** argv)
{
    //This parameters are ONLY for the aquarium images.
    int initial = 1;
    int final = 59;
    int iter = final - (initial + 30) + 1;
    //int iter = 1;
    //VideoWriter vid("CC_IterThres_Cells.avi",CV_FOURCC('D','I','V','X'),15,Size(1300,1030),true);

    for (int i=0; i < iter; i++) {
        string file1, file2;
        string ini = "./CS585-CellImages/Normalized/0";
        string fin = ".jpg";
        string num1 = static_cast<ostringstream*>( &(ostringstream() << (initial+i)) )->str();
        string num2 = static_cast<ostringstream*>( &(ostringstream() << (initial+i+30)) )->str();
        string zeros;
        if (num1.size()==1) num1 = "00" + num1;
        else if (num1.size()==2) num1 = "0" + num1;
        if (num2.size()==1) num2 = "00" + num2;
        if (num2.size()==2) num2 = "0" + num2;
        file1.append(ini); file1.append(num1); file1.append(fin);
        file2.append(ini); file2.append(num2); file2.append(fin);
        //cout << num1 << " " << num2 << endl;
        //cout << file1 << " " << file2 << endl;
        img = imread(file1,1);
        cvtColor(img,img1, CV_BGR2GRAY);
        blur(img1, img1, Size(3,3));
        //imshow("input", img1);
        //waitKey(0);

        /// Establish the number of bins
        int histSize = 256;

        /// Set the ranges ( for B,G,R) )
        float range[] = { 0, 256 } ;
        const float* histRange = { range };

        bool uniform = true; bool accumulate = false;

        Mat hist_v;

        calcHist(&img1, 1, 0, Mat(), hist_v, 1, &histSize, &histRange, uniform, accumulate);

        Mat binsMat = Mat::zeros(hist_v.rows, hist_v.cols, hist_v.type());

        //cout << "Type: " << type2str(hist_v.type()) << endl;

        for (int i=0; i<256; i++) binsMat.at<float>(i,0) = (float) i;

        Scalar oldT = mean(img1);
        Scalar mean_1, mean_2;

        meanHistValues( hist_v, binsMat, oldT, mean_1, mean_2);
        //cout << "Mean 1= " << mean_1[0] << " , Mean 2= " << mean_2[0] << endl;

        Scalar newT((mean_1[0]+mean_2[0])*0.5);
        Scalar tempT;

        cout << abs(oldT[0]-newT[0]) << endl;
        //while ( abs(oldT[0]-newT[0]) > 0.5 ) {
        for (;;) {
            oldT.val[0] = newT.val[0];
            cout << "Old T = " << oldT[0] << endl;
            meanHistValues( hist_v, binsMat, oldT, mean_1, mean_2);
            cout << "Mean 1= " << mean_1[0] << " , Mean 2= " << mean_2[0] << endl;
            newT.val[0] = (mean_1[0]+mean_2[0])*0.5;
            if (abs(oldT[0]-newT[0]) < 0.5) break;
            else continue;
        }

        int thresh = (int)newT[0];

        cout << "Final threshold = " << thresh << endl;
        //cout << "First T: " << T[0] << endl; //<< " with type: " << typeid(T.val[0]).name() << endl;

        double thr = threshold(img1, th1, thresh, max_thresh, THRESH_BINARY);

        //Zoom in a region of the picture
        //sub_th1 = th1(Rect(0, 299, 400, 300)).clone();

        //resize(sub_th1, sub_th1, Size(800,600));
        //imshow("Subimage",sub_th1);
        //waitKey(0);
        //filterC(sub_th1,sub_th1);
        color_th1.create(th1.rows,th1.cols, CV_8UC3);
        color_regions(th1,color_th1);

        imshow("Connected Components",color_th1);
        //imshow("Iterative Threshold", th1);
        //vid.write(color_th1);
        waitKey(30);
    }

    //waitKey(0);
    return 0;
}

void meanHistValues(Mat &histog, Mat &binMatrix, Scalar &avg, Scalar &avg1, Scalar &avg2)
{
    int T = (int) avg[0];
    //int T = 11;
    Mat R1, R2, bR1, bR2;
    Mat mult1, mult2;
    Scalar sum1, sum2;

    R1 = histog(Rect(0, 0, 1, T)).clone();
    R2 = histog(Rect(0, T, 1, 256-T )).clone();

    bR1 = binMatrix(Rect(0, 0, 1, T)).clone();
    bR2 = binMatrix(Rect(0, T, 1, 256-T )).clone();

    multiply(R1,bR1,mult1);
    multiply(R2,bR2,mult2);
    Scalar msum1 = sum(mult1);
    Scalar msum2 = sum(mult2);

    sum1 = sum(R1);
    sum2 = sum(R2);

    double mu1 = msum1[0]/sum1[0];
    double mu2 = msum2[0]/sum2[0];

    avg1.val[0] = mu1;
    avg2.val[0] = mu2;

}

string type2str(int type) {
  string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }

  r += "C";
  r += (chans+'0');

  return r;
}

//returns a 2d array of labelled_pixels
labelled_pixel** label_pixels(Mat &src){
    cout << "label_pixels called";
    //create a 2d array of labelled_pixels
    labelled_pixel** arr = new labelled_pixel*[src.rows];
    for (int i = 0; i < src.rows; i++){
        arr[i] = new labelled_pixel[src.cols];
    }
    int numWhite = 0;
    //translate each Vec3b into a labelled_pixel
    for (int i = 0; i < src.rows; i++){
        for (int j = 0; j < src.cols; j++){
            uchar current = src.at<uchar>(i, j);
            arr[i][j].i = i;
            arr[i][j].j = j;
            arr[i][j].label = -1;
            if (current == 0){
                arr[i][j].isForeground = false;
            }
            else{
                arr[i][j].isForeground = true;
                numWhite++;
            }
        }
    }
    cout << "white pixels :" << numWhite;
    //set up for labelling
    int label = 0;
    stack<labelled_pixel> s;
    labelled_pixel seed;
    for (int i = 0; i < src.rows; i++){
        for (int j = 0; j < src.cols; j++){
            seed = arr[i][j];
            //push a pixel if its unlabelled and foreground
            if (seed.isForeground && seed.label == -1){
            //	cout << "pushing a seed pixel at (" << i << ", " << j << "\n";
                s.push(seed);
                while (!s.empty()){
                    //if unlabelled, label
                    labelled_pixel pix = s.top();
                    s.pop();

                    if (arr[pix.i][pix.j].label == -1){
                        arr[pix.i][pix.j].label = label;
                        //pix.label = label;

                        //cout << "pixel at (" << pix.i << ", " << pix.j << ") set to " << label << "\n";
                        //push foreground neighbors
                        //NOTE - n4 definition of neighbor used
                        if (pix.i > 0){
                            labelled_pixel neighbor = arr[pix.i - 1][pix.j];
                            if (neighbor.isForeground){
                                //	cout << "pushing pixel at(" << pix.i - 1 << ", " << pix.j << "\n";
                                    s.push(neighbor);
                            }
                        }
                        if (pix.i < src.rows - 1){
                            labelled_pixel neighbor = arr[pix.i + 1][pix.j];
                            if (neighbor.isForeground){
                                //	cout << "pushing pixel at(" << pix.i + 1 << ", " << pix.j << "\n";
                                    s.push(neighbor);

                            }
                        }
                        if (pix.j > 0){
                            labelled_pixel neighbor = arr[pix.i][pix.j - 1];
                            if (neighbor.isForeground){
                                //	cout << "pushing pixel at(" << pix.i << ", " << pix.j - 1 << "\n";
                                    s.push(neighbor);

                            }
                        }
                        if (pix.j < src.cols - 1){
                            labelled_pixel neighbor = arr[pix.i][pix.j + 1];
                            if (neighbor.isForeground){
                            //		cout << "pushing pixel at(" << pix.i << ", " << pix.j + 1 << "\n";
                                    s.push(neighbor);

                            }
                        }
                    }
                }
                //all connected components are now labelled, increment label
                label++;
        //		cout << "label incremented to " << label;
            }
        }
    }

        //return array
    cout << "\n\n" << label << " regions labelled";
            return arr;
        }


//colors independent regions of a pixel randomly
void color_regions(Mat& src, Mat& dst){
    labelled_pixel** arr = label_pixels(src);
    color c;
    for (int i = 0; i < dst.rows; i++){
        for (int j = 0; j < dst.cols; j++){
            if (arr[i][j].isForeground){
                //if color is outside of colors, add a new color;
                if (arr[i][j].label >= colors.size()){
                    c.r = rand() % 254;
                    c.g = rand() % 254;
                    c.b = rand() % 254;
                    colors.insert(colors.end(), c);
                }

                dst.at<Vec3b>(i, j)[0] =  colors.at(arr[i][j].label).b; //b
                dst.at<Vec3b>(i, j)[1] = colors.at(arr[i][j].label).g; //g
                dst.at<Vec3b>(i, j)[2] = colors.at(arr[i][j].label).r; //r

                //get label, get color
            }
            else {
                dst.at<Vec3b>(i, j)[0] = 0; //b
                dst.at<Vec3b>(i, j)[1] = 0; //g
                dst.at<Vec3b>(i, j)[2] = 0; //r
            }
        }
    }
}
\
void filterC(Mat& src, Mat &dst){
    Mat temp;
    erode(src,temp,Mat(),Point(-1,-1),2);
    dilate(temp,dst,Mat());
}
