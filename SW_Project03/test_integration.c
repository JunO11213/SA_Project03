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

// IT-01: 좌회전 타이머 (5 Tick) 정확성 검증
void test_IT01_TurnLeft_Timer_Logic(void) {
    Obstacle trigger = { 1, 0, 0 };
    Obstacle clear = { 0, 0, 0 };

    Controller(trigger, false);
    tick_index++;// Tick 1 (Start)
    TEST_ASSERT_EQUAL(MOTOR_TURN_LEFT, motor_log[0]);

    for (int i = 1; i < 5; i++) { // Tick 2~5 (Maintain)
        Controller(clear, false);
        tick_index++;
        TEST_ASSERT_EQUAL(MOTOR_TURN_LEFT, motor_log[i]);
    }

    Controller(clear, false); // Tick 6 (Finish)
    tick_index++;
    TEST_ASSERT_EQUAL(MOTOR_FORWARD, motor_log[5]);
}

// IT-02: 우회전 타이머 (5 Tick) 정확성 검증
void test_IT02_TurnRight_Timer_Logic(void) {
    Obstacle trigger = { 1, 1, 0 };
    Obstacle clear = { 0, 0, 0 };

    Controller(trigger, false);
    tick_index++;
    TEST_ASSERT_EQUAL(MOTOR_TURN_RIGHT, motor_log[0]);

    for (int i = 1; i < 5; i++) {
        Controller(clear, false);
        tick_index++;
        TEST_ASSERT_EQUAL(MOTOR_TURN_RIGHT, motor_log[i]);
    }

    Controller(clear, false);
    tick_index++;
    TEST_ASSERT_EQUAL(MOTOR_FORWARD, motor_log[5]);
}

// IT-03: 후진 타이머 (5 Tick) 정확성 검증
void test_IT03_Backward_Timer_Logic(void) {
    Obstacle trigger = { 1, 1, 1 };
    Obstacle clear = { 0, 0, 0 }; // 후진 중에는 센서값 무시됨을 확인

    Controller(trigger, false);
    tick_index++;
    TEST_ASSERT_EQUAL(MOTOR_BACKWARD, motor_log[0]);

    for (int i = 1; i < 5; i++) {
        Controller(clear, false);
        tick_index++;
        TEST_ASSERT_EQUAL(MOTOR_BACKWARD, motor_log[i]);
    }
    Controller(clear, false);
    tick_index++;
    TEST_ASSERT_EQUAL(MOTOR_TURN_LEFT, motor_log[5]);
}

// IT-04: 먼지 PowerUp 타이머 (5 Tick) 검증
void test_IT04_PowerUp_Timer_Logic(void) {
    Obstacle o = { 0, 0, 0 };

    Controller(o, true); // 먼지 발견
    tick_index++;
    TEST_ASSERT_EQUAL(CLEAN_POWERUP, cleaner_log[0]);

    for (int i = 1; i < 5; i++) {
        Controller(o, false); // 먼지 사라짐
        tick_index++;
        TEST_ASSERT_EQUAL(CLEAN_POWERUP, cleaner_log[i]);
    }
    Controller(o, false);
    tick_index++;
    TEST_ASSERT_EQUAL(CLEAN_ON, cleaner_log[5]); // 복귀
}

// IT-05: 회전 중 장애물 무시 검증 (Action Atomic property)
void test_IT05_Ignore_Sensor_While_Turning(void) {
    Obstacle trigger = { 1, 0, 0 };
    Obstacle noise = { 1, 1, 1 }; // 회전 중에 갑자기 사방이 막힘

    Controller(trigger, false); // 좌회전 시작
    tick_index++;

    // 회전 중(Tick 2)에 장애물 입력이 들어와도 회전 상태 유지해야 함
    Controller(noise, false);
    tick_index++;
    TEST_ASSERT_EQUAL(MOTOR_TURN_LEFT, motor_log[1]);
}

// IT-06: 후진 후 다시 막혔을 때 재후진 검증
void test_IT06_Backward_Loop_Logic(void) {
    Obstacle blocked = { 1, 1, 1 };

    // 1회차 후진 (5 Tick)
    for (int i = 0; i < 5; i++) {
        Controller(blocked, false);
        tick_index++;
    }

    // 5 Tick 종료 후 다시 센서 확인 -> 여전히 막힘 -> 다시 후진 시작
    Controller(blocked, false);
    tick_index++;
    TEST_ASSERT_EQUAL(MOTOR_BACKWARD, motor_log[5]);
}

// IT-07: 후진 후 왼쪽 열림 -> 좌회전 전이 검증
void test_IT07_Backward_To_TurnLeft(void) {
    Obstacle blocked = { 1, 1, 1 };
    Obstacle left_open = { 0, 0, 1 }; // Front(X) Left(O) Right(X) -> ?? 
    // 코드 로직상: if (!obs.IsLeft) -> TurnLeft
    // {0,0,1}은 Front=0, Left=0, Right=1 이므로 IsLeft=false -> TurnLeft

    for (int i = 0; i < 5; i++) {
        Controller(blocked, false);
        tick_index++;
    }

    Controller(left_open, false);
    tick_index++;
    TEST_ASSERT_EQUAL(MOTOR_TURN_LEFT, motor_log[5]);
}

// IT-08: 모터(회전)중에는 클리너가 꺼져야하는 조건을 검증함
void test_IT08_Motor_Cleaner_Concurrency(void) {
    Obstacle turn = { 1, 0, 0 }; // 좌회전 조건
    bool dust = true;          

    Controller(turn, dust);
    tick_index++;

    TEST_ASSERT_EQUAL(MOTOR_TURN_LEFT, motor_log[0]);
    TEST_ASSERT_EQUAL(CLEAN_OFF, cleaner_log[0]);
}

// IT-09: 파워업 중 후진 발생 시 청소기 OFF 전환 검증
void test_IT09_PowerUp_To_Off_By_Backward(void) {
    // 1. 먼지 발견 (PowerUp)
    Controller((Obstacle) { 0, 0, 0 }, true);
    tick_index++;
    TEST_ASSERT_EQUAL(CLEAN_POWERUP, cleaner_log[0]);

    // 2. 갑자기 갇힘 (Backward) -> 청소기 꺼져야 함
    Controller((Obstacle) { 1, 1, 1 }, false);
    tick_index++;
    TEST_ASSERT_EQUAL(MOTOR_BACKWARD, motor_log[1]);
    TEST_ASSERT_EQUAL(CLEAN_OFF, cleaner_log[1]);
}

// IT-10: 연속 먼지 감지 시 타이머 연장 검증
void test_IT10_PowerUp_Extension(void) {
    Obstacle o = { 0,0,0 };

    Controller(o, true);
    tick_index++;// Tick 1 (Timer=1)
    Controller(o, false);
    tick_index++;// Tick 2 (Timer=2)
    Controller(o, true);
    tick_index++;// Tick 3 (먼지 재발견 -> Timer=1 리셋되어야 함)

    // 재발견 시점부터 다시 5 Tick 동안 PowerUp이어야 함
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL(CLEAN_POWERUP, cleaner_log[2 + i]);
        Controller(o, false);
        tick_index++;
    }
}