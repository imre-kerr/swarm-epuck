/*
 * stagnation.c Stagnation recovery behavior
 *
 * Whenever the e-puck reason about his push behavior not being effective,
 * the stagnation behavior should trigger. This behavior will reposition the
 * robot, hopefully getting a spot which will result
 *  Created on: 23. mars 2011
 *      Author: jannik
 */
#include "search.h"
#include <stdlib.h>
#include <stdio.h>

#define TRUE 1
#define FALSE 0
#define NEUTRAL 3
#define ON 1
#define OFF 0
#define IR_DIFF_THRESHOLD 4
#define DISTANCE_DIFF_THRESHOLD 10
#define REVERSE_LIMIT 20
#define TURN_LIMIT 10
#define FORWARD_LIMIT 40
#define NEIGHBOR_LIMIT 300

#define ALIGN_STRAIGTH_THRESHOLD 10 // If bigger, align straight
#define LOW_DIST_VALUE 10 // if lower (and detecting IR), the sensor is close.

/* Wheel speed variables */
static double left_wheel_speed;
static double right_wheel_speed;

/* Boolean variables */
static int has_recovered = FALSE;
static int turn_left = NEUTRAL;

/* Green LED */
static int green_LED_state = OFF; // Visual feedback

/* Counters */
static int reverse_counter = 0;
static int turn_counter = 0;
static int forward_counter = 0;
static int twice = 0;
static int align_counter = 0;

/******************************
 * Internal functions
*******************************/

/* Let it shine baby! */
void LED_blink()
{
	if(green_LED_state)
		green_LED_state = OFF;
	else
		green_LED_state = ON;
}

void realign(double distance_value[8])
{
	// Find the difference of the two front IR sensors
	int dist_diff_front = distance_value[7] - distance_value[0];

	// Are we pushing straight? If not, maybe we should try. If we are, maybe we should
	// try pushing from another angle.
	if(abs(dist_diff_front) > ALIGN_STRAIGTH_THRESHOLD) // True = we are not pushing straight
	{
		//Lets push straight, but which way are we angled?
		if(distance_value[0] < LOW_DIST_VALUE) //True = turn little right
		{
			right_wheel_speed = -500;
			left_wheel_speed = 500;
		}
		else if(distance_value[7] < LOW_DIST_VALUE) // True = turn little left
		{
			right_wheel_speed = 500;
			left_wheel_speed = -500;
		}
		else if(distance_value[1] < LOW_DIST_VALUE) // True = turn right
		{
			right_wheel_speed = -1000;
			left_wheel_speed = 700;
		}
		else if(distance_value[6] < LOW_DIST_VALUE) // True = turn left
		{
			right_wheel_speed = 700;
			left_wheel_speed = -1000;
		}
	}
	// We are standing straight, lets try pushing with another angle.
	else
	{
		// Roll a dice, left angle or right angle?
		double ran = rand()/((double)(RAND_MAX)+1);
		if (ran > 0.5)
		{
			right_wheel_speed = -500;
			left_wheel_speed = 500;
		}
		else
		{
			right_wheel_speed = 500;
			left_wheel_speed = -500;
		}
	}
	has_recovered = TRUE;
	green_LED_state = OFF;

}
/******************************
 * External functions
*******************************/

void find_new_spot(double distance_value[8], int DIST_THRESHOLD)
{
	if(twice == 2) // Reverse, Turn, Forward, Turn(opposite), Forward.
	{
		has_recovered = TRUE;
		green_LED_state = OFF;
		align_counter = 0;
	}
	else if(reverse_counter != REVERSE_LIMIT) // Make space by moving away from the box
	{
		reverse_counter = reverse_counter +1;
		left_wheel_speed = -800;
		right_wheel_speed = -800;
	}
	else if(turn_counter != TURN_LIMIT) // Line up with one of the sides of the box
	{
		turn_counter = turn_counter +1;
		forward_counter = 0;
		if(turn_left == NEUTRAL)
		{
		// Roll a dice, left or right?
		double ran = rand()/((double)(RAND_MAX)+1);
		if (ran > 0.5)
			turn_left = FALSE;
		else
			turn_left = TRUE;
		}

		if(turn_left) // Turn left
		{
			left_wheel_speed = -300;
			right_wheel_speed = 700;
		}
		else // Turn right
		{
			left_wheel_speed = 700;
			right_wheel_speed = -300;
		}
	}
	else if(forward_counter != FORWARD_LIMIT)
	{
		forward_counter = forward_counter +1;
		if(forward_counter == FORWARD_LIMIT-1)
		{
			twice = twice +1;
			turn_counter = 0;
			if(turn_left)
				turn_left = FALSE;
			else
				turn_left = TRUE;
		}
		update_search_speed(distance_value, DIST_THRESHOLD);
		left_wheel_speed = get_search_left_wheel_speed();
		right_wheel_speed = get_search_right_wheel_speed();

		if((left_wheel_speed > 0) && (right_wheel_speed> 0) )
		{
			right_wheel_speed = 1000;
			left_wheel_speed = 1000;
		}
	}

}

void reset_stagnation()
{
	has_recovered = FALSE;
	reverse_counter = 0;
	turn_counter = 0;
	forward_counter = 0;
	turn_left = NEUTRAL;
	twice = 0;
}
void stagnation_recovery(double distance_sensors_value[8], int DIST_THRESHOLD)
{
	if (align_counter < 2) // Align
	{
		align_counter = align_counter + 1;
		realign(distance_sensors_value);
	}

	else if(align_counter > 0)// Reposition
	{
		LED_blink();
		find_new_spot(distance_sensors_value, DIST_THRESHOLD);
	}

}

/* To keep pushing or not to keep pushing, that is the question */
void valuate_pushing(double dist_value[8], double prev_dist_value[8])
{	// Only assess this situation once
	// The front IR sensors pushing against the box
	int dist_diff7 = prev_dist_value[7] - dist_value[7];
	int dist_diff0 = prev_dist_value[0] - dist_value[0];

	if((abs(dist_diff7)> DISTANCE_DIFF_THRESHOLD) && (abs(dist_diff0)> DISTANCE_DIFF_THRESHOLD))
	{
		has_recovered = TRUE; // Keep pushing, it is working
		green_LED_state = OFF; // No more recovery
		align_counter = 0;
	}
	else if((dist_value[5] >NEIGHBOR_LIMIT)&&(dist_value[2]>NEIGHBOR_LIMIT)){ //Has any neighbors
		has_recovered = TRUE; // Keep pushing, it is working
		green_LED_state = OFF; // No more recovery
		align_counter = 0;
	}
	else if((dist_value[5] >NEIGHBOR_LIMIT)||(dist_value[2]>NEIGHBOR_LIMIT)){ //Has any neighbors
		// Roll a dice, do i trust just one team-mate?
		double ran = rand()/((double)(RAND_MAX)+1);
		if (ran > 0.5)
		{
			has_recovered = TRUE; // Keep pushing, it is working
			green_LED_state = OFF; // No more recovery
			align_counter = 0;
		}

	}

}

/* Return the boolean value of whether or not to continue with this behavior*/
int get_stagnation_state()
{
	if(has_recovered)
		return FALSE; // Recovered, stagnation behavior done
	return TRUE; // Still processing
}

/* Returns the state (ON/OFF) for green LED*/
int get_green_LED_state()
{
	return green_LED_state;
}

/* */
double get_stagnation_left_wheel_speed()
{
	return left_wheel_speed;
}

/* */
double get_stagnation_right_wheel_speed()
{
	return right_wheel_speed;
}
