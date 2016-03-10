#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>

/*
| Author: James Chapman-Brown
|
| welcome to my solution to the intenation collegiate programming competition 2015, problem A
|
|	https://icpc.baylor.edu/worldfinals/problems/icpc2015.pdf
|
| The program's goal is as follows: Find the largest decline in the price of artichokes given | by the formula price(k) = p * ( cos( a * k + b ) + sin( c * k + d ) + 2 )
|
| This program implements two solutions to the problem:
| - Brute force method using pre-calulated values
| - Derivitive evaluation to determine local max/minima
|
| Interestingly the derivitive approach is nearly always slower than the brute force approach
| likely the best solution would be to use the second derivitive to determine whether the 
| price is a maxima or minima, in order to reduce the order of the problem
|
| This program is distributed with a correct input and simple bash tester to verify 
| correctness. In order to use simply uncomment development macros and run test.sh
|
*/


// For maximum performance a million should be the highest possible number of zeros
// setting this lower takes less memory for smaller data sets, but costs performance in reallocs
#define START_NUM_POINTS 100000

// switch from method using the zeros of the derivitive to brute force
// Ironically faster than the 'smarter' method
//#define BRUTE_FORCE_TEST

// development macros. All must not be defined in order to use test script 

// define to enable time profiling.
//#define PROFILE_TIME

//Using this you can choose to use arguments or inputs
//#define USE_ARGS

// struct defining the input equation coefficients
typedef struct
{
	int p;
	int a;
	int b;
	int c;
	int d;
} coefficient_struct;

// structure containing the time and value of a point in the price
typedef struct
{
	double value;
	int time;
} point;

// holds local minima/maxima for further use
typedef struct
{
	// array of points
	point * points;
	// number of points
	int numPoints;
} pointHolder;


// Evaluate the derivitive using the given equation coefficients
double evalDeriv( int k, coefficient_struct equation );

// Evaluate the price using the given equation coefficients
double evalPrice( int k, coefficient_struct equation );

// Extend the points pointer in the pointHolder if neccessary depending on number of points
void zeroReallocator( pointHolder * toReallocate );

// Add a point to the pointHolder without checking if it's new or not.
void addPointRaw( pointHolder * toAddTo, int time , coefficient_struct equation );

// Add a point to the pointHolder, checking if it is already in the struct or not.
void addPoint( pointHolder * toAddTo, int time , coefficient_struct equation );

// Find the min max points using brute force method on the derivitive
pointHolder find_peaks( coefficient_struct equation , int n );

// Find the largest point using the derivitive method 
double findLargestDecline( int n, coefficient_struct equation );

// Find the largest point using a brute forrce method
double bruteForceFindLargestDecline( int n, coefficient_struct equation );


//main program function
int main ( int argc , char ** argv ){

	int n = 0;
	double decline = 0;
	coefficient_struct equation;

#ifdef USE_ARGS
	if ( argc != 7 )
	{
		printf("incorrect number of arguments\n");
		exit(1);
	}

	//input order is p , a , b , c , d , n
	// formula: price(k) = p * ( cos( a * k + b ) + sin( c * k + d ) + 2 )
	equation.p = atoi( argv[1] );
	equation.a = atoi( argv[2] );
	equation.b = atoi( argv[3] );
	equation.c = atoi( argv[4] );
	equation.d = atoi( argv[5] );
	n = atoi( argv[6] );

#else
	scanf( "%d" , &equation.p );
	scanf( "%d" , &equation.a );
	scanf( "%d" , &equation.b );
	scanf( "%d" , &equation.c );
	scanf( "%d" , &equation.d );
	scanf( "%d" , &n );
#endif



#ifdef PROFILE_TIME
	struct timeval timeStart,timeEnd,timeSpent;
	gettimeofday( &timeStart, NULL );
#endif


// Brute force algorithm for testing purposes.
#ifdef BRUTE_FORCE_TEST
	decline = bruteForceFindLargestDecline( n, equation ); 
#else
	decline = findLargestDecline( n, equation );
#endif
	printf( "%.6f\n" , decline );


#ifdef PROFILE_TIME
	gettimeofday( &timeEnd, NULL );
	timersub( &timeEnd , &timeStart , &timeSpent );

	printf( "took: %fs\n" , (double)timeSpent.tv_sec + (double)timeSpent.tv_usec/(double)1000000 );
	
#endif

	return 0;

}



// This will be usefull for finding local min/maxes. These are the set of points that
// could contain posible best places to sell or buy artichokes
// Using this you can either brute force the zeros or newton's method it
// the derivitive is  p * (  a * cos( a * k + b ) - c * sin( c * k + d ) )
double evalDeriv( int k, coefficient_struct equation )
{

	return ( equation.p * ( equation.a * cos( equation.a * k + equation.b ) ) + (-equation.c * sin( equation.c * k ) ) );

}

// Evaluate the pice of artichokes based on given equation
// price(k) = p * (sin( a * k +b ) + cos( c* k + d ) + 2)
double evalPrice( int k, coefficient_struct equation )
{

	return ( equation.p * ( sin( equation.a * k + equation.b ) + cos( equation.c * k + equation.d ) + 2 ) );

}

// check if there is enough space and realocate if neccessary
void zeroReallocator( pointHolder * toReallocate )
{
	point * newPoints;
	if ( toReallocate->numPoints % START_NUM_POINTS == 0 )
	{

		newPoints = realloc( toReallocate->points , ( toReallocate->numPoints + START_NUM_POINTS ) * sizeof( point ) );
		if ( !newPoints || errno == ENOMEM )
		{
			printf( "realloc failed!\n" );
			exit(0);
		}
		else
		{
			toReallocate->points = newPoints;
		}
	}
}

// adds points to the struct without checking for duplication
void addPointRaw( pointHolder * toAddTo, int time , coefficient_struct equation )
{
	// expand points as neccessary
	zeroReallocator( toAddTo );

	toAddTo->points[toAddTo->numPoints].value = evalPrice( time , equation );
	toAddTo->points[toAddTo->numPoints].time = time;
	toAddTo->numPoints = toAddTo->numPoints+1;
}


// Adds points to the pointHolder. Checks if the previous point is the same as the new point
// Evaluates the value of the point for future use
void addPoint( pointHolder * toAddTo, int time , coefficient_struct equation )
{
	if( toAddTo->numPoints == 0 )
	{
		addPointRaw( toAddTo, time , equation );
	}
	else
	{
		// only add non-duplicate members
		if ( toAddTo->points[toAddTo->numPoints-1].time != time )
		{
			addPointRaw( toAddTo, time , equation );
		}
	}
}

// Find the local min/maxs to optimize the solution. These are guaranteed to be buying/selling points
// If performance needs to be improved could use second derivitive to determine which is which
// to shorten search.
pointHolder find_peaks( coefficient_struct equation , int n )
{
	// These will be zeros of the derivitive
	pointHolder theZeros = {0};
	theZeros.points = malloc( sizeof( point ) * START_NUM_POINTS );

	// Problem is only to consider 1 ... n, not starting at 0
	int i = 1;
	double deriv = evalDeriv( i , equation );
	double lastValue;

	// when this switches from negative to positive we found a zero.
	// If we just looked for zero we would miss all zeros.
	int wasPositive = deriv > 0 ;

	// always incorporate the edges into min/max analyis
	addPoint( &theZeros , i , equation );
	
	// linear search for zeros of the derivitive
	// drop the first term, it's already included
	// can't drop the lalst term because it could make the term before it change the
	// sign of the derivitive and thus drag in the previous term.
	for ( i=2 ; i<(n+1); i++ )
	{
		lastValue = deriv;
		deriv = evalDeriv( i , equation );
		// ie the answer to the derivitive changed signs
		if ( wasPositive != ( deriv > 0 ) )
		{
			// found a maxima or minima
			// can't tell which point is actually the max/min so save both

			// take the left side first (so the list is in order)
			addPoint( &theZeros , i-1 , equation );
			// take the right side
			addPoint( &theZeros , i , equation );
			
		}
		wasPositive = deriv > 0;
	}

	// need to add the end bound
	addPoint( &theZeros , n , equation );

	return theZeros;

}

// Finds the largest price decline in the price of artichokes
// This function uses the min/max method
double findLargestDecline( int n, coefficient_struct equation )
{
	pointHolder zeros;

	double profit = 0;	
	int i = 0;
	int j = 0;

	zeros = find_peaks( equation , n );

	double prevMax = 0;

	double value = 0;
	for ( i=0; i<zeros.numPoints ; i++ )
	{
		value = zeros.points[i].value;
		// if this isn't the largest value
		if ( value > prevMax )
		{
			prevMax = value;
			for ( j = i; j<zeros.numPoints;j++ )
			{
				if ( ( value - zeros.points[j].value ) > profit )
				{
					profit = value - zeros.points[j].value;
				}
			}
		}
	}

	return profit; 
}

// Find the largest decline in artichoke prices using a brute force 
double bruteForceFindLargestDecline( int n, coefficient_struct equation )
{


	double profit = 0;
	double value = 0;
	double maxValue = 0;
	int i,j = 0;

	pointHolder thePoints = {0};

	// precalculate the price of each point saving recomputing the complicated trig function
	for ( i=1; i<(n+1); i++ )
	{
		addPoint( &thePoints , i , equation );
	}

	for ( i=0; i<thePoints.numPoints; i++ )
	{
		value = thePoints.points[i].value;
		if (value > maxValue)
		{
			maxValue = value;
			for ( j=i; j<n; j++ )
			{

				if ( ( value -  thePoints.points[j].value ) > profit )
				{
					profit = value - thePoints.points[j].value;
				}
			}
		}
	}
	return profit;
}
