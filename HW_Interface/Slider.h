#ifndef SLIDER_H
#define SLIDER_H

#include <Arduino.h>

#define SLIDER_CTRL   21  // Slider motor control line
#define SLIDER_ENC_1  28  // Slider encoder line 1
#define SLIDER_ENC_2  27  // Slider encoder line 2
#define L_LIMIT       16  // Left limit switch line
#define R_LIMIT       15  // Right limit switch line

extern volatile uint8_t slider_enc_state;
extern volatile int32_t slider_enc_count;

void slider_init();
void slider_enc_ISR();

#endif
