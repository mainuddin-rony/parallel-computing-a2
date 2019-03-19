#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <strings.h>


// two choices for sequenceType. 1,2,3,.. or random
#define NATURAL 1
#define RANDOM  2

// A struct holding the value and sum for a "state array" element, 
// including an associated mutex and cv.
typedef struct{

	pthread_cond_t  cv;
	pthread_mutex_t lock;
	
	int value;
	int sum;
} state;

// A struct for passing arguments to a thread function.
typedef struct{

    int state_arr_len;// number of elements in state array 
    int s_index; // this thread's "home" index in the state array
    state * state_arr; // the array of "state" elements 
} thread_function_args;


// Functon prototypes.
int prefix_sum(int nthreads, int sequenceType);
void *doWork(void *a);
void init(state * state_arr, int state_arr_len, int sequenceType);

 
 
 /**    Solves the prefix_sum problem for a specified number of elements and sequenceType.
  *     Usage: ./a2d <num_elements> [random]
  *         if "random" is specified, the array will be initialized with random values. 
  *             otherwise, the natural numbers will be used, beginning with 1.
  *
  *     The prefix sum of a sequence a0, a1, a2 ... ak is simply a0, a0+a1, a0+a1+a2, ... 
  *  For example:
  * % ./a2d 10 
  *   1     2     3     4     5     6     7     8     9    10 
  *   1     3     6    10    15    21    28    36    45    55
  */
int main( int argc, char *argv[]){

    if(argc < 2 || argc > 3){
    
      printf("Usage: ./a2d <num_elements> [random]\n");
      return EXIT_FAILURE;
    } 

	int nthreads = atoi(argv[1]) - 1;
	
	int sequenceType = NATURAL;
	
	if(argc == 3){
		if(strcasecmp(argv[2], "random") == 0){
			sequenceType = RANDOM;
		}
	}
		
	prefix_sum(nthreads, sequenceType);
}


/** Initialize elements of the state array. For each element, the CV and mutex will be initialized,
 *  while the sum will be set to 0. The value field will be set according to the sequenceType:
 *  NATURAL will use the sequence 1,2,3.., while RANDOM will use random numbers between 1 and 10
 *  inclusive.
 *  @param state_arr the state array
 *  @param state_arr_len number of elements in the state array
 *  @param sequenceType  either RANDOM or NATURAL
 */
 void init(state * state_arr, int state_arr_len, int sequenceType){

    for(int i=0; i < state_arr_len; i++){
    
        pthread_cond_init(&(state_arr[i].cv), NULL);
        pthread_mutex_init(&(state_arr[i].lock), NULL);
        state_arr[i].sum = 0;
        
        switch(sequenceType){
        
        	case RANDOM:
        		state_arr[i].value = 1 + (rand() % 10) ; // will be > 0
        		break;
        	case NATURAL:
        		state_arr[i].value = i + 1; // will be > 0
        		break;
        	
        	default:
        		printf("init(): Bad sequenceType.");
        		exit(EXIT_FAILURE);
        }
    }
}


/**	Solves the prefix_sum problem using the specified number of threads. Threads map to
 *  elements i=[1..nthreads] of the state array, and each wait on the cv in the element to
 *	their left: state_arr[i-1].cv. For example, thread 6 will cond_wait on state_arr[5].cv. 
 *	The CV state_arr[i-1].cv is associated with the condition "state_arr[i-1].sum != 0", 
 *  so if a thread i wakes and sees that the condition is true, it will do two things. 
 *  First, it will set state_arr[i].sum = state_arr[i-1].sum + state_arr[i].value. 
 *	Second, it will signal on state_arr[i].cv. These two things will make the condition for
 *  thread i+1 true, and also wake thread i+1. Thread i+1 will then update its own sum, and
 *  wake thread i+2, and so on. The dominos fall one by one.
 *
 *  What pushes the first domino? When it's ready, the main thread can set 
 *  state_arr[0].sum = state_arr[0].value and signal on state_arr[0].cv. That triggers
 *  the whole process.
 */
int prefix_sum(int nthreads, int sequenceType){

    int state_arr_len = nthreads+1;
    
    state * state_arr = malloc(state_arr_len * sizeof(state));
    
    // I put the thread array on the heap instead of the stack to
    // avoid subtle stack corruption issues with buggy code.
    pthread_t * thread_arr = malloc(nthreads * sizeof(pthread_t)); 
    
        
    init(state_arr, state_arr_len, sequenceType);

    // no load balancing needed
    for(int i=0; i < nthreads; i++){
    
        thread_function_args * args = malloc(sizeof(thread_function_args));
                      
        args->s_index = i + 1; // this thread's "home" location in the
        					   // state array.  Element 0 will be the "trigger".
        
        args->state_arr = state_arr;
        args->state_arr_len = state_arr_len;
    
        pthread_create(&(thread_arr[i]), NULL, doWork, args);
    }
 
    //TODO: Write me.
    // Pull the trigger. Use the mutex, CV, sum, and value fields
    // of state_arr[0] to start the domino effect, as described in the
    // comments at the top of this function.

    
    
    for (int i=0; i<nthreads; i++){
    
        if (pthread_join(thread_arr[i], NULL) != 0){
            
            return EXIT_FAILURE;
        } 
    }

    for(int i=0; i < state_arr_len; i++){
    	   
        pthread_cond_destroy(&(state_arr[i].cv));
        pthread_mutex_destroy(&(state_arr[i].lock));
    }

    for(int i=0; i < state_arr_len; i++){
    	
    	printf("%5d ", state_arr[i].value);    
    }
	printf("\n");

    
    for(int i=0; i < state_arr_len; i++){
    	
    	printf("%5d ", state_arr[i].sum);    
    }
    printf("\n");
       
    free(state_arr);
    free(thread_arr);   
    return EXIT_SUCCESS;
}


/** The thread function. The parameter is really a pointer to a thread_function_args struct,
 *  which contains a pointer to the state array, the length of the state array, and the
 *  "home index" of this thread in the state array.
 *
 *  Read the comment for prefix_sum() if you haven't already.
 *
 *  After unpacking the thread_function_args struct, the thread waits on the cv to its 
 *  left. Upon waking, it checks that the sum to its left is non-zero. If so, it updates
 *  its own sum, and signals its cv, which wakes the thread to the right.
 *
 *  Remember to use the mutexes properly when changing variables, and also use a while
 *  loop with cond_wait().
 */
void *doWork(void *a){

	//TODO: Write me.
	// The comments immediately above, and in the header for prefix_sum() should 
	// describe what to do.  

 	
    return NULL;
}



