#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#include <room.hpp>

float calculate_room_occupancy(room_data* room) {
    float people_count =
        MAX(room->device_count - room->empty_device_count, 0) / room->devices_per_person;
    float occupancy = (room->room_capacity == 0) ? 0 : people_count / (float)room->room_capacity;
    room->people_count = people_count;
    room->occupancy = occupancy;
    return people_count;
}

void room_data_to_json(char* dst, size_t max_size, room_data room) {
    static char json_room_format[] =
        "{\n"
        "\t\"room\": \"%s\",\n"
        "\t\"room_capacity\": %d,\n"
        "\t\"room_empty_device_count\": %d,\n"
        "\t\"room_device_per_person\": %f,\n"
        "\t\"device_count\": %d,\n"
        "\t\"people_count\": %f,\n"
        "\t\"occupancy\": %f\n"
        "}";
    snprintf(dst, max_size, json_room_format, room.name, room.room_capacity,
             room.empty_device_count, room.devices_per_person, room.device_count, room.people_count,
             room.occupancy);
    return;
}