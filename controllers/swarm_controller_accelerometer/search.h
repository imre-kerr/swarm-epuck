/*
 * search.h
 *
 *  Created on: 17. mars 2011
 *      Author: jannik
 */

#ifndef SEARCH_H_
#define SEARCH_H_


void update_search_speed(double sensor_value[8], int distance_threshold);
double get_search_left_wheel_speed();
double get_search_right_wheel_speed();

#endif /* SEARCH_H_ */
