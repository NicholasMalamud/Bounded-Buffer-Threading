#include <iostream>
#include <stdlib.h> /*required for rand()*/
#include <pthread.h>
#include <semaphore.h> //semaphores
#include <unistd.h> //usleep
#include "buffer.h"

//declare mutex and semaphores
pthread_mutex_t lock;
sem_t full;
sem_t empty;

//initialize buffer and buffer variables
int count = 0;
int in = 0;
int out = 0;
buffer_item buffer[BUFFER_SIZE];

//print buffer contents
void display(){
  std::cout << "The current content of the buffer is [ ";
  
  for(int i = out; i < count + out; i++)
    std::cout << buffer[i%BUFFER_SIZE] << " ";
 
  std::cout << "]" << std::endl;
  std::cout << std::endl;
  
}

//buffer functions:

int insert_item(buffer_item item){
  //insert item into buffer
  //return 0 if successful, otherwise
  //return -1 indicating an error condition    
  while (count == BUFFER_SIZE) { }// busy waiting, do nothing
  /* add one item */ 
  if(count < BUFFER_SIZE){//if there is space in the buffer still
    buffer[in] = item;
    in = (in + 1) % BUFFER_SIZE;
    count++;
    std::cout << "item " << item << " inserted by a producer. "<< std::endl;
    return 0;
  }
  
  return -1; //error
}

int remove_item(buffer_item *item){
  //remove an object from buffer
  //placing it in item
  //return 0 if successful, otherwise
  //return -1 indicating an error condition
  while (count == 0) { } //busy waiting,do nothing
  /* consume the item in 
  nextConsumed */
  if(count > 0){//if there are items that can be removed from the buffer
    *item = buffer[out];
    out = (out + 1) % BUFFER_SIZE;
    count--;
    std::cout << "item " << *item << " removed by a consumer. " << std::endl;
    return 0;
  }
  
  return -1; //error
}

//thread functions:

void *producer(void *param){
  buffer_item item;

  while(true) { 
    
    /*sleep for a random period of time*/
    usleep(rand()%1000000);

    /*generate random number*/  
    item = rand()%100+1;
    
    sem_wait(&empty);
    pthread_mutex_lock(&lock);

    //critical section
    if(insert_item(item))
      printf("report error condition\n"); //took away f
    else
      display();
    //critical section

    pthread_mutex_unlock(&lock);
    sem_post(&full);
  }
  
  pthread_exit(NULL);
}

void *consumer(void *param){ //added *
  buffer_item item;

  while(true) {

    /*sleep for a random period of time*/
    usleep(rand()%1000000);
    
    sem_wait(&full);
    pthread_mutex_lock(&lock);

    //critical section
    if(remove_item(&item))
      printf("report error condition\n"); //took away f
    else
      display();
    //critical section
      
    pthread_mutex_unlock(&lock);
    sem_post(&empty); 
  }
  
  pthread_exit(NULL);
}


int main(int argc, char *argv[]){
  srand((unsigned int)time(NULL)); //random seed
  
  std::cout << "CS 433 Programming assignment 3" << std::endl;
	std::cout << "Author: Nicholas Malamud and Alexander Sadeghipour and Young Taek Oh and  Phillip Bagu" << std::endl;
	std::cout << "Date: 04/26/2022" << std::endl;
	std::cout << "Course: CS433 (Operating Systems)" << std::endl;
  std::cout << "Description : The Producer-Consumer Problem" << std::endl;
  std::cout << "=================================" << std::endl;

  //check for missing arguments
  if (argv[1] == NULL || argv[2] == NULL || argv[3] == NULL)
  {
    std::cout << "You have entered too few parameters to run the program.  You must enter three command-line arguments:" << std::endl;
    std::cout << " - amount of time to run the program (sec)" << std::endl;
    std::cout << " - # producer threads" << std::endl;
    std::cout << " - # consumer threads" << std::endl;
    return 0;
  }
  
  /*1. Get command line arguments argv[1], argv[2], argv[3]*/
  int sleepDuration = atoi(argv[1]);
  int numProducer = atoi(argv[2]);
  int numConsumer = atoi(argv[3]);
  
  
  /*2. Initialize buffer*/
  //mutex lock
  pthread_mutex_init(&lock, NULL);
  //semaphores:
  //full = 0
  sem_init(&full, 0, 0);
  //empty = N
  sem_init(&empty, 0, BUFFER_SIZE);
  
  /*3. Create producer thread(s)*/
  pthread_t produceThreads[numProducer];//array of threads that is the size of command line argument
  int i;
  int rc;
  for(i = 0; i < numProducer; i++){
    rc = pthread_create(&produceThreads[i], NULL, producer, NULL);
    if(rc){
      std::cout << "Unable to create thread," << rc << std::endl;
      exit(-1);
    }  
  }
      
  /*4. Create consumer thread(s)*/
  pthread_t consumeThreads[numConsumer];//array of threads that is the size of command line argument
  for(i = 0; i < numConsumer; i++){
    rc = pthread_create(&consumeThreads[i], NULL, consumer, NULL);
    if(rc){
      std::cout << "Unable to create thread," << rc << std::endl;
      exit(-1);
    }
  }
  
  /*5. Sleep*/
  usleep(sleepDuration*1000000);
  
  /*6. Exit*/
  exit(0);
  
  //return 0;
}

