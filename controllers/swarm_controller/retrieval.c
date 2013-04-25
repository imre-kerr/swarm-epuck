/*
 * retrieval.c - Follow and push behavior.
 *
 *	Made to make the e-puck converge and push the box.
 *  Created on: 17. mars 2011
 *      Author: jannik
 */
#include <stdio.h>

#define NB_LEDS 8
#define ON 1
#define OFF 0
#define TRUE 1
#define FALSE 0
#define PUSH_THRESHOLD 500

/* Wheel speed variables */
static double left_wheel_speed;
static double right_wheel_speed;

/* LED variables */
int LED[8];

/* iterator */
int i = 0;

/* Boolean variables */
int converge = FALSE; // Moving towards the box
int push = FALSE; // Standing close to the box

/******************************
 * Internal functions
*******************************/

static void update_speed(int IR_number)
{
	if (IR_number==0)
		left_wheel_speed = left_wheel_speed + 700;
	else if(IR_number == 7)
		right_wheel_speed = right_wheel_speed + 700;
	else if(IR_number == 1)
		left_wheel_speed = left_wheel_speed + 350;
	else if (IR_number == 6)
		right_wheel_speed = right_wheel_speed + 350;
	else if (IR_number == 2)
	{
		left_wheel_speed = left_wheel_speed + 550;
		right_wheel_speed = right_wheel_speed - 300;
	}
    else if (IR_number == 5)
    {
        right_wheel_speed = right_wheel_speed + 550;
        left_wheel_speed = left_wheel_speed - 300;
    }
    else if (IR_number == 3)
        left_wheel_speed = left_wheel_speed + 500;

    else if(IR_number == 4)
        right_wheel_speed = right_wheel_speed + 500;
}
/* The movement for converging to the box */
static void converge_to_box(int IR_sensor_value[8], int IR_threshold)
{
	left_wheel_speed = 0;
	right_wheel_speed = 0;
	for(i=0;i<NB_LEDS;i++){
		if(IR_sensor_value[i] < IR_threshold)
		{
			LED[i] = ON;
			update_speed(i);
		}
		else
			LED[i]=OFF;
	}
}
/* The behavior when pushing the box */
static void push_box(int IR_sensor_value[8], int IR_threshold)
{
	left_wheel_speed = 0;
	right_wheel_speed = 0;

	// Blink for visual pushing feedback
	for(i=0;i<NB_LEDS;i++){
		if(LED[i])
			LED[i] = OFF;
		else
			LED[i]=ON;
		if(IR_sensor_value[i] < IR_threshold)
			update_speed(i);
	}
	if((IR_sensor_value[0]<IR_threshold) && (IR_sensor_value[7]<IR_threshold))
	{
		left_wheel_speed = 1000;
		right_wheel_speed = 1000;
	}
}

/* Selects the behavior push or converge */
static void select_behavior(int IR_sensor_value[8])
{
	push = FALSE;
	converge = TRUE;
	for(i=0;i<NB_LEDS;i++){
		if (IR_sensor_value[i] < PUSH_THRESHOLD){
			push = TRUE;
			break;
		}
	}
}
/******************************
 * External functions
*******************************/

/* Converge, push, and stagnation recovery */
void swarm_retrieval(int IR_sensor_value[8], int IR_threshold)
{
	select_behavior(IR_sensor_value);
	if(push)
		push_box(IR_sensor_value, IR_threshold);
	else // converge
		converge_to_box(IR_sensor_value, IR_threshold);
}

/* */
double get_retrieval_left_wheel_speed()
{
	return left_wheel_speed;
}

/* */
double get_retrieval_right_wheel_speed()
{
	return right_wheel_speed;
}

/* Returns the state (ON/OFF) of the given LED number */
int get_LED_state(int LED_num)
{
	return LED[LED_num];
}
