#include "LinearActuators.h"

volatile uint8_t L_LA_enc_state, R_LA_enc_state;
volatile int32_t L_LA_enc_count, R_LA_enc_count;

// Set up code for both linear actuators
void LA_init() {
  // Start with encoders zeroed
  L_LA_enc_count = 0;
  R_LA_enc_count = 0;
  
	// Initialize motor outputs to the stopped setting (50% duty cycle)
	analogWrite(L_LA_CTRL, 128);
	analogWrite(R_LA_CTRL, 128);

	// Setup encoder input pins
	pinMode(L_LA_ENC_1, INPUT_PULLUP);
	pinMode(L_LA_ENC_2, INPUT_PULLUP);
	pinMode(R_LA_ENC_1, INPUT_PULLUP);
	pinMode(R_LA_ENC_2, INPUT_PULLUP);

	// Set the initial state of the encoder variables
	L_LA_enc_state = (digitalRead(L_LA_ENC_1) << 3) | (digitalRead(L_LA_ENC_2) << 2);
	R_LA_enc_state = (digitalRead(R_LA_ENC_1) << 3) | (digitalRead(R_LA_ENC_2) << 2);

	// Attach interrupts to the inputs from the encoders
	attachInterrupt(L_LA_ENC_1, L_ENC_ISR, CHANGE);
	attachInterrupt(L_LA_ENC_2, L_ENC_ISR, CHANGE);
	attachInterrupt(R_LA_ENC_1, R_ENC_ISR, CHANGE);
	attachInterrupt(R_LA_ENC_2, R_ENC_ISR, CHANGE);
}

// Zero both linear actuators
void LA_zero() {
  // TODO: Drive both actuators to one end (top or bottom) to zero them
  //       Potentially watch the encoders to see when they stop
}

// ISR for encoder interrupts for left LA
void L_ENC_ISR() {
	L_LA_enc_state = (digitalRead(L_LA_ENC_1) << 3) | (digitalRead(L_LA_ENC_2) << 2) | (L_LA_enc_state >> 2);
	if (L_LA_enc_state == 0b0111 || L_LA_enc_state == 0b1110 || L_LA_enc_state == 0b1000 || L_LA_enc_state == 0b0001)
		L_LA_enc_count++;
	if (L_LA_enc_state == 0b0010 || L_LA_enc_state == 0b1011 || L_LA_enc_state == 0b1101 || L_LA_enc_state == 0b0100)
		L_LA_enc_count--;
}

// ISR for encoder interrupts for right LA
void R_ENC_ISR() {
	R_LA_enc_state = (digitalRead(R_LA_ENC_1) << 3) | (digitalRead(R_LA_ENC_2) << 2) | (R_LA_enc_state >> 2);
	if (R_LA_enc_state == 0b0111 || R_LA_enc_state == 0b1110 || R_LA_enc_state == 0b1000 || R_LA_enc_state == 0b0001)
		R_LA_enc_count++;
	if (R_LA_enc_state == 0b0010 || R_LA_enc_state == 0b1011 || R_LA_enc_state == 0b1101 || R_LA_enc_state == 0b0100)
		R_LA_enc_count--;
}
