#include <webots/robot.h>
#include <webots/distance_sensor.h>
#include <webots/differential_wheels.h>
#include <stdio.h>
#include <stdlib.h>
#include "retrieval.h"
#include "search.h"
#include "stagnation.h"

#define TIME_STEP 32
#define DISTANCE_SENSORS 8
#define DISTANCE_TRESHOLD 250

int main(){
	int i;
	wb_robot_init();
	
	WbDeviceTag distance_sensors[DISTANCE_SENSORS];
	double distance_sensor_data[DISTANCE_SENSORS];
	
	//Setup distance sensors
	char sensor_name[10];
	for(i = 0; i < DISTANCE_SENSORS; i++){
		sprintf(sensor_name, "ps%d", i);
		distance_sensors[i] = wb_robot_get_device(sensor_name);
		wb_distance_sensor_enable(distance_sensors[i], TIME_STEP);
	}
	
	for(i = 0; i < DISTANCE_SENSORS; i++){
		distance_sensor_data[i] = wb_distance_sensor_get_value(distance_sensors[i]);
	}
	
	while(1){
		wb_robot_step(TIME_STEP);
		update_search_speed(distance_sensor_data, DISTANCE_TRESHOLD);
		wb_differential_wheels_set_speed(get_search_left_wheel_speed(), get_search_right_wheel_speed());
		for(i = 0; i < DISTANCE_SENSORS; i++){
			distance_sensor_data[i] = wb_distance_sensor_get_value(distance_sensors[i]);
		}
	}
	
	return 0;
}