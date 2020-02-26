#ifndef ULTRASONICS_H
#define ULTRASONICS_H

#include <Arduino.h>

#define US_TO_DIST(delta_t) (343.0 * (delta_t) / 2000000.0)

#define ULTRASONIC_1  1   // Ultrasonic 1 (trigger and echo)
#define ULTRASONIC_2  2   // Ultrasonic 2 (trigger and echo)
#define ULTRASONIC_3  3   // Ultrasonic 3 (trigger and echo)
#define ULTRASONIC_4  4   // Ultrasonic 4 (trigger and echo)

extern volatile uint32_t timer;
extern volatile double ultrasonic_1_dist;
extern volatile double ultrasonic_2_dist;
extern volatile double ultrasonic_3_dist;
extern volatile double ultrasonic_4_dist;

void ultrasonics_init();

void ultrasonic_1_ISR();
void ultrasonic_2_ISR();
void ultrasonic_3_ISR();
void ultrasonic_4_ISR();

#endif