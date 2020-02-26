#ifndef ULTRASONICS_H
#define ULTRASONICS_H

#include <Arduino.h>

#define US_TO_DIST(delta_t) (343.0 * (delta_t) / 2000000.0)

#define ULTRASONIC_1  1   // Ultrasonic 1 (trigger and echo)
#define ULTRASONIC_2  2   // Ultrasonic 2 (trigger and echo)
#define ULTRASONIC_3  3   // Ultrasonic 3 (trigger and echo)
#define ULTRASONIC_4  4   // Ultrasonic 4 (trigger and echo)

#define ULTRASONIC_OPERATING_FREQ 40
#define ULTRASONIC_OPERATING_PERIOD	1000/ULTRASONIC_OPERATING_FREQ
extern uint32_t ultrasonic_last_start_time;

extern volatile uint32_t timer;
extern volatile double ultrasonic_1_dist;
extern volatile double ultrasonic_2_dist;
extern volatile double ultrasonic_3_dist;
extern volatile double ultrasonic_4_dist;
enum ultrasonic_state_t { US_1, US_2, US_3, US_4, idle, reading };
extern volatile enum ultrasonic_state_t ultrasonic_state;

void ultrasonics_init();

void ultrasonic_1_ISR();
void ultrasonic_2_ISR();
void ultrasonic_3_ISR();
void ultrasonic_4_ISR();

#endif
