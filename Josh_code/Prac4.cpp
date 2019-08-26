/*
 * Prac4.cpp
 * 
 * Originall written by Stefan Schröder and Dillion Heald
 * 
 * Adapted for EEE3096S 2019 by Keegan Crankshaw
 * 
 * This file is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "Prac4.h"
#include <pthread.h>
#include <sched.h>


using namespace std;

//priority stuff
pthread_attr_t tattr;
pthread_t tid;
int ret;
int newprio = 99;
sched_param param;


int chan = 0;
bool playing = true; // should be set false when paused
bool stopped = false; // If set to true, program should close
unsigned char buffer[2][BUFFER_SIZE][2];
int buffer_location = 0;
bool bufferReading = 0; //using this to switch between column 0 and 1 - the first column
bool threadReady = false; //using this to finish writing the first column at the start of the song, before the column is played




// Configure your interrupts here.
wiringPiISR(PLAY_BUTTON,INT_EDGE_RISING,&play_pause_isr);
wiringPiISR(STOP_BUTTON,INT_EDGE_RISING,&stop_isr);

// Don't forget to use debouncing.
long lastInterruptTime = 0;

void play_pause_isr(void){
    //Write your logis here
   long interruptTime = millis();
   if(playing == true){
	Playing = false;
   }else{
   	playing = true;
   }
   lastInterruptTime = interruptTime;
}

void stop_isr(void){
    // Write your logic here
 long interruptTime = millis();
       exit(0); 
        
   lastInterruptTime = interruptTime;
}

}

/*
 * Setup Function. Called once 
 */
int setup_gpio(void){
    //Set up wiring Pi
    wiringPiSetup();
    //setting up the buttons
    pinMode (PLAY_BUTTON, INPUT);
    pinMode (STOP_BUTTON,INPUT);
    pullUpDnControl(PLAY_BUTTON,PUD_DOWN);
    pullUpDnControl(STOP_BUTTON,PUD_DOWN);

    //setting upt the SPI interface
    wiringPiSPISetup(0,25 600 );
    
    {
   	 fprintf (stderr, "Unable to open SPI device 0: %s\n", strerror (errno)) ;
  	 exit (1) ;
    }
	
  // We want to use the first SPI channel
  int ce = 0;
  uint16_t writeCommand;

  // Enable writing
  writeCommand = 0b0111;
  wiringPiSPIDataRW (ce, writeCommand, 1);


	return 0;
}

/* 
 * Thread that handles writing to SPI
 * 
 * You must pause writing to SPI if not playing is true (the player is paused)
 * When calling the function to write to SPI, take note of the last argument.
 * You don't need to use the returned value from the wiring pi SPI function
 * You need to use the buffer_location variable to check when you need to switch buffers
 */
void *playThread(void *threadargs){
    // If the thread isn't ready, don't do anything
    while(!threadReady)
        continue;
    
    //You need to only be playing if the stopped flag is false
    while(!stopped){
        //Code to suspend playing if paused
        if(playing == false){
		continue;
	}
        //Write the buffer out to SPI
        for(buffer_location;buffer_location<1000,;i++){
		
		writeCommand = buffer(threadargs^1)(buffer_location);
		wiringPiSPIDataRW(ce, writeCommand, 1);
	}
        //Do some maths to check if you need to toggle buffers
        if(buffer_location == 999){
        	buffer_location = 0;
		bufferReading = bufferReading^1;
        }

	
    }
    
    pthread_exit(NULL);
}

int main(){
    // Call the setup GPIO function
	if(setup_gpio()==-1){
        return 0;
    }
    
    /* Initialize thread with parameters
     * Set the play thread to have a 99 priority
     * Read https://docs.oracle.com/cd/E19455-01/806-5257/attrib-16/index.html
     */ 
    
    //Write your logic here
    // more priority stuff
    // this should work 
    //maybe 
    /* initialized with default attributes */
    ret = pthread_attr_init (&tattr);

    /* safe to get existing scheduling param */
    ret = pthread_attr_getschedparam (&tattr, &param);

    /* set the priority; others are unchanged */
    param.sched_priority = newprio;

    /* setting the new scheduling param */
    ret = pthread_attr_setschedparam (&tattr, &param);

    /* with new priority specified */
    ret = pthread_create (&tid, &tattr,playThread,threadargs ); 



    /*
     * Read from the file, character by character
     * You need to perform two operations for each character read from the file
     * You will require bit shifting
     * 
     * buffer[bufferWriting][counter][0] needs to be set with the control bits
     * as well as the first few bits of audio
     * 
     * buffer[bufferWriting][counter][1] needs to be set with the last audio bits
     * 
     * Don't forget to check if you have pause set or not when writing to the buffer
     * 
     */
    FILE *filePointer;
    char ch;

    filePointer = fopen("sound_16k_8bit.raw", "r");

    int counter = 0;
    while ((ch = fgetc(filePointer)) != EOF && playing)
    {
      buffer[1][bufferlocation][0]=ch;


          counter++;
    }


    fclose(filePointer);

	
    // Open the file
     
    // Have a loop to read from the file
     
    // Close the file
     
    //Join and exit the playthread

    return 0;
}

