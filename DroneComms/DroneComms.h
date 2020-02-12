#ifndef DRONECOMMS_H
#define DRONECOMMS_H

#include <stdint.h>
#include <string>
#include <time.h>

#define START_BYTE 0xFD
#define MAX_DATA_SIZE 128

#define NO_ACK_TIMEOUT_SEC 0.2
#define NO_ACK_TIMEOUT_CLK NO_ACK_TIMEOUT_SEC*CLOCKS_PER_SEC

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
    MSG_HEARTBEAT = 0,   // Heartbeat - sent to and received from the base station
    MSG_STATUS = 1,      // Status message - sending a string to the base station to update the user and debug
    MSG_MINEFIELD = 2,   // Minefield info - received from the base station. Basically just telling the drone how many mines to expect
    MSG_MINE = 3,        // Mine info - received from the base station. Coordinates for a single mine
    MSG_TAKEOFF = 4,     // Signal from the base station to takeoff. Used as a trigger at the beggining of a run, or after a sandbag reload
    MSG_ABORT = 5,       // Signal from the base station to abort the current process and return to launch
    MSG_ACK = 6,         // Acknowledgement packet - Confirms the receipt of a packet of the specified type
    MSG_HOME = 7         // Home location - Passes the home location of the drone to the base station for path planning
};

enum system_state_t { SYS_STATE_UNINIT,
                      SYS_STATE_SETUP,
                      SYS_STATE_STANDBY,
                      SYS_STATE_ACTIVE,
                      SYS_STATE_EMERGENCY};
extern enum system_state_t system_state;

enum parse_status_t { PARSE_UNINIT, PARSE_IDLE, PARSE_TYPE, PARSE_LEN, PARSE_DATA, PARSE_PARITY };

enum command_status_t { SENT, ACCEPTED, REJECTED, COMPLETED };
extern enum command_status_t command_status;

extern clock_t cmd_last_sent_time;
extern enum message_types cmd_last_sent_type;

int setup_comms();
void send_message(uint8_t, uint8_t, uint8_t*);
uint8_t generate_parity(uint8_t*, uint8_t);
uint8_t accumulate_parity(uint8_t, uint8_t);
bool receive_messages(packet_t*);
bool receive_byte(packet_t*, uint8_t);

void send_msg_heartbeat();                  // Sends a heartbeat message to the base station
uint8_t parse_msg_heartbeat(packet_t*);     // Parses a received heartbeat message, returns the single data byte (system state)
std::string parse_msg_status(packet_t*);    // Parses the contsnts of a status message, and returns the transferred string (as std::string)
void send_msg_minefield(uint16_t);          // Sends a minefield message
void send_msg_mine(uint16_t, int32_t, int32_t); // Sends a mine message
void send_msg_takeoff();                    // Sends a takeoff message
void send_msg_abort();                      // Sends an abort message
uint8_t parse_msg_ack(packet_t*);           // Parses an ack message, returns the type of the acknowledged message
void parse_msg_home(packet_t*, int32_t*, int32_t*);  // Parses a home message, puts the parsed lat/lon into the passed pointer locations
bool check_timeouts();                      // Checks to see if a sent messge has gone unacknowledged - calls resend() if necessary
bool resend();                              // Resends the most recently sent message
void set_command_status();                  // Updates the command_status variable based on an ack message
#endif