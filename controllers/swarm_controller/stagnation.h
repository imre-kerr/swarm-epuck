/*
 * stagnation.h
 *
 *  Created on: 23. mars 2011
 *      Author: jannik
 */

#ifndef STAGNATION_H_
#define STAGNATION_H_

int get_stagnation_state();
int get_green_LED_state();
double get_stagnation_left_wheel_speed();
double get_stagnation_right_wheel_speed();
void stagnation_recovery(double distance_sensors_value[8], int DIST_THRESHOLD);
void valuate_pushing(double dist_value[8], double prev_dist_value[8]);
void reset_stagnation();
void find_new_spot(double distance_value[8], int DIST_THRESHOLD);

#endif /* STAGNATION_H_ */
