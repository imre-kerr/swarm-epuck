/*
 * search.c - Search and Avoid behavior.
 *
 *	Made to calculate the speed from distance sensors input.
 *	By getting sensor input from the four front distance sensors on the e-puck
 *	it will be determined the speed of the left and right wheel according to
 *	the case script and a threshold. If nothing is in your way - search.
 *
 *	case script: the four first int's are the sensor input, while the last two are speed
 *  Created on: 17. mars 2011
 *      Author: jannik
 */

#include <stdlib.h>
#include <time.h>

#define COUNTLIMIT 20

/* Case scenarios for for navigation */
static int case_script [16][6]= {{0,0,0,0,1,1},{0,0,0,1,1,0},{0,0,1,0,1,0},{0,0,1,1,1,0},{0,1,0,0,0,1},{0,1,0,1,1,0},{0,1,1,0,0,1},{0,1,1,1,1,0},{1,0,0,0,0,1},{1,0,0,1,1,0},{1,0,1,0,0,1},{1,0,1,1,1,0},{1,1,0,0,0,1},{1,1,0,1,1,0},{1,1,1,0,0,1},{1,1,1,1,0,1}};

/* Wheel speed variables */
static double left_wheel_speed; // default 0
static double right_wheel_speed;

/* Random search speed */
static double rand_double_left;
static double rand_double_right;

/* Counter */
static int counter = 0;

/******************************
 * Internal functions
*******************************/

/* Generates random double for left and right search speed */
void randdouble()
{
	rand_double_left = rand()/((double)(RAND_MAX)+1);
	rand_double_right = rand()/((double)(RAND_MAX)+1);
}

/* Given the input compared to the case script; where do we want to go?*/
static void calculate_search_speed(int threshold_list[4]){
	int i;
	counter = counter +1;
	for(i=0;i<16;i++){
		if(threshold_list[0]==case_script[i][0] && threshold_list[1]==case_script[i][1] && threshold_list[2]==case_script[i][2] && threshold_list[3]==case_script[i][3]){
			if(counter==COUNTLIMIT)
			{
				counter = 0;
				randdouble();
			}
			if(case_script[i][4] == case_script[i][5]) // Free passage; Straight forward
			{
				left_wheel_speed =(rand_double_left*500) + 500;
				right_wheel_speed=(rand_double_right*500) + 500;
			}
			else if(case_script[i][4] == 1 && case_script[i][5]== 0) // Turn left
			{
				left_wheel_speed=-300;
				right_wheel_speed= 700;
			}
			else // Turn right
			{
				left_wheel_speed =700;
				right_wheel_speed =-300;

			}
			return;
		}
	}
}

/* Calculate if there is an obstacle or not, depending on the threshold */
static void calculate_treshold(double sensors[4], int distance_threshold){
	int i;
	int threshold_list[4];
	for(i=0;i<4;i++){
		if(sensors[i]>distance_threshold){
			threshold_list[i] = 1; // obstacle
		}
		else{
			threshold_list[i] = 0; // Free passage
		}
	}
	calculate_search_speed(threshold_list);
}


/*********************
 * External functions
**********************/

/* Given the sensor input and threshold, calculates the speed for survival */
void update_search_speed(double sensor_value[8], int distance_threshold)
{
	double sensors[4] = {sensor_value[6], sensor_value[7], sensor_value[0], sensor_value[1]};
	calculate_treshold(sensors, distance_threshold);
}

/* */
double get_search_left_wheel_speed()
{
	return left_wheel_speed;
}

/* */
double get_search_right_wheel_speed()
{
	return right_wheel_speed;
}

/* Needed to avoid windows "WinMain@16" error */
int WinMain()
{
	return 0;
}
