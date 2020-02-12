#ifndef PATHPLAN_H
#define PATHPLAN_H

#include <stdint.h>
#include <math.h>

#define PI 3.14159265358979311599796346854
#define EARTH_RADIUS 6371000.0 // 6371 km
#define MINES_PER_RUN 6
#define MAX_NUM_MINES 256   // can be up to 2^16-1
#define deg_to_rad(deg) (deg) * PI/180.0
#define rad_to_deg(rad) (rad)*180.0/PI

// Distance to travel when escaping a detonation
#define ESCAPE_DIST 10  // 10 meters
#define ANGULAR_ESCAPE_DIST ESCAPE_DIST/EARTH_RADIUS

// Mine datatype
struct mine_t {
    int32_t lat;
    int32_t lon;
    int32_t escape_lat;
    int32_t escape_lon;
};
#define MINE_T_SIZE sizeof(mine_t)

// Node datatype for linked list
struct node_t {
    mine_t* mine;
    struct node_t* next;
};
#define NODE_T_SIZE sizeof(node_t)

// Structured mine array
extern mine_t mines[MAX_NUM_MINES];
extern uint16_t mines_index;

void add_mine(node_t**, int32_t, int32_t);
void LL_add(node_t**, mine_t*);
void LL_remove(node_t**, mine_t*);
bool mine_t_equals(mine_t, mine_t);
mine_t* closest_to(int32_t, int32_t, node_t*);
double dist_to(int32_t, int32_t, int32_t, int32_t);
double heading_to(int32_t, int32_t, int32_t, int32_t);
void get_escape_point(int32_t cur_lat, int32_t cur_lon, int32_t next_lat, int32_t next_lon, int32_t* target_lat_ptr, int32_t* target_lon_ptr);
void plan_path(int32_t, int32_t, node_t**);

#endif
