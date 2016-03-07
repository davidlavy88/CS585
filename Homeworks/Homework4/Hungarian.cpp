#include <stdlib.h>
#include <float.h>
#include <vector>
#include <iostream>
#include <limits>
#include <time.h>
#include "Hungarian.h"


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

/*
// --------------------------------------------------------------------------
// Usage example
// --------------------------------------------------------------------------
int main(void)
{
	// Matrix size
	int max_y=8; // tracks
	int max_x=9; // detects
	// Random numbers generator initialization
	srand (time(NULL));
	// Distance matrix max_y-th track to max_x-th detect.
	vector< vector<double> > Cost(max_y,vector<double>(max_x));
	// Fill matrix with random values
	for(int i=0; i<max_y; i++)
	{
		for(int j=0; j<max_x; j++)
		{
			Cost[i][j] = (double)(rand()%1000)/1000.0;
			std::cout << Cost[i][j] << "\t";
		}
		std::cout << std::endl;
	}
	Hungarian APS;
	vector<int> Assignment;
	
	cout << APS.Solve(Cost,Assignment) << endl;
	
	// Output the result
	for(int x=0; x<max_y; x++)
	{
		cout << x << ":" << Assignment[x] << "\t";
	}

	cout << "\n";
	//getchar();

	return 0;
}

// --------------------------------------------------------------------------
*/
