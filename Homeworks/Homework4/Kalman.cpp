#include "Kalman.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
using namespace cv;
using namespace std;

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

