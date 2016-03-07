//opencv libraries
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//C++ standard libraries
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <locale>
#include <sstream>
#include <string>
#include <fstream>
#include <stack>

using namespace cv;
using namespace std;

Mat img1, img2, hsv1, hsv2;
Mat th1, color_th1;
vector<Mat> spl, spl2;
vector<Mat> vecValMean, vecValGaus;
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

//Stats=========================================
struct Stats {
    int frame_number;
    double area;
    double orientation;
    double circularity;
    int perimeter;
    double compactness;
};

int fr_number=0;
ofstream output_file2("statistics_aquarium.csv");
void getStats(Mat &src);
double getOrientation(vector<Point> &pts, Mat &img, double& circularity);
//=============================================


std::vector<color> colors;

labelled_pixel** label_pixels(Mat &src);
void color_regions(Mat& src, Mat& dst);
void filterC(Mat& src, Mat &dst);


int main(int argc, char** argv)
{
    //This parameters are ONLY for the aquarium images.
    int initial = 3;
    int final = 301;
    int iter = final - (initial + 30) + 1;
    //int iter = 5;
    //VideoWriter vid("aq_orig.avi",CV_FOURCC('D','I','V','X'),20,Size(800,600),true);

    for (int i=0; i < iter; i++) {
        string file1, file2;
        string ini = "./CS585-AquariumImages/aquarium_frame000000";
        string fin = ".ppm.png";
        string num1 = static_cast<ostringstream*>( &(ostringstream() << (initial+i)) )->str();
        string num2 = static_cast<ostringstream*>( &(ostringstream() << (initial+i+30)) )->str();
        string zeros;
        if (num1.size()==1) num1 = "00" + num1;
        else if (num1.size()==2) num1 = "0" + num1;;
        if (num2.size()==1) num2 = "00" + num2;
        if (num2.size()==2) num2 = "0" + num2;
        file1.append(ini); file1.append(num1); file1.append(fin);
        file2.append(ini); file2.append(num2); file2.append(fin);
        //cout << num1 << " " << num2 << endl;
        //cout << file1 << " " << file2 << endl;
        img1 = imread(file1,1);
        img2 = imread(file2,1);

        resize(img1,img1,Size(800,600));
        resize(img2,img2,Size(800,600));
        cvtColor(img1,hsv1, CV_BGR2HSV);
        cvtColor(img2,hsv2, CV_BGR2HSV);
        //imshow("input", image);

        split(hsv1,spl);
        split(hsv2,spl2);
        //imshow("Hue",spl[0]);//h
        //imshow("Saturation",spl[1]);//s
        //imshow("Value",spl[2]);//v

        absdiff(spl[0],spl2[0],out_h);
        absdiff(spl[1],spl2[1],out_s);
        absdiff(spl[2],spl2[2],out_v);

        adaptiveThreshold(out_h,aout_h_mean,max_thresh,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,13,-15);
        adaptiveThreshold(out_h,aout_h_gaus,max_thresh,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY,13,-15);
        adaptiveThreshold(out_v,aout_v_mean,max_thresh,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,13,-15);
        adaptiveThreshold(out_v,aout_v_gaus,max_thresh,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY,13,-15);

        //filterC(aout_v_mean,aout_v_mean);
        getStats(aout_v_mean);

        color_th1.create(aout_v_mean.rows,aout_v_mean.cols, CV_8UC3);
        color_regions(aout_v_mean,color_th1);
        //vid.write(img1);
        //vid.write(color_th1);

        imshow("Diff Hue",color_th1);
        /*imshow("Diff Hue",out_h);
        imshow("Diff Saturation",out_s);
        imshow("Diff Value",out_v);
        imshow("Adap. Diff Hue Mean",aout_h_mean);
        imshow("Adap. Diff Hue Gaus",aout_h_gaus);
        imshow("Adap. Diff Value Mean",aout_v_mean);
        imshow("Adap. Diff Value Gaus",aout_v_gaus);*/
        waitKey(30);
    }
    output_file2.close();

    /*Mat test = imread("./CS585-AquariumImages/aquarium_frame000000003.ppm.png",1);
    resize(test,test,Size(800,600));
    for (int counter=0;counter<200;counter++) {
        vid.write(test);
    }*/

    waitKey(0);
    return 0;
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
    erode(src,temp,Mat(),Point(-1,-1),1);
    dilate(temp,dst,Mat(),Point(-1,-1),3);
}

void getStats(Mat &src) {
    vector<vector<Point> > contour;
    vector<Vec4i> hierarchy;

    fr_number++;
    Mat copy_src = src.clone();
    findContours(src, contour, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    vector<Stats> stats;
    double circularity;
    for (int i=0; i < contour.size(); i++)
    {
        Stats statistics;
        //double area = contourArea(contour[i]);
        statistics.frame_number=fr_number;
        statistics.area = contourArea(contour[i]);
        //areas.push_back(area);
        //int perimeter = contour[i].size();
        statistics.perimeter = contour[i].size();
        //cout << "Area: " << area << endl;
        //cout << "Perimeter: " << perimeter << endl;
        double orient = getOrientation(contour[i], copy_src, circularity);
        statistics.circularity = circularity;
        statistics.orientation = orient;
        double cmp = (statistics.perimeter^2)/statistics.area;
        statistics.compactness = cmp;
        stats.push_back(statistics);
    }


    //ostream_iterator<double> output_iterator2(output_file2, "\t");
    for ( int i = 0 ; i < stats.size() ; i++ )
    {
        //copy(areas[i].at(i).begin(), newLetters[1].at(i).end(), output_iterator2);
        output_file2 << stats[i].frame_number << ", ";
        output_file2 << stats[i].area << ", ";
        output_file2 << stats[i].orientation << ", ";
        output_file2 << stats[i].circularity << ", ";
        output_file2 << stats[i].perimeter << ", ";
        output_file2 << stats[i].compactness;
        output_file2 << '\n';
    }
    //output_file2.close();
    imshow("PCA",copy_src);

}

double getOrientation(vector<Point> &pts, Mat &img, double &circularity)
{
    //Construct a buffer used by the pca analysis
    Mat data_pts = Mat(pts.size(), 2, CV_64FC1);
    for (int i = 0; i < data_pts.rows; ++i)
    {
        data_pts.at<double>(i, 0) = pts[i].x;
        data_pts.at<double>(i, 1) = pts[i].y;
    }

    //Perform PCA analysis
    PCA pca_analysis(data_pts, Mat(), CV_PCA_DATA_AS_ROW);

    //Store the position of the object
    Point pos = Point(pca_analysis.mean.at<double>(0, 0),
                      pca_analysis.mean.at<double>(0, 1));

    //Store the eigenvalues and eigenvectors
    vector<Point2d> eigen_vecs(2);
    vector<double> eigen_val(2);
    for (int i = 0; i < 2; ++i)
    {
        eigen_vecs[i] = Point2d(pca_analysis.eigenvectors.at<double>(i, 0),
                                pca_analysis.eigenvectors.at<double>(i, 1));

        eigen_val[i] = pca_analysis.eigenvalues.at<double>(0, i);
    }

    // Draw the principal components
    //circle(img, pos, 3, CV_RGB(255, 0, 255), 2);
    //line(img, pos, pos + 0.02 * Point(eigen_vecs[0].x * eigen_val[0], eigen_vecs[0].y * eigen_val[0]) , CV_RGB(255, 255, 0));
    //line(img, pos, pos + 0.02 * Point(eigen_vecs[1].x * eigen_val[1], eigen_vecs[1].y * eigen_val[1]) , CV_RGB(0, 255, 255));
    circularity = eigen_val[1]/eigen_val[0];

    return atan2(eigen_vecs[0].y, eigen_vecs[0].x);
}
