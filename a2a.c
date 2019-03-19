#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#include "timing.h"


// this type is used to pass arguments to threads
typedef struct{
    float * gMax;               // A pointer to the result.
    pthread_mutex_t *lockptr;   // A lock protecting gMax
    float * data;               // the data array
    
    int si; // start index      // first element of data[] to be examined by this thread
    int ei; // end index        // last element of data[] to be examined by this thread
} arguments;


// function prototypes
int findMax(float data[], int n, float *gMax);
int pFindMax(float data[], int n, float *gMax, int nthreads);
void *doWork(void *a);

// Find the maximum value in an array of n floats with random values.
// Usage:   ./a2a n nthreads 
// where    n is the length of the array of floats, initialized to random values, and
//          nthreads is the number of threads to use.
int main( int argc, char *argv[]){

    if(argc != 3){
    
      printf("Usage: ./a2a n nthreads\n");
      return EXIT_FAILURE;
    }

	int n = atoi(argv[1]);
	int nthreads = atoi(argv[2]);
	
	float *data = malloc(n * sizeof(float));
	
	if (data == NULL)
	    return EXIT_FAILURE;
	    
	for (int i=0; i<n; i++){
	
	    data[i] = (float) rand()/ (rand() + 1.0f);
	}
	
	float gMax;
	int status;
	
	double then = currentTime();
	if(nthreads < 2)
	    status=findMax(data,n,&gMax);
	else
	    status=pFindMax(data, n, &gMax, nthreads);
    double now = currentTime();
    
    if(status == EXIT_SUCCESS) {
        printf("gMax = %f\n", gMax);
        if(nthreads<2)
            printf("%%%%%% serialtime %lf milliseconds\n", 1000.0 * (now - then));
        else
            printf("%%%%%% paralleltime %lf milliseconds\n", 1000.0 * (now - then));
    } else {
        fprintf(stderr,"An error occurred.\n");
    }
    return status;
}


/** Finds the maximum value in the given array of n floats. This is the serial implementation. The
 *  result is written to *gMax.
 *  @param data the array of floats
 *  @param n the number of elements in the array
 *  @param gMax the address where the result will be written
 *  @nthreads the number of threads available for this function to use.
 *  @return an integer status code. 0 indicates success
 */
int findMax(float data[], int n, float *gMax){

    *gMax = data[0];
    
    for(int i=0; i<n; i++){
    
        *gMax = (data[i] > *gMax) ? data[i] : *gMax;
    }
    
    return EXIT_SUCCESS;
}

/** Finds the maximum value in the given array of n floats. This version uses the specified number of 
 *  threads running in parallel, dividing the array as evenly as possible amongst the threads. The
 *  result is written to *gMax.
 *  @param data the array of floats
 *  @param n the number of elements in the array
 *  @param gMax the address where the result will be written
 *  @nthreads the number of threads available for this function to use.
 *  @return an integer status code. 0 indicates success
 */
int pFindMax(float data[], int n, float *gMax, int nthreads){



	//TODO: Write me.
	// Declare an array of threads.
	// set *gMax to data[0]; 	
	// Declare and initialize a mutex that will be used to protect access to gMax
	// 
	// for i = 0 ... nthreads - 1
	//		malloc a new arguments struct
	//		set the arguments fields
	//		create thread i
	// 
	//	for i = 0 to nthreads - 1
	//		join thread i
	//
	//  destroy mutex
	// 

	pthread_t threads[nthreads];

	*gMax = data[0];

	pthread_mutex_t lock; 

	if (pthread_mutex_init(&lock, NULL) != 0)
	{
		printf("%s\n", "Mutex init has failed");
		return 1;
	}

	int indexperThread = ceil((double)n/nthreads);
	// printf("IndexPerThread:%d\n", indexperThread);

	int indexAssigned = 0;
	int start = 0;
	int threadUsed = 0;
	int rc;

	for (int i = 0; i < nthreads; i++) {
		if (indexAssigned < n){
			arguments *arg = malloc(sizeof(arguments));
			arg->data = data;
			arg->lockptr = &lock;
			arg->gMax = gMax;
			arg->si = start;
			arg->ei = start + indexperThread -1;

			indexAssigned += indexperThread;
			// printf("Start Id %d\n", arg->si);
			// printf("End Id %d\n", arg->ei);

			if (indexAssigned > n) {
				arg->ei = n - 1;
				indexAssigned = n;
			}

			start += indexperThread;
			threadUsed += 1;

			rc = pthread_create(threads+i, NULL, doWork, arg);

			if (rc){
				fprintf(stderr, "Error joining thread %d and return code is %d\n", i, rc);
				exit(-1);
			}

		}
		
	}

	for (int t = 0; t < threadUsed; t++) {
			pthread_join(threads[t], NULL);
	}
	// printf("Joining done\n");

	pthread_mutex_destroy(&lock);
    
    return EXIT_SUCCESS;
}

/** This thread function will find the maximum value in its portion of an array of floats, and 
 *  then update ( if appropriate) gMax -- a variable containing the maximum value seen in the 
 *  entire array so far. Because gMax is shared by multiple threads, it should only be updated
 *  under the protection of a mutex. All arguments are passed via the "arguments" type.
 *  @param a a pointer to an arguments struct.
 *  @returns NULL
 */ 
void *doWork(void *a){


	//TODO: The thread function
	// Unpack the arguments struct into local variables.
	// free the arguments struct
	// 
	// find the max value for a portion of the array,
	// lock the mutex
	// update gMax if appropriate
	// unlock the mutex

	// printf("In doWork\n");
    arguments* arg;
    arg = (arguments *) a;

    int si = arg->si;
    int ei = arg->ei;
    float *currMax = arg->gMax;
    float *arr = arg->data;

    pthread_mutex_t *lckptr = arg->lockptr;

    free(arg);



    pthread_mutex_lock(lckptr);

//    float *currMax = arg->gMax;
    // printf("Current max is %f\n", *currMax);
    for(int i=si; i<=ei; i++){

        *currMax = (arr[i] > *currMax) ? arr[i] : *currMax;
    }

    pthread_mutex_unlock(lckptr);


    return NULL;
}



