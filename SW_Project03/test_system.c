#include "unity.h"
#include "rvc.h"

extern MotorCommand motor_log[1000];
extern int tick_index;

void RunScenario(Obstacle obs_seq[], bool dust_seq[], int ticks) {
    for (int i = 0; i < ticks; i++) {
        Controller(obs_seq[i], dust_seq[i]);
        tick_index++;
    }
}

//void setUp(void) {
//    Controller_Reset();
//    tick_index = 0;
//}
//void tearDown(void) {}

// ST-01: 기본 주행 (All Clear)
void test_ST01_Basic_Cruise(void) {
    Obstacle seq[5] = { {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0} };
    bool dust[5] = { 0 };
    RunScenario(seq, dust, 5);
    for (int i = 0; i < 5; i++) TEST_ASSERT_EQUAL(MOTOR_FORWARD, motor_log[i]);
}

// ST-02: 단순 장애물 회피 (Front -> Left -> Forward)
void test_ST02_Simple_Avoidance(void) {
    Obstacle seq[7] = {
        {1,0,0}, // 앞에 장애물 존재
        {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, //도는 동안 장애물 미감지
        {0,0,0}  //  이 순간부터 전진으로 돌아가야함 
    };
    bool dust[7] = { 0 };
    RunScenario(seq, dust, 7);
    TEST_ASSERT_EQUAL(MOTOR_TURN_LEFT, motor_log[0]);
    TEST_ASSERT_EQUAL(MOTOR_FORWARD, motor_log[6]);
}

// ST-03: 막다른 길 탈출 
void test_ST03_DeadEnd_Escape(void) {
    Obstacle seq[7] = {
        {1,1,0}, // 우회전 상황
        {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, // 도는 동안 장애물 미감지
        {0,0,0}  //  이 순간부터 전진으로 돌아가야함 
    };
    bool dust[7] = { 0 };
    RunScenario(seq, dust, 7);
    TEST_ASSERT_EQUAL(MOTOR_TURN_RIGHT, motor_log[0]);
    TEST_ASSERT_EQUAL(MOTOR_FORWARD, motor_log[6]);
}

// ST-04: 구석 갇힘 및 후진
void test_ST04_Corner_Trap(void) {
    Obstacle seq[5] = { {1,1,1}, {1,1,1}, {1,1,1}, {1,1,1}, {1,1,1} };
    bool dust[5] = { 0 };
    RunScenario(seq, dust, 5);
    TEST_ASSERT_EQUAL(MOTOR_BACKWARD, motor_log[4]);
}

// ST-05: 후진 후 좌회전 탈출 
void test_ST05_Back_And_Turn_Left(void) {
    Obstacle seq[12] = {
        {1,1,1}, {1,1,1}, {1,1,1}, {1,1,1}, {1,1,1}, 
        {0,0,1}, 
        {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
        {0,0,0}  
    };
    bool dust[12] = { 0 };
    RunScenario(seq, dust, 12);
    TEST_ASSERT_EQUAL(MOTOR_BACKWARD, motor_log[0]);
    TEST_ASSERT_EQUAL(MOTOR_TURN_LEFT, motor_log[5]);
    TEST_ASSERT_EQUAL(MOTOR_FORWARD, motor_log[11]);
}

// ST-06: 후진 후 우회전 탈출
void test_ST06_Back_And_Turn_Right(void) {
    Obstacle seq[12] = {
        {1,1,1}, {1,1,1}, {1,1,1}, {1,1,1}, {1,1,1}, 
        {1,1,0},
        {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
        {0,0,0} 
    };
    bool dust[12] = { 0 };
    RunScenario(seq, dust, 12);
    TEST_ASSERT_EQUAL(MOTOR_TURN_RIGHT, motor_log[5]);
    TEST_ASSERT_EQUAL(MOTOR_FORWARD, motor_log[11]);
}

// ST-07: 먼지 구간 통과 (Forward with Dust)
void test_ST07_Dusty_Path(void) {
    Obstacle seq[5] = { {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0} };
    bool dust[5] = { 1, 0, 0, 0, 0 }; // 먼지 발견
    RunScenario(seq, dust, 5);
    extern CleanerCommand cleaner_log[1000];
    TEST_ASSERT_EQUAL(CLEAN_POWERUP, cleaner_log[4]);
}

// ST-08: 지그재그 장애물 
void test_ST08_ZigZag_Obstacles(void) {
    Obstacle seq[13] = {
        {1,0,0}, // Left Turn (1~5)
        {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
        {0,0,0}, // Forward (6)
        {1,0,0}, // Left Turn (7~11)
        {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
        {0,0,0}  // Forward (12)
    };
    bool dust[13] = { 0 };
    RunScenario(seq, dust, 13);
    TEST_ASSERT_EQUAL(MOTOR_TURN_LEFT, motor_log[0]);
    TEST_ASSERT_EQUAL(MOTOR_FORWARD, motor_log[5]);
    TEST_ASSERT_EQUAL(MOTOR_TURN_LEFT, motor_log[6]);
    TEST_ASSERT_EQUAL(MOTOR_FORWARD, motor_log[12]);
}

// ST-09: 청소 중 장애물 충돌 
void test_ST09_Cleaning_Collision(void) {
    Obstacle seq[6] = {
        {0,0,0}, // Forward + Dust
        {1,0,0}, // Turn Left
        {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} 
    };
    bool dust[6] = { 1, 0, 0, 0, 0, 0 };
    RunScenario(seq, dust, 6);

    // 회전 할때는 클리너가 꺼지는지를 검증함
    TEST_ASSERT_EQUAL(MOTOR_TURN_LEFT, motor_log[1]);
    extern CleanerCommand cleaner_log[1000];
    TEST_ASSERT_EQUAL(CLEAN_OFF, cleaner_log[1]);
}

// ST-10: 갇힘 반복 (Infinite Trap Loop)
void test_ST10_Infinite_Trap(void) {
    Obstacle seq[11] = {
        {1,1,1}, {1,1,1}, {1,1,1}, {1,1,1}, {1,1,1}, // 1차 후진
        {0,1,1}, // 다시 확인해도 막힘 -> 2차 후진 시작
        {0,1,1}, {0,1,1}, {0,1,1}, {0,1,1}
    };
    bool dust[11] = { 0 };
    RunScenario(seq, dust, 11);

    // 계속 후진 중이어야 함
    TEST_ASSERT_EQUAL(MOTOR_BACKWARD, motor_log[0]);
    TEST_ASSERT_EQUAL(MOTOR_BACKWARD, motor_log[4]);
    TEST_ASSERT_EQUAL(MOTOR_BACKWARD, motor_log[9]);
}