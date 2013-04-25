#include <webots/robot.h>
#include <webots/distance_sensor.h>
#include <webots/differential_wheels.h>
#include <webots/light_sensor.h><<<<
#include <stdio.h>
#include <stdlib.h>
#include "retrieval.h"
#include "search.h"
#include "stagnation.h"

#define MIN(A, B) A<B?A:B

#define TIME_STEP 32
#define NUM_SENSORS 8
#define DISTANCE_TRESHOLD 250
#define DETECTION_TRESHOLD 3700

int main(){
	int i;
	wb_robot_init();
	
	WbDeviceTag distance_sensors[NUM_SENSORS];
	WbDeviceTag light_sensors[NUM_SENSORS];
	double distance_sensor_data[NUM_SENSORS];
	double previous_distance_sensor_data[NUM_SENSORS];
	int light_sensor_data[NUM_SENSORS];
	
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
	
	for(i = 0; i < NUM_SENSORS; i++){
		distance_sensor_data[i] = wb_distance_sensor_get_value(distance_sensors[i]);
		previous_distance_sensor_data[i] = wb_distance_sensor_get_value(distance_sensors[i]);
	}
	
	for(i = 0; i < NUM_SENSORS; i++){
		light_sensor_data[i] = (int) wb_light_sensor_get_value(light_sensors[i]);
	}
	
	while(1){
		wb_robot_step(TIME_STEP);
		update_search_speed(distance_sensor_data, DISTANCE_TRESHOLD);
		swarm_retrieval(light_sensor_data, DETECTION_TRESHOLD);
		valuate_pushing(distance_sensor_data, previous_distance_sensor_data);
		
		int case2 = 0;
		for(i = 0; i < NUM_SENSORS && !(case2 = light_sensor_data[i] < DETECTION_TRESHOLD); i++);
		
		if(case2){
			if(get_stagnation_state()){
				printf("Recovering\n");
				stagnation_recovery(distance_sensor_data, DISTANCE_TRESHOLD);
				wb_differential_wheels_set_speed(get_stagnation_left_wheel_speed(), get_stagnation_right_wheel_speed());
			} else {
				wb_differential_wheels_set_speed(MIN(get_retrieval_left_wheel_speed(), 1000), MIN(get_retrieval_right_wheel_speed(), 1000));
			}
		} else {
			wb_differential_wheels_set_speed(get_search_left_wheel_speed(), get_search_right_wheel_speed());
		}
		
		
		for(i = 0; i < NUM_SENSORS; i++){
			previous_distance_sensor_data[i] = distance_sensor_data[i];
			distance_sensor_data[i] = wb_distance_sensor_get_value(distance_sensors[i]);
		}
		
		for(i = 0; i < NUM_SENSORS; i++){
			light_sensor_data[i] = (int) wb_light_sensor_get_value(light_sensors[i]);
		}
	}
	
	return 0;
}