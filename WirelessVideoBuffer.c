/* ============================================================================
 * Author: Brian Bowden
 * Date: November 12, 2018
 * Due: November 13, 2018
 * Class: CPSC 441 || Tut 04
 * ============================================================================
 * WirelessVideoBuffer.c
 * executable: ./WVB
 * ============================================================================
 * Simulation of packets being buffered and dropped across an AP link 
 * 
 * Opens a file containing data, formatted:
 *  arrival time(in seconds) <space> packet size (in bytes)
 * 
 * checks current time via timestamps in file
 * is the current packet finished processing?
 *      yes:
 *          unbuffer processed packet, start processing next buffered packet
 *      no:
 *          buffer packet if there is space in the buffer, otherwise drop packet
 * 
 * ============================================================================
 * Bugs: No known bugs
 * 
 * Testing: Testing done on my laptop
 *          Testing only done with ONE file, so it is HARDCODED in the associated .h file
 * 
 * Notes: The average delay is an ESTIMATION based on the total number of packets
 *          processed (dropped packets are not considered), and the average 
 *          processing time multiplied by an assumed "full buffer" of these packets
 * 
 * ============================================================================
 * */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "WirelessVideoBuffer.h"

int main(int argc, char *argv[]){

    FILE *fp;

    /* MAIN VARIABLES */
    double arrival = START; // current time
    double departure = START; // expected finish time
    double service = START; // time it takes to process package
    double start = START; // time to start processing packet
    int packet_size; // size of packet in bytes
    long long total_size = 0; // size of all packets combined
    double avg_pack = 0.0; // average service time of average packet

    /* COUNTERS */
    int total_packets = 0; // all packets (processed + dropped)
    int dropped_packets = 0; // all dropped packets
    int buffered_packets = 0; // all packets in buffer
    int first_in = 0; // first packet in queue
    int last_in = 0; // last arrived packet in queue, never used
    int available_space = 0; // first available position in buffer
    
    int AP_buffer[BUFFER_SIZE]; // AP buffer
    char path_pointer[0xFF];
    fp = NULL;

    memset(AP_buffer, 0, BUFFER_SIZE);
    strcpy(path_pointer, PATH);
    openFile(&fp, path_pointer);

    while(!feof(fp)){
        #if DEBUG
        printf("%d: %f        %d\n", total_packets, arrival, packet_size);
        #endif
        arrival = getArrivalTime(fp); // current time
        packet_size = getSize(fp); // size of packet
        departure = serviceTime(AP_buffer[first_in]) + start; // expected time to finish
        if (arrival >= departure){
            //unbuffer
            AP_buffer[first_in] = 0; // get rid of packet
            buffered_packets--; // tell buffer that we can allow more packets in
            iPlus(&first_in, BUFFER_SIZE); // start working on next packet in buffer
            start = arrival; // start processing next packet at current time stamp
        }
        if (buffered_packets < BUFFER_SIZE){
            //add to buffer
            buffered_packets++; // fill up one more spot in the buffer
            iPlus(&last_in, BUFFER_SIZE); // this will never be used, but this is the last packet to be buffered
            AP_buffer[available_space] = packet_size; // "buffer" the packet
            iPlus(&available_space, BUFFER_SIZE); // space following this is now the next available
            total_size += packet_size; // for calculating average delay, only count buffered packets
        }
        else{
            // do nothing with packet, it is dead to us
            dropped_packets++;
        }
        total_packets++; // keep track of all packets
    }
    avg_pack = total_size/(total_packets - dropped_packets); // average packet size
    /* screen stuff for collecting data */ 
    printf("total packets: %d\ndropped packets: %d\nprocessed packets: %d\n", total_packets, dropped_packets, total_packets - dropped_packets);
    printf("total size of packets: %lld(bytes)\naverage delay: %f(sec)\n", total_size, serviceTime(avg_pack) * (BUFFER_SIZE - 1));
    printf("packet loss percentage: %f%%\n", ((double)(dropped_packets)/(double)(total_packets)) * 100 );
    closeFile(fp); // clean up

    return 0;
}

/* -----------------------------------------------
 *  Author: Brian Bowden
 *  Function Name: double getArrivalTime(FILE *fp)
 *  Input: *fp - a pointer to the file we want to scan
 *  Returns: arrival time (in seconds) in a well formatted file
 *  Usage: needed for everything up to the first space in a line,
 *         ideally reads this line as a (double) time stamp
 * -----------------------------------------------
 * */
double getArrivalTime(FILE *fp){
    double AT;
    fscanf(fp, "%lf", &AT);
    return AT;
}

/* -----------------------------------------------
 *  Author: Brian Bowden
 *  Function Name: int getSize(FILE *fp)
 *  Input: *fp - a pointer to the file we want to scan 
 *  Returns: packet size (in bytes) in a well formatted file
 *  Usage: needed for everything up to the first space and new line,
 *         ideally reads this line as a (int) packet size
 * -----------------------------------------------
 * */
int getSize(FILE *fp){  
    int size;
    fscanf(fp, "%d\n", &size);
    return size;
}

/* -----------------------------------------------
 *  Author: Brian Bowden
 *  Function Name: void openFile(FILE **fp, char* path)
 *  Input: **fp - address of filepointer to be set to
 *         *path - string (char pointer) conting the file path
 *  Returns: fp, either as null or an opened file
 *  Usage: simple file opener for tidier code up top
 * -----------------------------------------------
 * */
void openFile(FILE **fp, char* path){
    *fp = fopen(path, "r"); //readable only
    if (*fp == NULL){
        fprintf(stderr, "Cannot open File \"%s\"\n", path);
    }
}

/* -----------------------------------------------
 *  Author: Brian Bowden
 *  Function Name: void closeFile(FILE *fp)
 *  Input: *fp - file pointer associated with currently open file
 *  Usage: simple file closer to tidier code up top
 * -----------------------------------------------
 * */
void closeFile(FILE *fp){
    fclose(fp);
    #if DEBUG
    printf("File closed successfully! \n");
    #endif
}

/* -----------------------------------------------
 *  Author: Brian Bowden
 *  Function Name: double serviceTime(int packet_size)
 *  Input: packet_size - the size of the packet we want to know its associated
 *                       service time
 *  Returns: service_time - time in seconds about how long it would take to 
 *                          process the given packet through a set outspeed time
 *  Usage: anytime we need the service time of a certain packet
 * -----------------------------------------------
 * */
double serviceTime(int packet_size){
    double service_time;
    service_time = (double)((double)packet_size * 8)/(OUTSPEED * 1024 * 1024);
    return service_time;

}

/* -----------------------------------------------
 *  Author: Brian Bowden
 *  Function Name: void iPlus(int* index, int max)
 *  Input: *index - pointer to an int we want to index up
 *         max - the maximum size index should be
 *  Returns: the new index
 *  Usage: anytime we need to keep track of an index in a circular
 *         buffer.
 *  NOTE: realistically, this can be implented easier by:
 *           index = (index + 1) % max
 *        but I am stubborn, and forgot how to actually implement a circular 
 *        array.
 * -----------------------------------------------
 * */
void iPlus(int* index, int max){
    (*index)++;
    if (*index >= max)
        *index = 0;
}
