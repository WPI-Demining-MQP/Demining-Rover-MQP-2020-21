#include <PWMServo.h>

#define L_LA_ENC_1    32  // Left linear actuator encoder input 1
#define L_LA_ENC_2    31  // Left linear actuator encoder input 2
#define R_LA_ENC_1    30  // Right linear actuator encoder input 1
#define R_LA_ENC_2    29  // Right linear actuator encoder input 2
#define L_LA_CTRL     23  // Left linear actuator control line
#define R_LA_CTRL     22  // Right linear actuator control line
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

volatile int32_t L_LA_ENC_val = 0, R_LA_ENC_val = 0;    // global encoder readings for left and right linear actuator encoders

PWMServo pitch_servo, roll_servo;

void setup() {
    // Setup encoder inputs as inputs
    pinMode(L_LA_ENC_1, INPUT);
    pinMode(L_LA_ENC_2, INPUT);
    pinMode(R_LA_ENC_1, INPUT);
    pinMode(R_LA_ENC_2, INPUT);
    pinMode(SLIDER_ENC_1, INPUT);
    pinMode(SLIDER_ENC_2, INPUT);

    // Initialize motor outputs to the stopped setting (50% duty cycle)
    analogWrite(L_LA_CTRL, 128);
    analogWrite(R_LA_CTRL, 128);
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


    // TODO: Interrupt code might be sketchy, will need testing
    // Attach interrupts
    attachInterrupt(L_LA_ENC_1, L_LA_ENC_ISR_A, CHANGE);
    attachInterrupt(L_LA_ENC_2, L_LA_ENC_ISR_B, CHANGE);
    //attachInterrupt(R_LA_ENC_1, R_LA_ENC_ISR_A, CHANGE);
    //attachInterrupt(R_LA_ENC_2, R_LA_ENC_ISR_B, CHANGE);
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
}

// NOTE: Encoder code is prelimrinary until tested. Will likely be wrapped into a class in a separate file
void L_LA_ENC_ISR_A() {
    bool p1 = digitalRead(L_LA_ENC_1);
    bool p2 = digitalRead(L_LA_ENC_2);
    if (p1) {
        if (p2)
            L_LA_ENC_val++;
        else
            L_LA_ENC_val--;
    }
    else {
        if (p2)
            L_LA_ENC_val--;
        else
            L_LA_ENC_val++;
    }
}

void L_LA_ENC_ISR_B() {
    bool p1 = digitalRead(L_LA_ENC_1);
    bool p2 = digitalRead(L_LA_ENC_2);
    if (p2) {
        if (p1)
            L_LA_ENC_val--;
        else
            L_LA_ENC_val++;
    }
    else {
        if (p1)
            L_LA_ENC_val++;
        else
            L_LA_ENC_val--;
    }
}
