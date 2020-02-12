#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "DroneComms.h"
#include "PathPlan.h"
using namespace std;

#define HEARTBEAT_INTERVAL 1
#define UNRESPONSIVE_TIME 3
#define MSG_RETRY_TIME 0.8

#define MAX_PACKETS_IN 10	// Max number of packets that can be read at once

// Minefield data
const uint16_t num_mines = 9;
int32_t home_lat = 422755310, home_lon = -718047510;
int32_t dummy_mine_lats[num_mines] = { 422757210,  422756290,  422757810, 422757210,  422756290,  422757810, 422757210,  422756290,  422757810};
int32_t dummy_mine_lons[num_mines] = {-718049650, -718050280, -718051800, -718049650, -718050280, -718051800, -718049650, -718050280, -718051800};
//mine_t mines[num_mines];

int main() {
	node_t* head = NULL;	// Head of the linked list of mines
	for(int i = 0; i < num_mines; i++) {	// add dummy mines to linked list
		add_mine(&head, dummy_mine_lats[i], dummy_mine_lons[i]);
	}

	packet_t packet_in;
	clock_t last_received_heartbeat_time, last_sent_heartbeat_time, msg_last_sent_time = 0, cur_time = clock(), last_error_print_time = 0;
	last_sent_heartbeat_time = cur_time;
	last_received_heartbeat_time = cur_time;
	bool first_heartbeat_received = false, home_msg_received = false;
	uint16_t num_mines_sent = 0;
	enum setup_state_t { WAIT_FOR_HEARTBEAT, WAIT_FOR_HOME, PLAN_PATH, SEND_MINEFIELD, WAIT_FOR_MINEFIELD_ACK, SEND_MINES, WAIT_FOR_MINE_ACK, STANDBY, DONE } setup_state = WAIT_FOR_HEARTBEAT;
	if (setup_comms()) {	// setup communication with the radio (to the drone)
		// if the function returned 1, there was an error
		printf("Could not connect to radio, terminating\n");
		return 1;
	}
	else {
		printf("Successfully connected to radio\n");
	}
	printf("Waiting for drone heartbeat\n");
	system_state = SYS_STATE_ACTIVE;
	while (setup_state != DONE) {		// Setup loop
		switch (setup_state) {
		case WAIT_FOR_HEARTBEAT:
			if (first_heartbeat_received) {
				printf("Drone heartbeat received\n");
				printf("Waiting for home location\n");
				setup_state = WAIT_FOR_HOME;
			}
			break;
		case WAIT_FOR_HOME:
			if (home_msg_received) {
				printf("Home location received\n");
				setup_state = PLAN_PATH;
			}
			break;
		case PLAN_PATH:
			plan_path(home_lat, home_lon, &head);
			
			printf("Path planned:\n");
			for(int i = 0; i < num_mines; i++) {
				printf("Point: %d, %d\tEscape: %d, %d\n",mines[i].lat, mines[i].lon, mines[i].escape_lat, mines[i].escape_lon);
			}
			setup_state = SEND_MINEFIELD;
			break;
		case SEND_MINEFIELD:
			printf("Sending minefield message (%d mines)\n", num_mines);
			send_msg_minefield(num_mines);
			msg_last_sent_time = clock();
			setup_state = WAIT_FOR_MINEFIELD_ACK;
			break;
		case WAIT_FOR_MINEFIELD_ACK:
			cur_time = clock();
			if(cur_time - msg_last_sent_time >= MSG_RETRY_TIME*CLOCKS_PER_SEC) {
				printf("Minefield message not acknowledged, retrying...\n");
				msg_last_sent_time = cur_time;
				send_msg_minefield(num_mines);
			}
			break;	// state updated in message parsing code below
		case SEND_MINES:
			printf("Sending mine %d/%d\n", num_mines_sent+1, num_mines);
			send_msg_mine(num_mines_sent, mines[num_mines_sent].lat, mines[num_mines_sent].lon); // Send a mine message
			msg_last_sent_time = clock();
			setup_state = WAIT_FOR_MINE_ACK;
			break;
		case WAIT_FOR_MINE_ACK:
			cur_time = clock();
			if(cur_time - msg_last_sent_time >= MSG_RETRY_TIME*CLOCKS_PER_SEC) {
				printf("Mine message #%d not acknowledged, retrying...\n", num_mines_sent+1);
				msg_last_sent_time = cur_time;
				send_msg_mine(num_mines_sent, mines[num_mines_sent].lat, mines[num_mines_sent].lon);
			}
			break;	// state updated in message parsing code below
		case STANDBY:
			break;
		case DONE:
			break;
		}

		// Send a heartbeat every second
		cur_time = clock();
		if (cur_time - last_sent_heartbeat_time >= HEARTBEAT_INTERVAL*CLOCKS_PER_SEC) {
			last_sent_heartbeat_time = cur_time;
			send_msg_heartbeat();
		}

		if(receive_messages(&packet_in)) {
			switch (packet_in.msg_type) {
			case MSG_HEARTBEAT:
				first_heartbeat_received = true;
				last_received_heartbeat_time = cur_time;
				if(setup_state == STANDBY && parse_msg_heartbeat(&packet_in) == SYS_STATE_STANDBY) {
					setup_state = DONE;
				}
				break;
			case MSG_STATUS:
				printf("STATUS: %s\n", parse_msg_status(&packet_in).c_str());
				break;
			case MSG_ACK:
				switch(parse_msg_ack(&packet_in)) {	// Switch on the acknowledged packet type
				case MSG_MINEFIELD:	// Minefield msg was acknowledged, move on to dending mines
					setup_state = SEND_MINES;
					break;
				case MSG_MINE:	// Mine msg was acknowledged, either send another mine or move on if done
					num_mines_sent++;	// We've successfully sent a mine
					if (num_mines_sent == num_mines) {
						printf("All mine location sent\n");
						setup_state = STANDBY;
					}
					else {
						setup_state = SEND_MINES;
					}
					break;
				}
				break;
			case MSG_HOME:
				parse_msg_home(&packet_in, &home_lat, &home_lon);
				home_msg_received = true;
				break;
			}
		}
		
		if (first_heartbeat_received && (cur_time - last_received_heartbeat_time) > UNRESPONSIVE_TIME*CLOCKS_PER_SEC) {
			if(clock() - last_error_print_time > 1*CLOCKS_PER_SEC) {
				printf("ERROR: Drone has become unresponsive\n");
				last_error_print_time = clock();
			}
		}
	}

	printf("Setup complete\n");

	int state = 0;
	clock_t start_time = clock();
	// Main loop
	while (1) {
		// test code: 
		switch(state) {
			case 0:
				if((clock() - start_time) >= 2.0*CLOCKS_PER_SEC) {
					state = 1; // takeoff
				}
				break;
			case 1: // begin takeoff
				send_msg_takeoff();
				printf("Sending takeoff command to controller\n");
				state = 2;
				break;
			case 2:
				if(command_status == ACCEPTED && cmd_last_sent_type == MSG_TAKEOFF) {
					printf("Controller accepted takeoff command\n");
					state = 3;
				}
			case 3: // chill
				if((clock() - start_time) >= 7.0*CLOCKS_PER_SEC) {
					//state = 4;
				}
				break;
			case 4:
				send_msg_abort();
				printf("Aborting\n");
				state = 5;
				break;
			case 5:
				break;
		}
		// Send a heartbeat every second
		cur_time = clock();
		if ((cur_time - last_sent_heartbeat_time) >= HEARTBEAT_INTERVAL*CLOCKS_PER_SEC) {
			last_sent_heartbeat_time = cur_time;
			send_msg_heartbeat();
		}

		if(receive_messages(&packet_in)) {
			switch (packet_in.msg_type) {
			case MSG_HEARTBEAT:
				last_received_heartbeat_time = cur_time;
				break;
			case MSG_STATUS:
			{
				std::string str = parse_msg_status(&packet_in);
				printf("STATUS: %s\n", str.c_str());
				break;
			}
			case MSG_ACK:
				set_command_status();
				break;
			}
		}
		if (first_heartbeat_received && (cur_time - last_received_heartbeat_time) > UNRESPONSIVE_TIME*CLOCKS_PER_SEC) {
			if(clock() - last_error_print_time >= 1*CLOCKS_PER_SEC) {
				printf("ERROR: Drone has become unresponsive\n");
				last_error_print_time = clock();
			}
		}

		if(check_timeouts()) {
			printf("Resent #%d\n", cmd_last_sent_type);
		}
	}
	return 0;
}
