#include <PWMServo.h>
#include "ComputerComms.h"
#include "LinearActuators.h"
#include "Slider.h"
#include "Ultrasonics.h"

#define SERVO_PITCH   20  // Pitch servo pin
#define SERVO_ROLL    14  // Roll servo pin
#define MD_INPUT      5   // Metal detector input

PWMServo pitch_servo, roll_servo;

void setup() {
  setup_comms(&Serial, 115200);

  // Initialize linear actuator functionality
  LA_init();

  // Initialize ultrasonic sensors
  ultrasonics_init();

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
  //        Note: cubic nonsense that slows down before hitting the mechanical stop on each end 
  // TODO: A control loop for the sensor head actuation
  //        Note: This will be a control loop where the ultrasonic distance will move the linear actuators until the encoders match the desired height
  //        This will also influence the servos on the sensor head if the sensor head needs to slant at all 
  // TODO: Read ultrasonics in sequence
  //       Note: this is a long process - it needs to be nonblocking and split across many loop iterations
  //        Note: this will be a mini state machine to keep them running in sequence, with the interupts changing the states

    // Ultrasonics are read in sequence (1-4)
    // If the set of readings is completed in less than 25 ms, wait until 25 ms has passed before triggering another set
    if ((millis() - ultrasonic_last_start_time > ULTRASONIC_OPERATING_PERIOD) && (ultrasonic_state = idle)) {
        ultrasonic_last_start_time = millis();
        ultrasonic_state = US_1;
    }
    switch (ultrasonic_state) {
    case US_1:
        pinMode(ULTRASONIC_1, OUTPUT);
        digitalWrite(ULTRASONIC_1, HIGH);
        delayMicroseconds(10);
        digitalWrite(ULTRASONIC_1, LOW);
        pinMode(ULTRASONIC_1, INPUT);
        ultrasonic_state = reading;
        break;
    case US_2:
        pinMode(ULTRASONIC_2, OUTPUT);
        digitalWrite(ULTRASONIC_2, HIGH);
        delayMicroseconds(10);
        digitalWrite(ULTRASONIC_2, LOW);
        pinMode(ULTRASONIC_2, INPUT);
        ultrasonic_state = reading;
        break;
    case US_3:
        pinMode(ULTRASONIC_3, OUTPUT);
        digitalWrite(ULTRASONIC_3, HIGH);
        delayMicroseconds(10);
        digitalWrite(ULTRASONIC_3, LOW);
        pinMode(ULTRASONIC_3, INPUT);
        ultrasonic_state = reading;
        break;
    case US_4:
        pinMode(ULTRASONIC_4, OUTPUT);
        digitalWrite(ULTRASONIC_4, HIGH);
        delayMicroseconds(10);
        digitalWrite(ULTRASONIC_4, LOW);
        pinMode(ULTRASONIC_4, INPUT);
        ultrasonic_state = reading;
        break;
    default:    // reading or idle
        break;
    }

  // TODO: Update pitch/roll servos based on ultrasonic readings
  // TODO: Poll the pin coming from the metal detector
  //        Note: On the rising edge of a frequency: Scan for the maximum value to send as the dGPS location
}
