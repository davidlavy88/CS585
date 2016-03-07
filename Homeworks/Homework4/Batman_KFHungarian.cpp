#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "math.h"
#include <stdlib.h>
#include <float.h>
#include <limits>
#include <time.h>

//#include "Tracker.h"

using namespace cv;
using namespace std;

//---------Structures for our program---------//
struct centroid {
    Point coords;
    bool isTracked;
};

//--------Classes for our program----------//
class Hungarian
{
private:
    // Computes the optimal assignment (minimum overall costs) using Munkres algorithm.
    void assignmentoptimal(int *assignment, double *cost, double *distMatrix, int nOfRows, int nOfColumns);
    void buildassignmentvector(int *assignment, bool *starMatrix, int nOfRows, int nOfColumns);
    void computeassignmentcost(int *assignment, double *cost, double *distMatrix, int nOfRows);
    void step2a(int *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim);
    void step2b(int *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim);
    void step3 (int *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim);
    void step4 (int *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim, int row, int col);
    void step5 (int *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim);
public:
    double Solve(vector<vector<double> >& DistMatrix,vector<int>& Assignment);
};

class TKalmanFilter
{
public:
    KalmanFilter* kalman;
    double deltatime;
    Point2f LastResult;
    TKalmanFilter(Point2f p,float dt=0.2,float Accel_noise_mag=0.5);
    ~TKalmanFilter();
    Point2f GetPrediction();
    Point2f Update(Point2f p, bool DataCorrect);
};

class Track
{
public:
    vector<Point2d> trace;
    static size_t NextTrackID;
    size_t track_id;
    size_t skipped_frames;
    Point2d prediction;
    TKalmanFilter* KF;
    Track(Point2f p, float dt, float Accel_noise_mag);
};


class Tracker
{
public:

    // Step time for the filter
    float dt;

    float Accel_noise_mag;
    // The threshold distance. If the points are separated by a distance
    // greater than this threshold, then this pair is not considered the problem of assignments
    double dist_thres;
    // Maximum number of frames that the track saves to not receive data measurements
    int maximum_allowed_skipped_frames;
    // Maximum length of the track
    int max_trace_length;

    vector<Track*> tracks;
    void Update(vector<Point2d>& detections);
    //Initialize our tracker and other parameters
    Tracker(float _dt, float _Accel_noise_mag, double _dist_thres=60, int _maximum_allowed_skipped_frames=10,int _max_trace_length=10);
    ~Tracker(void);
};

//---------ADDITIONAL FUNCTIONS FOR OUR PROGRAM-----------//
void convertFileToMat(String filename, Mat& labelled, Mat& binary);
void getCentroid(String filename, vector<centroid>& centroids);

double MINIMUM_TRACKING_RADIUS = 20;

int main()
{
    int initial = 750;
    int final = 900;
    int iter = final - initial + 1;

    vector<centroid> centroids;

    Mat labelled, binary;
    Mat frame, frameOld;

    Tracker tracker(0.2,0.5,60.0,10,10);
    Scalar Colors[]={Scalar(255,0,0),Scalar(0,255,0),Scalar(0,0,255),Scalar(255,255,0),Scalar(0,255,255),Scalar(255,0,255),Scalar(255,127,255),Scalar(127,0,255),Scalar(127,0,127)};

    frame = Mat::zeros(Size(1024,1024),CV_8UC3);

    //VideoWriter vid("bat_original.avi",CV_FOURCC('D','I','V','X'),20,Size(1024,1024),true);

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

        cout << file_seg << " , " << file_loc << endl;

        convertFileToMat(file_seg, labelled, binary);
        Mat binary3C;
        cvtColor(binary,binary3C, CV_GRAY2BGR);
        getCentroid(file_loc,centroids);

        vector<Point2d> centers;
        for (int i=0;i<centroids.size();i++)
        {
            centers.push_back(centroids[i].coords);
        }

        if(centers.size()>0)
        {
            tracker.Update(centers);

            cout << tracker.tracks.size()  << endl;

            add(frame,binary3C,frame);

            for(int i=0;i<tracker.tracks.size();i++)
            {
                if(tracker.tracks[i]->trace.size()>1)
                {
                    for(int j=0;j<tracker.tracks[i]->trace.size()-1;j++)
                    {
                        line(frame,tracker.tracks[i]->trace[j],tracker.tracks[i]->trace[j+1],Colors[tracker.tracks[i]->track_id%9],2,CV_AA);
                    }
                }
            }
        }

        imshow("Video",frame);
        //vid.write(binary3C);
        frame = frame - binary3C;
        waitKey(20);
    }

    char key = waitKey(0);
    return 0;
}

void convertFileToMat(String filename, Mat& labelled, Mat& binary)
{
    //Read file
    ifstream infile(filename.c_str());
    vector <vector <int> > data;
    if (!infile){
        cout << "Error reading file!\t" << filename << "\n";
        return;
    }

    //Read the comma separated values into a vector of vector of ints
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

    //Construct the labelled matrix from the vector of vector of ints
    labelled = Mat::zeros(data.size(), data.at(0).size(), CV_8UC1);
    for (int i = 0; i < labelled.rows; ++i)
        for (int j = 0; j < labelled.cols; ++j)
            labelled.at<uchar>(i, j) = data.at(i).at(j);

    //Construct the binary matrix from the labelled matrix
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
        cout << "Error reading file for Centroid!";
        return;
    }

    //Read the comma separated values into a vector of vector of ints
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
            sint = atoi(srow.c_str()); //Convert string to int
            datarow.push_back(sint);
        }
        data.push_back(datarow);
    }

    centroid cent;
    centroids.clear();
    for (int i = 0; i < data.size(); ++i)
    {
        Point center(data.at(i).at(0), data.at(i).at(1));
        cent.coords = center;
        cent.isTracked = false;
        centroids.push_back(cent);
    }
}

//-------DEFINING OUR HUNGARIAN CLASS----------//
double Hungarian::Solve(vector<vector<double> >& DistMatrix,vector<int>& Assignment)
{
    int max_y = DistMatrix.size(); // number of columns (tracks)
    int max_x = DistMatrix[0].size(); // number of rows (measurements)

    int *assignment = new int[max_y];
    double *distIn	= new double[max_y*max_x];

    double  cost;
    // Fill matrix with random numbers
    for(int y=0; y<max_y; y++)
    {
        for(int x=0; x<max_x; x++)
        {
            distIn[y+max_y*x] = DistMatrix[y][x];
        }
    }
    assignmentoptimal(assignment, &cost, distIn, max_y, max_x);

    // form result
    Assignment.clear();
    for(int x=0; x<max_y; x++)
    {
        Assignment.push_back(assignment[x]);
    }

    delete[] assignment;
    delete[] distIn;
    return cost;
}
// --------------------------------------------------------------------------
// Computes the optimal assignment (minimum overall costs) using Munkres algorithm.
// --------------------------------------------------------------------------
void Hungarian::assignmentoptimal(int *assignment, double *cost, double *distMatrixIn, int nOfRows, int nOfColumns)
{
    double *distMatrix;
    double *distMatrixTemp;
    double *distMatrixEnd;
    double *columnEnd;
    double  value;
    double  minValue;

    bool *coveredColumns;
    bool *coveredRows;
    bool *starMatrix;
    bool *newStarMatrix;
    bool *primeMatrix;

    int nOfElements;
    int minDim;
    int row;
    int col;

    // Init
    *cost = 0;
    for(row=0; row<nOfRows; row++)
    {
        assignment[row] = -1.0;
    }

    // Generate distance matrix
    // and check matrix elements positiveness :)

    // Total elements number
    nOfElements   = nOfRows * nOfColumns;
    // Memory allocation
    distMatrix    = (double *)malloc(nOfElements * sizeof(double));
    // Pointer to last element
    distMatrixEnd = distMatrix + nOfElements;

    //
    for(row=0; row<nOfElements; row++)
    {
        value = distMatrixIn[row];
        if(value < 0)
        {
            cout << "All matrix elements have to be non-negative." << endl;
        }
        distMatrix[row] = value;
    }

    // Memory allocation
    coveredColumns = (bool *)calloc(nOfColumns,  sizeof(bool));
    coveredRows    = (bool *)calloc(nOfRows,     sizeof(bool));
    starMatrix     = (bool *)calloc(nOfElements, sizeof(bool));
    primeMatrix    = (bool *)calloc(nOfElements, sizeof(bool));
    newStarMatrix  = (bool *)calloc(nOfElements, sizeof(bool)); /* used in step4 */

    /* preliminary steps */
    if(nOfRows <= nOfColumns)
    {
        minDim = nOfRows;
        for(row=0; row<nOfRows; row++)
        {
            /* find the smallest element in the row */
            distMatrixTemp = distMatrix + row;
            minValue = *distMatrixTemp;
            distMatrixTemp += nOfRows;
            while(distMatrixTemp < distMatrixEnd)
            {
                value = *distMatrixTemp;
                if(value < minValue)
                {
                    minValue = value;
                }
                distMatrixTemp += nOfRows;
            }
            /* subtract the smallest element from each element of the row */
            distMatrixTemp = distMatrix + row;
            while(distMatrixTemp < distMatrixEnd)
            {
                *distMatrixTemp -= minValue;
                distMatrixTemp += nOfRows;
            }
        }
        /* Steps 1 and 2a */
        for(row=0; row<nOfRows; row++)
        {
            for(col=0; col<nOfColumns; col++)
            {
                if(distMatrix[row + nOfRows*col] == 0)
                {
                    if(!coveredColumns[col])
                    {
                        starMatrix[row + nOfRows*col] = true;
                        coveredColumns[col]           = true;
                        break;
                    }
                }
            }
        }
    }
    else /* if(nOfRows > nOfColumns) */
    {
        minDim = nOfColumns;
        for(col=0; col<nOfColumns; col++)
        {
            /* find the smallest element in the column */
            distMatrixTemp = distMatrix     + nOfRows*col;
            columnEnd      = distMatrixTemp + nOfRows;
            minValue = *distMatrixTemp++;
            while(distMatrixTemp < columnEnd)
            {
                value = *distMatrixTemp++;
                if(value < minValue)
                {
                    minValue = value;
                }
            }
            /* subtract the smallest element from each element of the column */
            distMatrixTemp = distMatrix + nOfRows*col;
            while(distMatrixTemp < columnEnd)
            {
                *distMatrixTemp++ -= minValue;
            }
        }
        /* Steps 1 and 2a */
        for(col=0; col<nOfColumns; col++)
        {
            for(row=0; row<nOfRows; row++)
            {
                if(distMatrix[row + nOfRows*col] == 0)
                {
                    if(!coveredRows[row])
                    {
                        starMatrix[row + nOfRows*col] = true;
                        coveredColumns[col]           = true;
                        coveredRows[row]              = true;
                        break;
                    }
                }
            }
        }

        for(row=0; row<nOfRows; row++)
        {
            coveredRows[row] = false;
        }
    }
    /* move to step 2b */
    step2b(assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim);
    /* compute cost and remove invalid assignments */
    computeassignmentcost(assignment, cost, distMatrixIn, nOfRows);
    /* free allocated memory */
    free(distMatrix);
    free(coveredColumns);
    free(coveredRows);
    free(starMatrix);
    free(primeMatrix);
    free(newStarMatrix);
    return;
}
// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
void Hungarian::buildassignmentvector(int *assignment, bool *starMatrix, int nOfRows, int nOfColumns)
{
    int row, col;
    for(row=0; row<nOfRows; row++)
    {
        for(col=0; col<nOfColumns; col++)
        {
            if(starMatrix[row + nOfRows*col])
            {
                assignment[row] = col;
                break;
            }
        }
    }
}
// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
void Hungarian::computeassignmentcost(int *assignment, double *cost, double *distMatrix, int nOfRows)
{
    int row, col;
    for(row=0; row<nOfRows; row++)
    {
        col = assignment[row];
        if(col >= 0)
        {
            *cost += distMatrix[row + nOfRows*col];
        }
    }
}

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
void Hungarian::step2a(int *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim)
{
    bool *starMatrixTemp, *columnEnd;
    int col;
    /* cover every column containing a starred zero */
    for(col=0; col<nOfColumns; col++)
    {
        starMatrixTemp = starMatrix     + nOfRows*col;
        columnEnd      = starMatrixTemp + nOfRows;
        while(starMatrixTemp < columnEnd)
        {
            if(*starMatrixTemp++)
            {
                coveredColumns[col] = true;
                break;
            }
        }
    }
    /* move to step 3 */
    step2b(assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim);
}

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
void Hungarian::step2b(int *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim)
{
    int col, nOfCoveredColumns;
    /* count covered columns */
    nOfCoveredColumns = 0;
    for(col=0; col<nOfColumns; col++)
    {
        if(coveredColumns[col])
        {
            nOfCoveredColumns++;
        }
    }
    if(nOfCoveredColumns == minDim)
    {
        /* algorithm finished */
        buildassignmentvector(assignment, starMatrix, nOfRows, nOfColumns);
    }
    else
    {
        /* move to step 3 */
        step3(assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim);
    }
}

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
void Hungarian::step3(int *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim)
{
    bool zerosFound;
    int row, col, starCol;
    zerosFound = true;
    while(zerosFound)
    {
        zerosFound = false;
        for(col=0; col<nOfColumns; col++)
        {
            if(!coveredColumns[col])
            {
                for(row=0; row<nOfRows; row++)
                {
                    if((!coveredRows[row]) && (distMatrix[row + nOfRows*col] == 0))
                    {
                        /* prime zero */
                        primeMatrix[row + nOfRows*col] = true;
                        /* find starred zero in current row */
                        for(starCol=0; starCol<nOfColumns; starCol++)
                            if(starMatrix[row + nOfRows*starCol])
                            {
                                break;
                            }
                            if(starCol == nOfColumns) /* no starred zero found */
                            {
                                /* move to step 4 */
                                step4(assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim, row, col);
                                return;
                            }
                            else
                            {
                                coveredRows[row]        = true;
                                coveredColumns[starCol] = false;
                                zerosFound              = true;
                                break;
                            }
                    }
                }
            }
        }
    }
    /* move to step 5 */
    step5(assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim);
}

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
void Hungarian::step4(int *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim, int row, int col)
{
    int n, starRow, starCol, primeRow, primeCol;
    int nOfElements = nOfRows*nOfColumns;
    /* generate temporary copy of starMatrix */
    for(n=0; n<nOfElements; n++)
    {
        newStarMatrix[n] = starMatrix[n];
    }
    /* star current zero */
    newStarMatrix[row + nOfRows*col] = true;
    /* find starred zero in current column */
    starCol = col;
    for(starRow=0; starRow<nOfRows; starRow++)
    {
        if(starMatrix[starRow + nOfRows*starCol])
        {
            break;
        }
    }
    while(starRow<nOfRows)
    {
        /* unstar the starred zero */
        newStarMatrix[starRow + nOfRows*starCol] = false;
        /* find primed zero in current row */
        primeRow = starRow;
        for(primeCol=0; primeCol<nOfColumns; primeCol++)
        {
            if(primeMatrix[primeRow + nOfRows*primeCol])
            {
                break;
            }
        }
        /* star the primed zero */
        newStarMatrix[primeRow + nOfRows*primeCol] = true;
        /* find starred zero in current column */
        starCol = primeCol;
        for(starRow=0; starRow<nOfRows; starRow++)
        {
            if(starMatrix[starRow + nOfRows*starCol])
            {
                break;
            }
        }
    }
    /* use temporary copy as new starMatrix */
    /* delete all primes, uncover all rows */
    for(n=0; n<nOfElements; n++)
    {
        primeMatrix[n] = false;
        starMatrix[n]  = newStarMatrix[n];
    }
    for(n=0; n<nOfRows; n++)
    {
        coveredRows[n] = false;
    }
    /* move to step 2a */
    step2a(assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim);
}

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
void Hungarian::step5(int *assignment, double *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, int nOfRows, int nOfColumns, int minDim)
{
    double h, value;
    int row, col;
    /* find smallest uncovered element h */
    h = DBL_MAX;
    for(row=0; row<nOfRows; row++)
    {
        if(!coveredRows[row])
        {
            for(col=0; col<nOfColumns; col++)
            {
                if(!coveredColumns[col])
                {
                    value = distMatrix[row + nOfRows*col];
                    if(value < h)
                    {
                        h = value;
                    }
                }
            }
        }
    }
    /* add h to each covered row */
    for(row=0; row<nOfRows; row++)
    {
        if(coveredRows[row])
        {
            for(col=0; col<nOfColumns; col++)
            {
                distMatrix[row + nOfRows*col] += h;
            }
        }
    }
    /* subtract h from each uncovered column */
    for(col=0; col<nOfColumns; col++)
    {
        if(!coveredColumns[col])
        {
            for(row=0; row<nOfRows; row++)
            {
                distMatrix[row + nOfRows*col] -= h;
            }
        }
    }
    /* move to step 3 */
    step3(assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim);
}

//-------DEFINING OUR KALMAN FILTER CLASS---------//
TKalmanFilter::TKalmanFilter(Point2f pt,float dt,float Accel_noise_mag)
{
    //time increment. Nothing special for a value, but has to be small
    deltatime = dt; //0.2

    // We don't know acceleration, so, assume it to process noise.

    //4 state variables [x y v_x v_y]', 2 measurements [x y]
    kalman = new KalmanFilter( 4, 2, 0 );
    //Transition matrix, reflects usual position velocities
    kalman->transitionMatrix = (Mat_<float>(4, 4) << 1,0,deltatime,0,   0,1,0,deltatime,  0,0,1,0,  0,0,0,1);

    //Initialization
    LastResult = pt;
    kalman->statePre.at<float>(0) = pt.x; // x
    kalman->statePre.at<float>(1) = pt.y; // y

    kalman->statePre.at<float>(2) = 0;
    kalman->statePre.at<float>(3) = 0;

    kalman->statePost.at<float>(0)=pt.x;
    kalman->statePost.at<float>(1)=pt.y;

    setIdentity(kalman->measurementMatrix);

    //Extracted from kalman process example (http://answers.opencv.org/question/73190/how-to-improve-kalman-filter-performance/)
    kalman->processNoiseCov=(Mat_<float>(4, 4) <<
        pow(deltatime,4.0)/4.0	,0						,pow(deltatime,3.0)/2.0		,0,
        0						,pow(deltatime,4.0)/4.0	,0							,pow(deltatime,3.0)/2.0,
        pow(deltatime,3.0)/2.0	,0						,pow(deltatime,2.0)			,0,
        0						,pow(deltatime,3.0)/2.0	,0							,pow(deltatime,2.0));


    kalman->processNoiseCov *= Accel_noise_mag;

    setIdentity(kalman->measurementNoiseCov, Scalar::all(0.1));

    setIdentity(kalman->errorCovPost, Scalar::all(.1));

}

TKalmanFilter::~TKalmanFilter()
{
    delete kalman; //Free memory
}

Point2f TKalmanFilter::GetPrediction()
{
    Mat prediction = kalman->predict();
    LastResult=Point2f(prediction.at<float>(0),prediction.at<float>(1));
    return LastResult;
}

Point2f TKalmanFilter::Update(Point2f p, bool DataCorrect)
{
    Mat measurement(2,1,CV_32FC1);
    if(!DataCorrect) //We don't have the measured position
    {
        measurement.at<float>(0) = LastResult.x;  //update using prediction
        measurement.at<float>(1) = LastResult.y;
    }
    else //We have measured position
    {
        measurement.at<float>(0) = p.x; //update using measurements
        measurement.at<float>(1) = p.y;
    }
    // Correction
        Mat estimated = kalman->correct(measurement);
        LastResult.x=estimated.at<float>(0);  //update using measurements
        LastResult.y=estimated.at<float>(1);
    return LastResult;
}


//-------DEFINING OUR TRACK CLASS--------//
size_t Track::NextTrackID=0;

//Our track constructor. Assign our initial point to begin the tracking
Track::Track(Point2f pt, float dt, float Accel_noise_mag)
{
    track_id=NextTrackID;

    NextTrackID++;
    // Every track have its own Kalman filter for next prediction
    KF = new TKalmanFilter(pt,dt,Accel_noise_mag);
    //Storing our point coordinates for our next prediction
    prediction=pt;
    skipped_frames=0;
}

//-------DEFINING OUR TRACKER CLASS--------//

//Tracker constructor
Tracker::Tracker(float _dt, float _Accel_noise_mag, double _dist_thres, int _maximum_allowed_skipped_frames,int _max_trace_length)
{
    dt=_dt;
    Accel_noise_mag=_Accel_noise_mag;
    dist_thres=_dist_thres;
    maximum_allowed_skipped_frames=_maximum_allowed_skipped_frames;
    max_trace_length=_max_trace_length;
}

void Tracker::Update(vector<Point2d>& detections)
{
    //If no tracking has happened yet, every point starts its own track
    //This usually happens at t=0
    if(tracks.size()==0)
    {
        // If no tracks yet
        for(int i=0;i<detections.size();i++)
        {
            Track* tr=new Track(detections[i],dt,Accel_noise_mag);
            tracks.push_back(tr);
        }
    }

    //If the tracking is already happening
    int N=tracks.size(); //Tracks
    int M=detections.size(); //How many detectors

    // Matrix of distances from the N-th track to the M-th detector
    vector< vector<double> > Cost(N,vector<double>(M));
    vector<int> assignment; // Destination

    // From the tracks we already have, create our cost matrix
    double dist;
    for(int i=0;i<tracks.size();i++)
    {
        // Point2d prediction=tracks[i]->prediction;
        // cout << prediction << endl;
        for(int j=0;j<detections.size();j++)
        {
            Point2d diff=(tracks[i]->prediction-detections[j]); //We substract our prediction with our detection
            dist=sqrtf(diff.x*diff.x+diff.y*diff.y); //Find the magnitude of the
            Cost[i][j]=dist;
        }
    }

    //Apply Hungarian Algorithm for the assignment problem
    //AssignmentProblemSolver APS;
    Hungarian hungarian;
    //APS.Solve(Cost,assignment,AssignmentProblemSolver::optimal);
    hungarian.Solve(Cost,assignment);


    //Clear up the assignments with pairs having long distance (>dist_thres)

    // Unassigned tracks
    vector<int> not_assigned_tracks;

    for(int i=0;i<assignment.size();i++)
    {
        if(assignment[i]!=-1)
        {
            if(Cost[i][assignment[i]]>dist_thres)
            {
                assignment[i]=-1;
                // Mark the unassigned tracks and increase the counter of dropped frames,
                // when the number of dropped frames exceed the threshold value, the track is erased
                not_assigned_tracks.push_back(i);
            }
        }
        else
        {
            // If the track is not assigned to a detector, then increment the skipped frames
            tracks[i]->skipped_frames++;
        }

    }

    //After certain frames, if the track doesn't get assigned to a detector, we remove it
    for(int i=0;i<tracks.size();i++)
    {
        if(tracks[i]->skipped_frames>maximum_allowed_skipped_frames)
        {
            delete tracks[i];
            tracks.erase(tracks.begin()+i);
            assignment.erase(assignment.begin()+i);
            i--;
        }
    }

    //Check for the unassigned detectors
    vector<int> not_assigned_detections;
    vector<int>::iterator it;
    for(int i=0;i<detections.size();i++)
    {
        it=find(assignment.begin(), assignment.end(), i);
        if(it==assignment.end())
        {
            not_assigned_detections.push_back(i);
        }
    }


    //These will be considered new tracks, so we initialize it
    if(not_assigned_detections.size()!=0)
    {
        for(int i=0;i<not_assigned_detections.size();i++)
        {
            Track* tr=new Track(detections[not_assigned_detections[i]],dt,Accel_noise_mag);
            tracks.push_back(tr);
        }
    }

    // Update status of the filters
    for(int i=0;i<assignment.size();i++)
    {

        //Start by calculating our prediction
        tracks[i]->KF->GetPrediction();

        if(assignment[i]!=-1) // If the assigment has the update on it
        {
            tracks[i]->skipped_frames=0;
            tracks[i]->prediction=tracks[i]->KF->Update(detections[assignment[i]],1);
        }else	// If not, we continue to predict
        {
            tracks[i]->prediction=tracks[i]->KF->Update(Point2f(0,0),0);
        }

        if(tracks[i]->trace.size()>max_trace_length)
        {
            tracks[i]->trace.erase(tracks[i]->trace.begin(),tracks[i]->trace.end()-max_trace_length);
        }

        tracks[i]->trace.push_back(tracks[i]->prediction);
        tracks[i]->KF->LastResult=tracks[i]->prediction;
    }

}

//Destructor to free up our memory
Tracker::~Tracker(void)
{
    for(int i=0;i<tracks.size();i++)
    {
        delete tracks[i];
    }
    tracks.clear();
}
