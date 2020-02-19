#include "Slider.h"

volatile uint8_t slider_enc_state;
volatile int32_t slider_enc_count;

void slider_init() {
  // Initialize slider motor output to the stopped setting (50% duty cycle)
  analogWrite(SLIDER_CTRL, 128);

  // Setup slider limit switch inputs
  pinMode(L_LIMIT, INPUT_PULLUP);
  pinMode(R_LIMIT, INPUT_PULLUP);

  // Attach interrupts
  attachInterrupt(SLIDER_ENC_1, slider_enc_ISR, CHANGE);
  attachInterrupt(SLIDER_ENC_2, slider_enc_ISR, CHANGE);
}

// ISR for encoder interrupts for left LA
void slider_enc_ISR() {
  slider_enc_state = (digitalRead(SLIDER_ENC_1) << 3) | (digitalRead(SLIDER_ENC_2) << 2) | (slider_enc_state >> 2);
  if (slider_enc_state == 0b0111 || slider_enc_state == 0b1110 || slider_enc_state == 0b1000 || slider_enc_state == 0b0001)
    slider_enc_count++;
  if (slider_enc_state == 0b0010 || slider_enc_state == 0b1011 || slider_enc_state == 0b1101 || slider_enc_state == 0b0100)
    slider_enc_count--;
}
