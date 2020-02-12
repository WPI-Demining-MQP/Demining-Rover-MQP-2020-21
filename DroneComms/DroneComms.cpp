#include "DroneComms.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>


int comms_port;
enum system_state_t system_state = SYS_STATE_UNINIT;

enum command_status_t command_status;

clock_t cmd_last_sent_time;
enum message_types cmd_last_sent_type;

int setup_comms() {
	comms_port = open("/dev/ttyUSB0", O_RDWR);
    if (comms_port < 0) {
        printf("Comms setup, error %i from open: %s\n", errno, strerror(errno));
        return 1;
    }
    //else {
    //    printf("Port opened\n");
    //}

    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(comms_port, &tty) != 0) {
        printf("Comms setup, error %i from tcgetattr: %s\n", errno, strerror(errno));
        return 1;
    }
    tty.c_cflag &= ~PARENB;	 // No parity
    tty.c_cflag &= ~CSTOPB;  // One stop bit
    tty.c_cflag |= CS8;      // 8 data bits
    tty.c_cflag &= ~CRTSCTS; // No flow control
    tty.c_cflag |= CREAD | CLOCAL;  // Turn on read and ignore control lines

    tty.c_lflag &= ~ICANON;  // Disable canonical mode
    tty.c_lflag &= ~ISIG;    // Disable interpretation of INTR, QUIT, and SUSP

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

    tty.c_oflag &= ~OPOST;
    tty.c_oflag &= ~ONLCR;

    tty.c_cc[VTIME] = 0;
    tty.c_cc[VMIN] = 0;

    cfsetispeed(&tty, B57600);
    cfsetospeed(&tty, B57600);

    if (tcsetattr(comms_port, TCSANOW, &tty) != 0) {
        printf("Comms setup, error %i from tcsetattr: %s\n", errno, strerror(errno));
        return 1;
    }

    return 0;
}

void send_message(uint8_t msg_type, uint8_t data_len, uint8_t* data) {
    uint8_t parity = generate_parity(data, data_len);
    parity = accumulate_parity(parity, START_BYTE);
    parity = accumulate_parity(parity, msg_type);
    parity = accumulate_parity(parity, data_len);
    const uint8_t start_byte = START_BYTE;
    write(comms_port, &start_byte, 1);
    write(comms_port, &msg_type, 1);
    write(comms_port, &data_len, 1);
    //printf("%d\n%d\n%d\n",start_byte, msg_type, data_len);
    if (data_len != 0) {
        write(comms_port, (void*)data, data_len);
        //for(int i = 0; i < data_len; i++) {
        //   printf("%d\n", data[i]);
        //}
    }
    write(comms_port, &parity, 1);
    //printf("%d\n", parity);
}

// XOR's all data bytes together to get a parity byte
uint8_t generate_parity(uint8_t* data, uint8_t len) {
    uint8_t parity = 0;
    for (int i = 0; i < len; i++) {
        parity = accumulate_parity(parity, data[i]);
    }
    return parity;
}

// XOR's a single byte with an existing parity byte
uint8_t accumulate_parity(uint8_t parity, uint8_t c) {
    return(parity ^ c);
}

// Read everything out of the serial buffer
// returns the number of received packets, and puts those packets into an array of packets (pointed to by packet_ptr_arr)
bool receive_messages(packet_t* packet_ptr) {
    uint8_t byte_in;
    while (read(comms_port, &byte_in, 1) > 0) {
        if (receive_byte(packet_ptr, byte_in)) {
            return true;
        }
    }
    return false;
}

// Process a single received byte
// returns: true if a message was received, false otherwise
bool receive_byte(packet_t* packet_ptr, uint8_t c) {
    static packet_t received_packet;
    static uint8_t data_bytes_received;
    static uint8_t received_parity = 0;
    static enum parse_status_t parse_status = PARSE_UNINIT;
    switch (parse_status) {
    case PARSE_UNINIT:
    case PARSE_IDLE:    // Idle, waiting for a start byte to come through
        if (c == START_BYTE) {
            received_parity = START_BYTE;
            data_bytes_received = 0;
            parse_status = PARSE_TYPE;
        }
        break;
    case PARSE_TYPE:   // receiving the message type byte
        received_packet.msg_type = c;
        received_parity = accumulate_parity(received_parity, c);
        parse_status = PARSE_LEN;
        break;
    case PARSE_LEN:    // receiving the data length byte
        received_packet.msg_len = c;
        received_parity = accumulate_parity(received_parity, c);
        if (received_packet.msg_len != 0) {
            parse_status = PARSE_DATA;
        }
        else {
            parse_status = PARSE_PARITY;
        }
        break;
    case PARSE_DATA:    // receiving data
        received_packet.data[data_bytes_received] = c;
        data_bytes_received++;
        received_parity = accumulate_parity(received_parity, c);
        if (data_bytes_received == received_packet.msg_len) {    // If we have received the correct number of bytes,
            parse_status = PARSE_PARITY;                          // the next byte will be the parity byte
        }
        break;
    case PARSE_PARITY:  // receiving the parity byte
        parse_status = PARSE_IDLE;
        if (received_parity == c) {  // If the parity is correct...
            //memcpy(packet_ptr, &received_packet, PACKET_T_SIZE);  // Copy the packet data into the location specified by packet_ptr
            *packet_ptr = received_packet;
            return true;
        }
        // If the parity was incorrect, the function will ignore the contents of the message and return false
        break;
    }
    return false;
}

// Sends a heartbeat message to the base station
void send_msg_heartbeat() {
    send_message(MSG_HEARTBEAT, 1, (uint8_t*)(&system_state));
}

// Parses a received heartbeat message, returns the single data byte (system state)
uint8_t parse_msg_heartbeat(packet_t* packet_ptr) {
    return packet_ptr->data[0];
}

// Parses the contents of a status message, and returns the transferred string (as std::string)
std::string parse_msg_status(packet_t* packet_ptr) {
    return std::string((char*)packet_ptr->data, packet_ptr->msg_len);
}

// Sends a minefield message to the drone, stating that there are num_mines mines to be detonated
void send_msg_minefield(uint16_t num_mines) {
    uint8_t data[2] = { (uint8_t)num_mines, (uint8_t)(num_mines >> 8) };
    send_message(MSG_MINEFIELD, 2, data);
}

// Sends a mine message to the drone, conveying the location of a single mine
void send_msg_mine(uint16_t mine_id, int32_t lat, int32_t lon) {
    uint8_t data[10];
    data[0] = (uint8_t)mine_id;
    data[1] = (uint8_t)(mine_id >> 8);
    data[2] = (uint8_t)lat;
    data[3] = (uint8_t)(lat >> 8);
    data[4] = (uint8_t)(lat >> 16);
    data[5] = (uint8_t)(lat >> 24);
    data[6] = (uint8_t)lon;
    data[7] = (uint8_t)(lon >> 8);
    data[8] = (uint8_t)(lon >> 16);
    data[9] = (uint8_t)(lon >> 24);
    send_message(MSG_MINE, 10, data);
}

// Sends a takeoff message to the drone
void send_msg_takeoff() {
    send_message(MSG_TAKEOFF, 0, NULL);
    cmd_last_sent_time = clock();
    cmd_last_sent_type = MSG_TAKEOFF;
    command_status = SENT;
}

// Sends an abort message to the drone
void send_msg_abort() {
    send_message(MSG_ABORT, 0, NULL);
    cmd_last_sent_time = clock();
    cmd_last_sent_type = MSG_ABORT;
    command_status = SENT;
}

// Parses an ack message, returns the type of the acknowledged message
uint8_t parse_msg_ack(packet_t* packet_ptr){
    return packet_ptr->data[0];
}

// Parses a home message, puts the parsed lat/lon into the passed pointer locations
void parse_msg_home(packet_t* packet_ptr, int32_t* lat_ptr, int32_t* lon_ptr) {
    *lat_ptr = *((int32_t*)(packet_ptr->data));
    *lon_ptr = *((int32_t*)(packet_ptr->data + 4));
}

bool check_timeouts() {
    clock_t cur_time = clock();
    if(command_status == SENT && cur_time - cmd_last_sent_time >= NO_ACK_TIMEOUT_CLK) {
        return resend();
    }
    return false;
}

bool resend() {
    switch(cmd_last_sent_type) {
    case MSG_TAKEOFF:
        send_msg_takeoff();
        break;
    case MSG_ABORT:
        send_msg_abort();
        break;
    default:
        return false;
        break;
    }
    return true;
}

void set_command_status() {
    command_status = ACCEPTED;
}