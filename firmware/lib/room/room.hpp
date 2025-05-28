#ifndef ROOM_HPP
#define ROOM_HPP

#include <stdio.h>

#define LEARNING_LAB (room_data){"Learning Lab", 40, 80, 2.5, 0, 0, 0}
#define C140 (room_data){"C140", 154, 4, 2, 0, 0, 0}

#define MAX_ROOM_NAME_LENGTH 100

typedef struct room_data {
    const char name[MAX_ROOM_NAME_LENGTH];
    const int room_capacity;
    int empty_device_count;
    float devices_per_person;
    //
    int device_count;
    float people_count;
    float occupancy;
} room_data;

float calculate_room_occupancy(room_data* room);
void room_data_to_json(char* dst, size_t max_size, room_data room);

#endif