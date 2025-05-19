#include <Arduino.h>
#include <unity.h>

#include <room.hpp>

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_people_count_calculation_simple() {
    // simple test
    room_data test = {"ROOM", 0, 0, 1, 20, 0, 0};

    calculate_room_occupancy(&test);
    TEST_ASSERT_EQUAL(20, test.people_count);
}

void test_people_count_calculation_complex() {
    room_data test = {"ROOM", 0, 10, 2.5, 25, 0, 0};
    calculate_room_occupancy(&test);
    TEST_ASSERT_EQUAL(6, test.people_count);
}

void test_occupancy_simple() {
    room_data test = {"ROOM", 10, 0, 1, 10, 0, 0};
    calculate_room_occupancy(&test);
    TEST_ASSERT_EQUAL(1, test.occupancy);
}

void test_occupancy_complex() {
    room_data test = {"ROOM", 20, 20, 2, 50, 0, 0};
    calculate_room_occupancy(&test);
    TEST_ASSERT_EQUAL(0.75, test.occupancy);
}

void setup(void) {
    UNITY_BEGIN();
    RUN_TEST(test_people_count_calculation_simple);
    RUN_TEST(test_people_count_calculation_complex);
    RUN_TEST(test_occupancy_simple);
    RUN_TEST(test_occupancy_complex);
    UNITY_END();
}

void loop(void) {}