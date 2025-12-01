#include "unity.h"
#include "rvc.h"

extern MotorCommand motor_log[1000];
extern CleanerCommand cleaner_log[1000];
extern int tick_index;

//void setUp(void) {
//    Controller_Reset();
//    tick_index = 0;
//}
//void tearDown(void) {}

// UT-01: 장애물 위치 구조체 생성 로직 (Front)
void test_UT01_Determine_Obstacle_Front(void) {
    Obstacle o = Determine_Obstacle_Location(true, false, false);
    TEST_ASSERT_TRUE(o.IsFront);
    TEST_ASSERT_FALSE(o.IsLeft);
    TEST_ASSERT_FALSE(o.IsRight);
}

// UT-02: 장애물 위치 구조체 생성 로직 (All)
void test_UT02_Determine_Obstacle_All(void) {
    Obstacle o = Determine_Obstacle_Location(true, true, true);
    TEST_ASSERT_TRUE(o.IsFront);
    TEST_ASSERT_TRUE(o.IsLeft);
    TEST_ASSERT_TRUE(o.IsRight);
}

// UT-03: 먼지 존재 여부 반환 로직
void test_UT03_Determine_Dust_True(void) {
    TEST_ASSERT_TRUE(Determine_Dust_Existence(true));
}

// UT-04: 먼지 미존재 여부 반환 로직
void test_UT04_Determine_Dust_False(void) {
    TEST_ASSERT_FALSE(Determine_Dust_Existence(false));
}

// UT-05: [Controller] 장애물 없음 -> 전진 명령 확인
void test_UT05_Controller_NoObstacle_Forward(void) {
    Obstacle o = { 0, 0, 0 };
    Controller(o, false);
    TEST_ASSERT_EQUAL(MOTOR_FORWARD, motor_log[0]);
}

// UT-06: [Controller] 앞 장애물 감지 -> 좌회전 전이 확인
void test_UT06_Controller_FrontBlocked_TurnLeft(void) {
    Obstacle o = { 1, 0, 0 };
    Controller(o, false);
    TEST_ASSERT_EQUAL(MOTOR_TURN_LEFT, motor_log[0]);
}

// UT-07: [Controller] 앞+왼쪽 장애물 -> 우회전 전이 확인 (우선순위)
void test_UT07_Controller_FrontLeftBlocked_TurnRight(void) {
    Obstacle o = { 1, 1, 0 };
    Controller(o, false);
    TEST_ASSERT_EQUAL(MOTOR_TURN_RIGHT, motor_log[0]);
}

// UT-08: [Controller] 3면 막힘 -> 후진 전이 확인
void test_UT08_Controller_AllBlocked_Backward(void) {
    Obstacle o = { 1, 1, 1 };
    Controller(o, false);
    TEST_ASSERT_EQUAL(MOTOR_BACKWARD, motor_log[0]);
}

// UT-09: [Controller] 전진 중 먼지 감지 -> PowerUp 확인
void test_UT09_Controller_DustDetected_PowerUp(void) {
    Obstacle o = { 0, 0, 0 };
    Controller(o, true); // 먼지 있음
    TEST_ASSERT_EQUAL(CLEAN_POWERUP, cleaner_log[0]);
}

// UT-10: [Controller] 전진 중이 아닐 때(후진) -> Cleaner OFF 확인
void test_UT10_Controller_Backward_CleanerOff(void) {
    // 3면이 막혀서 후진해야 하는 상황
    Obstacle o = { 1, 1, 1 };
    Controller(o, true); // 먼지가 있어도
    TEST_ASSERT_EQUAL(MOTOR_BACKWARD, motor_log[0]);
    TEST_ASSERT_EQUAL(CLEAN_OFF, cleaner_log[0]); // 후진 중엔 꺼져야 함
}