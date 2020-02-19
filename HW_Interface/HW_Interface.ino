#include <PWMServo.h>
#include "ComputerComms.h"
#include "LinearActuators.h"
#include "Slider.h"

#define ULTRASONIC_1  1   // Ultrasonic 1 (trigger and echo)
#define ULTRASONIC_2  2   // Ultrasonic 2 (trigger and echo)
#define ULTRASONIC_3  3   // Ultrasonic 3 (trigger and echo)
#define ULTRASONIC_4  4   // Ultrasonic 4 (trigger and echo)
#define SERVO_PITCH   20  // Pitch servo pin
#define SERVO_ROLL    14  // Roll servo pin
#define MD_INPUT      5   // Metal detector input

PWMServo pitch_servo, roll_servo;

void setup() {
  setup_comms(&Serial, 115200);

  // Initialize linear actuator functionality
  LA_init();
  
  // Setup ultrasonics to initial state (ready to trigger)
  pinMode(ULTRASONIC_1, OUTPUT);
  pinMode(ULTRASONIC_2, OUTPUT);
  pinMode(ULTRASONIC_3, OUTPUT);
  pinMode(ULTRASONIC_4, OUTPUT);

  // Setup servos
  pitch_servo.attach(SERVO_PITCH);
  roll_servo.attach(SERVO_ROLL);
  pitch_servo.write(90);
  roll_servo.write(90);

  // Setup metal detector input
  pinMode(MD_INPUT, INPUT);

  
}

void loop() {
  // TODO: A control loop for each linear actuator
  // TODO: A control loop for the slider
  // TODO: Read ultrasonics in sequence
  //       Note: this is a long process - it needs to be nonblocking and split across many loop iterations
  //       Probably a small state machine for this. Potentially a separate class since there're 4 of them
  // TODO: Update pitch/roll servos based on ultrasonic readings
  // TODO: Poll the pin coming from the metal detector
}
