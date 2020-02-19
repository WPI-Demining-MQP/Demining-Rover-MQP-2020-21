#ifndef COMPUTERCOMMS_H
#define COMPUTERCOMMS_H

#include <Arduino.h>

#define START_BYTE 0xFD
#define MAX_DATA_SIZE 128

/* PACKET STRUCTURE
 *  1) Start byte (0xFD)          uint8_t
 *  2) Data (message) type byte   uint8_t
 *  3) Data length byte           uint8_t
 *  4) Data bytes                 uint8_t[]
 *  5) Parity byte                uint8_t
 */

 // Simplified packet structure for received data
struct packet_t {
    uint8_t msg_type;
    uint8_t msg_len;
    uint8_t data[MAX_DATA_SIZE];
};

#define PACKET_T_SIZE sizeof(packet_t)

enum message_types {
    MSG_SYSTEM_STATE = 0,   // State of the system, as defined be the system_state_t enum
    MSG_STATUS = 1,         // Status message (text string) sent to the computer
    MSG_ACK = 2,            // Acknowledgement - can signal in-process or complete
};
#define INVALID_MSG_TYPE_CUTOFF 3       // Any message type >= 7 is considered invalid

// Current state of the system
enum system_state_t {
    SYS_STATE_UNINIT,       // Uninitialized
    SYS_STATE_INIT,         // Initializing system
    SYS_STATE_STANDBY,      // Waiting for further instruction
    SYS_STATE_ACTIVE        // Active, but not doing anything
};

enum parse_status_t { PARSE_UNINIT, PARSE_IDLE, PARSE_TYPE, PARSE_LEN, PARSE_DATA, PARSE_PARITY };

void setup_comms(usb_serial_class*, uint32_t);
void send_message(uint8_t, uint8_t, uint8_t*);
uint8_t generate_parity(uint8_t*, uint8_t);
uint8_t accumulate_parity(uint8_t, uint8_t);
bool receive_message(packet_t*);
bool receive_byte(packet_t*, uint8_t);

void send_msg_status(const char*);            // Sends a status message containing the passed string

#endif
