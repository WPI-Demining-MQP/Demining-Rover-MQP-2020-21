#include "Ultrasonics.h"

volatile uint32_t timer = 0;
volatile double ultrasonic_1_dist = 0;
volatile double ultrasonic_2_dist = 0;
volatile double ultrasonic_3_dist = 0;
volatile double ultrasonic_4_dist = 0;

volatile enum ultrasonic_state_t ultrasonic_state = idle;

uint32_t ultrasonic_last_start_time = 0;

void ultrasonics_init() {
	// Setup ultrasonics to initial state (ready to trigger)
	pinMode(ULTRASONIC_1, OUTPUT);
	pinMode(ULTRASONIC_2, OUTPUT);
	pinMode(ULTRASONIC_3, OUTPUT);
	pinMode(ULTRASONIC_4, OUTPUT);

	// Attach interrupts
	attachInterrupt(ULTRASONIC_1, ultrasonic_1_ISR, CHANGE);
	attachInterrupt(ULTRASONIC_2, ultrasonic_2_ISR, CHANGE);
	attachInterrupt(ULTRASONIC_3, ultrasonic_3_ISR, CHANGE);
	attachInterrupt(ULTRASONIC_4, ultrasonic_4_ISR, CHANGE);
}

void ultrasonic_1_ISR() {
	if (digitalRead(ULTRASONIC_1)) {
		timer = micros();
	}
	else {
		ultrasonic_1_dist = US_TO_DIST(micros() - timer);
		ultrasonic_state = US_2;
	}
}

void ultrasonic_2_ISR() {
	if (digitalRead(ULTRASONIC_2)) {
		timer = micros();
	}
	else {
		ultrasonic_2_dist = US_TO_DIST(micros() - timer);
		ultrasonic_state = US_3;
	}
}

void ultrasonic_3_ISR() {
	if (digitalRead(ULTRASONIC_3)) {
		timer = micros();
	}
	else {
		ultrasonic_3_dist = US_TO_DIST(micros() - timer);
		ultrasonic_state = US_4;
	}
}

void ultrasonic_4_ISR() {
	if (digitalRead(ULTRASONIC_4)) {
		timer = micros();
	}
	else {
		ultrasonic_4_dist = US_TO_DIST(micros() - timer);
		ultrasonic_state = idle;
	}
}
