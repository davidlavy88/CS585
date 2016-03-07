#include "Tracker.h"

using namespace cv;
using namespace std;

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
