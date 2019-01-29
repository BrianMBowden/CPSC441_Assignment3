/* ============================================================================
 * Author: Brian Bowden
 * Date: November 12, 2018
 * ============================================================================
 * WirelessVideoBuffer.h
 * ============================================================================
 * Simulation for AP link
 * ============================================================================
 */
#ifndef WIRELESSVIDEOBUFFER_H
#define WIRELESSVIDEOBUFFER_H

#define DEBUG 0
/* default path name */
#define PATH "./seq.txt"
#define INSPEED 100 //Mbps

/* OUTSPEED can be changed for different data*/
// data tested: 6, 11, 30, 54, 100 
#define OUTSPEED 11 //Mbps 

/* BUFFER_SIZE can be canged for different data*/
//data tested: 10, 100, 10000, 100000,
#define BUFFER_SIZE 100
#define START 0.0

double getArrivalTime(FILE *fp);
int getSize(FILE *fp);
void openFile(FILE **fp, char* path);
void closeFile(FILE *fp);
double serviceTime(int packet_size);
void iPlus(int* index, int max);

#endif