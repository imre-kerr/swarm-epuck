#include <webots/robot.h>
#include <webots/distance_sensor.h>
#include <webots/differential_wheels.h>
#include <webots/light_sensor.h><<<<
#include <stdio.h>
#include <stdlib.h>
#include "retrieval.h"
#include "search.h"
#include "stagnation.h"

#define MIN(A, B) (A<B?A:B)
#define MAX(A, B) (A>B?A:B)

#define TIME_STEP 32
#define NUM_SENSORS 8
#define DISTANCE_TRESHOLD 250
#define DETECTION_TRESHOLD 3000
#define STAGNATION_TRESHOLD 500

int i;
int stagnated = 0;

WbDeviceTag distance_sensors[NUM_SENSORS];
WbDeviceTag light_sensors[NUM_SENSORS];
double distance_sensor_data[NUM_SENSORS];
double previous_distance_sensor_data[NUM_SENSORS];
int light_sensor_data[NUM_SENSORS];

int counter = 1;
int feedback = 5;
int timed_review = 150;
int stagnated;

void update_sensors(){
	for(i = 0; i < NUM_SENSORS; i++){
		previous_distance_sensor_data[i] = distance_sensor_data[i];
		distance_sensor_data[i] = wb_distance_sensor_get_value(distance_sensors[i]);
		light_sensor_data[i] = (int) wb_light_sensor_get_value(light_sensors[i]);
	}
}

int main(){

	wb_robot_init();
	
	//Setup sensors
	char sensor_name[10];
	for(i = 0; i < NUM_SENSORS; i++){
		sprintf(sensor_name, "ps%d", i);
		distance_sensors[i] = wb_robot_get_device(sensor_name);
		wb_distance_sensor_enable(distance_sensors[i], TIME_STEP);
		sprintf(sensor_name, "ls%d", i);
		light_sensors[i] = wb_robot_get_device(sensor_name);
		wb_light_sensor_enable(light_sensors[i], TIME_STEP);
	}
	
	wb_robot_step(TIME_STEP);
	
	for(i = 0; i < NUM_SENSORS; i++){
		distance_sensor_data[i] = wb_distance_sensor_get_value(distance_sensors[i]);
		previous_distance_sensor_data[i] = wb_distance_sensor_get_value(distance_sensors[i]);
		light_sensor_data[i] = (int) wb_light_sensor_get_value(light_sensors[i]);
	}
	
	
	while(1){
		wb_robot_step(TIME_STEP);
		
		
		int case2 = 0;
		for(i = 0; i < NUM_SENSORS && !(case2 = light_sensor_data[i] < DETECTION_TRESHOLD); i++);
		
		if(case2){
			if(counter){
				swarm_retrieval(light_sensor_data, DETECTION_TRESHOLD);
				wb_differential_wheels_set_speed(MIN(get_retrieval_left_wheel_speed(), 1000), MIN(get_retrieval_right_wheel_speed(), 1000));
			} else {
				wb_differential_wheels_set_speed(0, 0);
				wb_robot_step(TIME_STEP);
				update_sensors();
				reset_stagnation();
				valuate_pushing(distance_sensor_data, previous_distance_sensor_data);
				stagnated = 0;
				while(get_stagnation_state){
					stagnation_recovery(distance_sensor_data, DISTANCE_TRESHOLD);
					wb_differential_wheels_set_speed(get_stagnation_left_wheel_speed(), get_stagnation_right_wheel_speed());
					wb_robot_step(TIME_STEP);
					stagnated = 1;
				}
				if(stagnated){
					feedback = MIN(feedback+1, 8);
				} else {
					feedback = MAX(feedback-1, 1);
				}
				timed_review = ((5./(10-feedback))*100)+50;
			}
			counter %= counter + 1;
		} else {
			update_search_speed(distance_sensor_data, DISTANCE_TRESHOLD);
			wb_differential_wheels_set_speed(get_search_left_wheel_speed(), get_search_right_wheel_speed());
		}
		
		update_sensors();
	}
	
	return 0;
}