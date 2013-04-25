#include <webots/robot.h>
#include <webots/distance_sensor.h>
#include <webots/differential_wheels.h>
#include <stdio.h>

#define TIME_STEP 32

int main(){
	wb_robot_init();
	
	while(1){
		wb_differential_wheels_set_speed(500, 500);
		wb_robot_step(TIME_STEP);
	}
	
	return 0;
}