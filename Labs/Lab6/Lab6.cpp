/**
CS585_Lab6.cpp

CS585 Image and Video Computing Fall 2014
Lab 6
--------------
This program introduces the following concepts:
a) Applying an alpha beta filter in a one-dimensional state space
b) Applying a Kalman filter in a one-dimensional state space
--------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;


// Global variables
//--------------------------------------------------------
// shared variables
float x_init = 1; //initial value of x
float z[10] = { 0.93, 1.44, 2.34, 4.01, 5.66, 6.01, 6.22, 7.31, 8.01, 8.22 }; //measurement values

// Alpha-beta filter variables
float delta_T = 1.0;
float v_init = 0;
float alpha = 0.85;
float beta = 0.005;

// Kalman filter variables
float A = 1;
float Q = 0.1;
float H = 1;
float R = 0.1;
float P_init = R;
//--------------------------------------------------------

/*TODO: Given the initialization variables and the measurement values, write an alphabeta filter
and a Kalman filter which processes the given measurements. Finally print out the outcome of both
of your filters alongside the measurements*/

//Function headers
/**
Applies the Alpha-Beta filter to an array of noisy measurements.

@param arr[] Array of floats to be filled with the state estimates after application of filter
*/
float* AlphaBetaFilter(float arr[]);

/**
Applies the Kalman filter to an array of noisy measurements.

@param arr[] Array of floats to be filled with the state estimates after application of filter
*/
float* KalmanFilter(float arr[]);

int main()
{
	//call AlphaBetaFilter
	float ab[10];
	AlphaBetaFilter(ab);

	//call KalmanFilter
	float k[10];
	KalmanFilter(k);

	//print out values
	printf("%s \t %s \t %s\n\n", "Measurements", "A-B estimates", "Kalman estimates");
	for (int i = 0; i < 10; i++) {
		printf("%f \t %f \t %f\n", z[i], ab[i], k[i]);
	}

	int x;
	cin >> x;
	return 0;
}

//AlphaBeta filter
float* AlphaBetaFilter(float arr[])
{
	float xk_1 = x_init, vk_1 = v_init;
	float xk, vk, rk;

	//AlphaBetaFilter
	for (int i = 0; i < 10; i++)
	{
		xk = xk_1 + (vk_1 * delta_T);
		vk = vk_1;

		rk = z[i] - xk; //residual

		xk += alpha * rk;
		vk += (beta * rk) / delta_T;

		xk_1 = xk;
		vk_1 = vk;

		arr[i] = xk;
	}
	return arr;
}

//Kalman filter
float* KalmanFilter(float arr[])
{
	float Pk_1 = P_init;
	float xk_1 = x_init;

	float xk, Pk, K;

	//Kalman Filter
	for (int i = 0; i < 10; i++)
	{
		//predict
		xk = A*xk_1;
		Pk = A*Pk_1*A + Q;

		//update
		K = Pk*H / (H*Pk*H + R);
		xk = xk + K*(z[i] - H*xk);
		Pk = (1 - K*H)*Pk;

		xk_1 = xk;
		Pk_1 = Pk;

		arr[i] = xk;
	}
	return arr;
}

