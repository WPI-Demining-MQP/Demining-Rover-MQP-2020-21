#include <PWMServo.h>
#include "LinearActuators.h"

#define SLIDER_CTRL   21  // Slider motor control line
#define SLIDER_ENC_1  28  // Slider encoder line 1
#define SLIDER_ENC_2  27  // Slider encoder line 2
#define L_LIMIT       16  // Left limit switch line
#define R_LIMIT       15  // Right limit switch line
#define ULTRASONIC_1  1   // Ultrasonic 1 (trigger and echo)  ************************* on TX pin. Bad? ****************************************
#define ULTRASONIC_2  2   // Ultrasonic 2 (trigger and echo)
#define ULTRASONIC_3  3   // Ultrasonic 3 (trigger and echo)
#define ULTRASONIC_4  4   // Ultrasonic 4 (trigger and echo)
#define SERVO_PITCH   20  // Pitch servo pin
#define SERVO_ROLL    14  // Roll servo pin
#define MD_INPUT      5   // Metal detector input

PWMServo pitch_servo, roll_servo;

int32_t old_count = 0;

void setup() {
  Serial.begin(115200);

  // Initialize linear actuator functionality
  LA_init();
  
    // Initialize slider motor output to the stopped setting (50% duty cycle)
    analogWrite(SLIDER_CTRL, 128);

    // Setup slider limit switch inputs
    pinMode(L_LIMIT, INPUT_PULLUP);
    pinMode(R_LIMIT, INPUT_PULLUP);

    // Setup ultrasonics to initial state (ready to trigger)
    pinMode(ULTRASONIC_1, OUTPUT);
    pinMode(ULTRASONIC_2, OUTPUT);
    pinMode(ULTRASONIC_3, OUTPUT);
    pinMode(ULTRASONIC_4, OUTPUT);

    // Setup servos
    pitch_servo.attach(SERVO_PITCH);
    roll_servo.attach(SERVO_ROLL);

    // Setup metal detector input
    pinMode(MD_INPUT, INPUT);

    // Attach interrupts
    //attachInterrupt(SLIDER_ENC_1, SLIDER_ENC_ISR_A, CHANGE);
    //attachInterrupt(SLIDER_ENC_2, SLIDER_ENC_ISR_B, CHANGE);
}

void loop() {
    // TODO: A control loop for each linear actuator
    // TODO: A control loop for the slider
    // TODO: Read ultrasonics in sequence
    //       Note: this is a long process - it needs to be nonblocking and split across many loop iterations
    //       Probably a small state machine for this. Potentially a separate class since there're 4 of them
    // TODO: Update pitch/roll servos based on ultrasonic readings
    // TODO: Poll the pin coming from the metal detector
    if(L_LA_enc_count != old_count) {
      Serial.print(L_LA_enc_count);
      old_count = L_LA_enc_count;
    }
}
