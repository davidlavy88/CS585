#include "Kalman.h"
//#include "HungarianAlg.h"
#include "Hungarian.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
using namespace cv;
using namespace std;

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

