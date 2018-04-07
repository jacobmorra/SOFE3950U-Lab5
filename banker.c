/*
 * Banker's Algorithm for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015
 * All rights reserved.
 *
 * Group members: Jacob Morra, Vrund Shah, Kevin Apuyan
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "banker.h"


// Put any other macros or constants here using #define
// May be any values >= 0
#define NUM_CUSTOMERS 5
#define NUM_RESOURCES 3

// Mutex lock 
pthread_mutex_t mutex;

// Put global environment variables here
// Available amount of each resource
int available[NUM_RESOURCES];

// Maximum demand of each customer
int maximum[NUM_CUSTOMERS][NUM_RESOURCES];

// Amount currently allocated to each customer
int allocation[NUM_CUSTOMERS][NUM_RESOURCES];

// Remaining need of each customer
int need[NUM_CUSTOMERS][NUM_RESOURCES];


// Define functions declared in banker.h here
// Customer function to deal with resources

void *customer(void *NumOfCustomers){
    	
	// Initialize variables 
	int customer = *(int*)NumOfCustomers;
    	bool isAcquired = false;
	int request [3];
    
    	while(true){
		
		// Fill up requests array with a random number of resources 
        	for(int i = 0; i < NUM_RESOURCES; i++){
            	request[i] = rand() % (maximum[customer][i] + 1); 
        }
	
	// Acquire Mutex lock  for object
        pthread_mutex_lock(&mutex);
	
	// Request resources 
        isAcquired = request_res(customer,request);
	
	// Release Mutex lock for object 
        pthread_mutex_unlock(&mutex);

	// If statement to execute if resources are successfully requested and acquired
        if(isAcquired){

		// Reset variable to 0 in order to release the resources 
            	isAcquired = false;

		// Acquire mutex lock
            	pthread_mutex_lock(&mutex);

		// Release the resources which were acquired 
            	release_res(customer,request);

		// Unlock Mutex Lock for object 
            	pthread_mutex_unlock(&mutex);


            	if(rand() % 2 == 0){
                	printf("\nCustomer : %d done successfully\n", customer);

			// Since Customer is finished, reset their resourses to 0
                	for(int i = 0; i < NUM_RESOURCES; i++){
				maximum[customer][i] = 0;
                    		need[customer][i] = 0;
                }

		// Remaining needs of the customers remaining 
                printf("Needs still Remaining \n");
                for(int i = 0; i < NUM_CUSTOMERS; i++){
                    	for(int j = 0; j < NUM_RESOURCES; j++){
                        	printf("%3d",maximum[i][j]);
                     }
                    puts("");
                }
                break;
            }
        }
    }
    return 0;
}

//function determines whether or not resources can be acquired
bool getResources(){
	//declare count, indices, buffer to check for availability, and whether or not customer is done
	int n = 0, ii = -1, fi = -1, buffer[NUM_RESOURCES], isDone[NUM_CUSTOMERS], done = 1;

	//initialize isDone to array of 0's
    for(int i = 0; i < NUM_CUSTOMERS; i++){
        isDone[i] = 0;
    }
	//initialize buffer to available array
	//memcpy(buffer,available,sizeof(available));	
	for(int i = 0; i < NUM_RESOURCES; i++){
        buffer[i] = available[i];
    }

	//use isDone and buffer to determine whether or not resources can be acquired
    while (n < NUM_CUSTOMERS){
        ii = fi;
        for(int i = 0; i < NUM_CUSTOMERS; i++){
            if(isDone[i]==0){
                for(int j = 0; j < NUM_RESOURCES; j++){
                    if (need[i][j] > buffer[j])
                        done = 0;
                }
                if(done == 1){
                    fi = i;
                    for(int j = 0; j < NUM_RESOURCES; j++){
                        buffer[j] += allocation[i][j];
                    }
                    isDone[i] = 1;
					done = 1;                    
					n++;
                    break;
                }
            }
        }
		//if initial and final indices match, cannot acquire resources 
        if(ii == fi){
			return false;
		}
    }
	//otherwise, can acquire resources
    return true;
}

//function that requests and allocates the resource to a customer.
bool request_res(int n_customer, int request[])
{
	printf("Customer %d: \n", n_customer);
	for(int i= 0; i < NUM_RESOURCES; i++){
		printf("%d", request[i]);
	}
	
	printf("Prev Resources: \n");
    for(int i = 0; i < NUM_RESOURCES; i++){
        printf("%d ",available[i]);
    }
    puts("");
     for(int i = 0; i < NUM_RESOURCES; i++){
        if(request[i] <= need[n_customer][i]){
            if(request[i] > available[i]){
                printf("Error -> resource not available \n");
                return false;
            }
            else{
                //Testing resource acquisition
                for(int j = 0; j < NUM_RESOURCES; j++){
                    allocation[n_customer][j] += request[j];
                    available[j] -= request[j];
                    need[n_customer][j] -= request[j];
                }
                if(getResources()){
                    printf("Remaining resources: \n");
                    for(int i = 0; i < NUM_RESOURCES; i++){
                        printf("%d ",available[i]);
                    }
                    puts("\n");
                    return true;
                }
                else{
                    printf("Error -> resource not available \n");
                    for(int j = 0; j < NUM_RESOURCES; j++){
                        available[j] += request[j]; //add this to available resources
                        need[n_customer][j] += request[j]; //add this to customer's need array
                        allocation[n_customer][j] -= request[j]; //reducing resouce allocation for a customer 
                    }
                    return false;
                }
            }
        } else{
     
            return false;
        }

    }
 
return true;
}

// function that releases resources to customers requesting it, returns true if successful
bool release_res(int n_customer, int release[])
{
	for(int i = 0; i < NUM_RESOURCES; i++){
		printf("%d ", release[i]);
		available[i] += release[i];
		need[n_customer][i] += release[i];
		allocation[n_customer][i] -= release[i];
	}
	puts("");
	printf("Remaining resources:\n");
	for(int i = 0; i < NUM_RESOURCES; i++){
		printf("%d ", available[i]);
	}
	puts("");
	return true;
}



int main(int argc, char *argv[])
{
	//standard thread code
    pthread_t t[NUM_CUSTOMERS];
    pthread_mutex_init(&mutex, NULL);

	//specify number of available resources
	int i = 0;
	while (i < NUM_RESOURCES){
		available[i] = atoi(argv[i+1]);
		i++;	
	}
	//specify maximum demands of customers
	    printf("\n Max customer demands --------------------------------------- \n\n");
	for (int i =0; i < NUM_RESOURCES; i++){
        for(int j=0; j < NUM_CUSTOMERS; j++){
			allocation[j][i] = 0;
            maximum[j][i] = rand() % (available[i] + 1);
            need[j][i] = maximum[j][i];
			printf("%3d ",maximum[j][i]);
        }
		printf("\n");
    }
    srand(time(NULL));
		
	//create threads
    for(int i = 0; i < NUM_CUSTOMERS; i++){
        int *cid = malloc(sizeof(*cid));
        if(cid ==NULL){
            exit(1);
        }
        *cid = i;

        pthread_create(&t[i],NULL,&customer, cid);
    }
	//join threads
    for(int i = 0; i < NUM_CUSTOMERS; i++){
        pthread_join(t[i],0);
    }
    pthread_mutex_destroy(&mutex);

	//exit program
    return EXIT_SUCCESS;
}

