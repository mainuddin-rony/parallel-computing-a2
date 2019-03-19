#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mutices[2];

// function prototypes
void *thread_func0(void *vptr_args);
void *thread_func1(void *vptr_args);
int oneRound();


/** Runs the specified number of rounds of a two-thread dining philosophers problem. 
*  This implementation will never deadlock.
*  Usage: ./a2c <reps>  
*          where <reps> is the number of rounds to run
*/
int main( int argc, char *argv[]){

   if(argc != 2){
    
      printf("Usage: ./a2c reps\n");
      return 1;
    }

	int reps = atoi(argv[1]);
	
	for (int i=0; i<reps; i++){
	
		oneRound();

	}

    return EXIT_SUCCESS;
}

/** Runs one round of a two-thread dining philosophers problem. 
 *  This implementation will occasionally deadlock.
 */
int oneRound(){
    pthread_t thread[2];
    
    for (int i=0; i<2; i++){
        if (pthread_mutex_init(&(mutices[i]), NULL) != 0){
            return EXIT_FAILURE;
        }
    }
    
    
    if (pthread_create(&(thread[0]), NULL, thread_func0, NULL) != 0){
        return EXIT_FAILURE;
    }
 
    if (pthread_create(&(thread[1]), NULL, thread_func1, NULL) != 0){
        return EXIT_FAILURE;
    }

    for (int i=0; i<2; i++){
    
        if (pthread_join(thread[i], NULL) != 0){
            
            return EXIT_FAILURE;
        } 
    }
    
    for (int i=0; i<2; i++){
        pthread_mutex_destroy(&(mutices[i]));
    }
    
	printf("\n");    
    return EXIT_SUCCESS;
}


/** Thread function for philosopher 0. The parameter is ignored, 
 *  but the global mutices[] array is available.
 */
void *thread_func0(void *vptr_args){
	
	//TODO: Write Me
	// lock both mutices[0] and mutices[1]
	
	printf("Philosopher 0 is eating.\n");
	
    // unlock both mutices[0] and mutices[1]

    return NULL;
}

/** Thread function for philosopher 1. The parameter is ignored, 
 *  but the global mutices[] array is available.
 */
void *thread_func1(void *vptr_args){

	//TODO: Write Me
	// do the same thing you did for thread_func0(), but in a way
	// that cannot lead to deadlock.
	
	printf("Philosopher 1 is eating.\n");
	

    return NULL;
}



