/**
CS585_Assignment4.cpp
@author:
@version:

CS585 Image and Video Computing Fall 2014
Assignment 4: BATS
*/

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "math.h"

using namespace cv;
using namespace std;

void convertFileToMat(String filename, Mat& labelled, Mat& binary);
void drawObjectDetections(String filename, Mat& binary3channel, Scalar colorp);

struct tracked_bat {
    Point2d last;
    Point2d current;
    Point2d projected;
    bool tracked;
    Scalar color;
};

struct centroid {
    Point2d coords;
    bool isTracked;
};

void getCentroid(String filename, vector<centroid>& centroids);
void greedyTrack(vector<tracked_bat>& bats, vector<centroid> &centroids);
void drawTrajectory(vector<tracked_bat>& bats, Mat &canvas);
void MyLine( Mat &img, Point2d &start, Point2d &end );
void alpha_beta(tracked_bat *bat);

double MINIMUM_TRACKING_RADIUS = 20;

double a = .85;
double b = .005;
double v_x;
double v_x_1 = 0;
double projected_x_last = 0;
double projected_x;
double v_y;
double v_y_1 = 0;
double projected_y_last = 0;
double projected_y;

int main()
{
    int initial = 750;
    int final = 900;
    //int final = 755;
    int iter = final - initial + 1;

    vector<centroid> centroids;
    vector<tracked_bat> trackedBats;
    Scalar Colors[]={Scalar(255,0,0),Scalar(0,255,0),Scalar(0,0,255),Scalar(255,255,0),Scalar(0,255,255),Scalar(255,0,255),Scalar(255,127,255),Scalar(127,0,255),Scalar(127,0,127)};
    //vector<vector<centroid> > global_bat_centroids;
    //vector<Point> bat_one;

    Mat labelled, binary;
    Mat frame;
    Mat binary3C;

    //frame = Mat::zeros(Size(1024,1024),CV_8UC3);
    binary3C = Mat::zeros(Size(1024,1024),CV_8UC3);

    //VideoWriter vid("bat_alphabeta.avi",CV_FOURCC('D','I','V','X'),20,Size(1024,1024),true);

    //binary3C = Mat::zeros(Size(1024,1024),CV_8UC3);
    for (int i=0; i < iter; i++) {
        centroids.clear();
        string file_seg, file_loc;
        string ini_seg = "./Segmentation_Bats/CS585Bats-Segmentation_frame000000";
        string ini_loc = "./Localization_Bats/CS585Bats-Localization_frame000000";
        string fin = ".txt";
        string num1 = static_cast<ostringstream*>( &(ostringstream() << (initial+i)) )->str();
        if (num1.size()==1) num1 = "00" + num1;
        else if (num1.size()==2) num1 = "0" + num1;
        file_seg.append(ini_seg); file_seg.append(num1); file_seg.append(fin);
        file_loc.append(ini_loc); file_loc.append(num1); file_loc.append(fin);

        convertFileToMat(file_seg, labelled, binary);
        Mat blob;
        //cvtColor(binary,binary3C, CV_GRAY2BGR);
        cvtColor(binary,blob, CV_GRAY2BGR);
        //drawObjectDetections(filename_det1, binary3C, Scalar(255,0,0));
        getCentroid(file_loc,centroids);

        //project points with alpha-beta filter
        //preferentially let current tracked bats get centroids by greedy alg:
        for (int k=0; k<trackedBats.size(); k++)
        {
            alpha_beta(&trackedBats[k]);
        }
        greedyTrack(trackedBats, centroids);
        //add(frame,binary3C,frame);
        //for all remaining trackedbats, create new tracked bats
        int flag = 0;
        for (int j = 0; j < centroids.size(); j++){
            //loop through untracked centroids
            if (!centroids.at(j).isTracked){
                flag++;
                tracked_bat newBat;
                //naively set current, last, and projected all to be the centroid's points
                newBat.current = centroids.at(j).coords;
                newBat.last = centroids.at(j).coords;
                newBat.projected = centroids.at(j).coords;
                int blue = rand()%254;
                int green = rand()%254;
                int red = rand()%254;
                newBat.color = Colors[j%9];
                //cout << "New color is (" << blue << "," << green << "," << red << ")" << endl;
                trackedBats.push_back(newBat);
            }
        }

        //drawTrajectory(trackedBats,frame);
        drawTrajectory(trackedBats,binary3C);
        add(blob,binary3C,blob);
        //vid.write(blob);
        imshow("Trajectory",blob);
        //char keey = waitKey(0);
        waitKey(20);
        cout << "Number of new bats: " << flag << " in frame number " << i << endl;
    }
    //imshow("Trajectory",binary3C);

	char key = waitKey(0);
	return 0;
}

void convertFileToMat(String filename, Mat& labelled, Mat& binary)
{
	//read file
    ifstream infile(filename.c_str());
    vector <vector <int> > data;
	if (!infile){
		cout << "Error reading file!";
		return;
	}

	//read the comma separated values into a vector of vector of ints
	while (infile)
	{
		string s;
		if (!getline(infile, s)) break;

		istringstream ss(s);
		vector <int> datarow;

		while (ss)
		{
			string srow;
			int sint;
			if (!getline(ss, srow, ',')) break;
			sint = atoi(srow.c_str()); //convert string to int
			datarow.push_back(sint);
		}
		data.push_back(datarow);
	}

	//construct the labelled matrix from the vector of vector of ints
	labelled = Mat::zeros(data.size(), data.at(0).size(), CV_8UC1);
	for (int i = 0; i < labelled.rows; ++i)
		for (int j = 0; j < labelled.cols; ++j)
			labelled.at<uchar>(i, j) = data.at(i).at(j);

	//construct the binary matrix from the labelled matrix
	binary = Mat::zeros(labelled.rows, labelled.cols, CV_8UC1);
	for (int i = 0; i < labelled.rows; ++i)
		for (int j = 0; j < labelled.cols; ++j)
			binary.at<uchar>(i, j) = (labelled.at<uchar>(i, j) == 0) ? 0 : 255;

}

void getCentroid(String filename, vector<centroid>& centroids)
{
    ifstream infile(filename.c_str());
    vector <vector <int> > data;
    if (!infile){
        cout << "Error reading file!";
        return;
    }

    //read the comma separated values into a vector of vector of ints
    while (infile)
    {
        string s;
        if (!getline(infile, s)) break;

        istringstream ss(s);
        vector <int> datarow;

        while (ss)
        {
            string srow;
            int sint;
            if (!getline(ss, srow, ',')) break;
            sint = atoi(srow.c_str()); //convert string to int
            datarow.push_back(sint);
        }
        data.push_back(datarow);
    }

    centroid cent;
    centroids.clear();
    for (int i = 0; i < data.size(); ++i)
    {
        Point2d center(data.at(i).at(0), data.at(i).at(1));
        cent.coords = center;
        cent.isTracked = false;
        centroids.push_back(cent);
    }
}

void drawObjectDetections(String filename, Mat& binary3channel, Scalar colorp)
{
    ifstream infile(filename.c_str());
    vector <vector <int> > data;
	if (!infile){
		cout << "Error reading file!";
		return;
	}

	//read the comma separated values into a vector of vector of ints
	while (infile)
	{
		string s;
		if (!getline(infile, s)) break;

		istringstream ss(s);
		vector <int> datarow;

		while (ss)
		{
			string srow;
			int sint;
			if (!getline(ss, srow, ',')) break;
			sint = atoi(srow.c_str()); //convert string to int
			datarow.push_back(sint);
		}
		data.push_back(datarow);
	}

	//draw red circles on the image
	for (int i = 0; i < data.size(); ++i)
	{
        Point2d center(data.at(i).at(0), data.at(i).at(1));
        circle(binary3channel, center, 3, colorp, -1, 8);
	}
}

void drawTrajectory(vector<tracked_bat>& bats, Mat &canvas)
{
    int thickness = 2;
    int lineType = 8;
    for (int i=0; i<bats.size(); i++)
    {
        line( canvas, bats[i].last, bats[i].current, bats[i].color, thickness, lineType );
    }
}

double getDistance(int x1, int y1, int x2, int y2){
    return sqrt(pow(abs(x2 - x1), 2) + pow(abs(y2 - y1), 2));
}

void greedyTrack(vector<tracked_bat>& bats, vector<centroid>& centroids){
    int maxX = 1024;
    int maxY = 1024;

    for (int i = 0; i < bats.size(); i++){
        //cout << "processing a tracked_bat from x = " << bats.at(i).current.x << " y = " << bats.at(i).current.y << "projected x = " << bats.at(i).projected.x << " projected y = " << bats.at(i).projected.y << endl;
        int projX = bats.at(i).projected.x;
        int projY = bats.at(i).projected.y;
        double minDif = 2000000; //arbitrary large number (bigger than maximum distance in 1024x1024
        int centroidIndex = -1;
        //remove if the bat is projected outside the bounds.
        if (projX > maxX || projY > maxY || projX < 0 || projY < 0){
            bats.erase(bats.begin() + i);
        //cout << "erased OOB bat" << endl;
        }
        else {
            for (int j = 0; j < centroids.size(); j++){
                //loop through untracked centroids
                if (!centroids.at(j).isTracked){
                    double distance = getDistance(projX, projY, centroids.at(j).coords.x, centroids.at(j).coords.y);
                    //cout << "distance = " << distance << endl;
                    if (distance < minDif){
                            //keep track of smallest difference
                            minDif = distance;
                            centroidIndex = j;
                         //   cout << "centroid index set to " << centroidIndex << endl;
                        }
                    }
             }

            //make sure the distance is within the minimum tracking distance, if not retry with tracked centroids
            //this takes care of occlusion case


            if (minDif > MINIMUM_TRACKING_RADIUS){
                for (int j = 0; j < centroids.size(); j++){
                    //loop through tracked centroids
                    if (centroids.at(j).isTracked){
                        double distance = getDistance(projX, projY, centroids.at(j).coords.x, centroids.at(j).coords.y);
                        if (distance < minDif){
                            //keep track of smallest difference
                            minDif = distance;
                            centroidIndex = j;
                        }
                    }
                }
            }

            //NOTE - we could make sure we actually have something that improved here, but it isn't necessary

            //make sure we actually found a centroid (Note - centroidIndex defaults to -1)
            if (centroidIndex == -1){
                //if we ran out of centroids, delete this tracked bat. Note super accurate, but the greedy alg has spoken
                bats.erase(bats.begin() + i);
                cout << "centroid index = -1" << endl;
            }

            else{
                //assign the min centroid to the tracked bat
                //TODO - double check these assignments are correct and not some silly c this where it doesn't actually do anything
                bats.at(i).last = bats.at(i).current;
                bats.at(i).current = centroids.at(centroidIndex).coords;
                centroids.at(centroidIndex).isTracked = true;
                //cout << "marked centroid tracked" << endl;
                //cout << "Bat at x = " << bats.at(i).current.x << " y = " << bats.at(i).current.y << "projected x = " << bats.at(i).projected.x << " projected y = " << bats.at(i).projected.y
                     //<< "was set to to a centroid at x = " << centroids.at(centroidIndex).coords.x << "y = "  << centroids.at(centroidIndex).coords.y << " with distance = " << minDif << endl;

            }
        }
    }
}

void alpha_beta(tracked_bat *bat) {
    // projected distance moved
    projected_x = projected_x_last + v_x_1;
    v_x = v_x_1;
    projected_y = projected_y_last + v_y_1;
    v_y = v_y_1;

    // Update the residuals
    double r_x = bat->current.x - projected_x;
    double r_y = bat->current.y - projected_y;

    // Update projections
    projected_x += a * r_x;
    v_x += (b * r_x);
    projected_y += a * r_y;
    v_y += (b * r_y);

    // Set new last projections to current
    projected_x_last = projected_x;
    v_x_1 = v_x;
    projected_y_last = projected_y;
    v_y_1 = v_y;


    // Update x and y projections
    bat->projected.x = projected_x;
    bat->projected.y = projected_y;
}
