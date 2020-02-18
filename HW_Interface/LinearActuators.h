#ifndef LINEARACTUATORS_H
#define LINEARACTUATORS_H

#include <Arduino.h>

#define L_LA_ENC_1    32  // Left linear actuator encoder input 1
#define L_LA_ENC_2    31  // Left linear actuator encoder input 2
#define R_LA_ENC_1    30  // Right linear actuator encoder input 1
#define R_LA_ENC_2    29  // Right linear actuator encoder input 2
#define L_LA_CTRL     23  // Left linear actuator control line
#define R_LA_CTRL     22  // Right linear actuator control line

extern volatile uint8_t L_LA_enc_state, R_LA_enc_state;
extern volatile int32_t L_LA_enc_count, R_LA_enc_count;

void LA_init();		// Set up code for both linear actuators
void L_ENC_ISR();	// ISR for encoder interrupts for left LA
void R_ENC_ISR();	// ISR for encoder interrupts for right LA

#endif
