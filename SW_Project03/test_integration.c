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

// IT-01좌회전 타이머(5Tick) 정확성 검증
void test_IT01_TurnLeft_Timer_Logic(void) {
    Obstacle trigger = { 1, 0, 0 };
    Obstacle clear = { 0, 0, 0 };

    Controller(trigger, false);
    tick_index++;// Tick 1 (Start)
    TEST_ASSERT_EQUAL(MOTOR_TURN_LEFT, motor_log[0]);

    for (int i = 1; i < 5; i++) { // Tick 2~5 동안 모터는 좌회전을 유지해야함
        Controller(clear, false);
        tick_index++;
        TEST_ASSERT_EQUAL(MOTOR_TURN_LEFT, motor_log[i]);
    }

    Controller(clear, false); // Tick 6이 되면 모터는 전진을 해야함
    tick_index++;
    TEST_ASSERT_EQUAL(MOTOR_FORWARD, motor_log[5]);
}

//IT-02 우회전 타이머(5Tick) 정확성 검증
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
} //좌회전 검증과 동일하게 작성함

// IT-03 후진 타이머(5Tick) 정확성 검증
void test_IT03_Backward_Timer_Logic(void) {
    Obstacle trigger = { 1, 1, 1 };
    Obstacle clear = { 0, 0, 0 };  //먼지가 계속 감지되지 않음

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
    //후진 후 좌회전에 우선순위가 있음을 검증함
}

// IT-04PowerUp 타이머(5Tick) 검증
void test_IT04_PowerUp_Timer_Logic(void) {
    Obstacle o = { 0, 0, 0 };

    Controller(o, true); //장애물은 없고 먼지가 감지된 상황 부여
    tick_index++;
    TEST_ASSERT_EQUAL(CLEAN_POWERUP, cleaner_log[0]);

    for (int i = 1; i < 5; i++) {
        Controller(o, false); // 장애물 없고, 먼지는 감지되지 않은 상황
        tick_index++;
        TEST_ASSERT_EQUAL(CLEAN_POWERUP, cleaner_log[i]);
        // 5 tick 동안 파워업 유지하는 FSM 검증
    }
    Controller(o, false);
    tick_index++;
    TEST_ASSERT_EQUAL(CLEAN_ON, cleaner_log[5]); //전진중 클리너온으로 복귀 
}

//IT-05 회전 중 센서 입력 무시 검증 (AtomicAction)
void test_IT05_Ignore_Sensor_While_Turning(void) {
    Obstacle trigger = { 1, 0, 0 };
    Obstacle noise = { 1, 1, 1 }; // 회전중 다른 센서입력이 들어와도 무시함 

    Controller(trigger, false); 
    tick_index++;

    
    Controller(noise, false);
    tick_index++;
    TEST_ASSERT_EQUAL(MOTOR_TURN_LEFT, motor_log[1]);
}

// IT-06 후진 반복 루프 동작 검증
void test_IT06_Backward_Loop_Logic(void) {
    Obstacle blocked = { 1, 1, 1 };

    // 막힌 상황에서 계속 후진 하는 상황 검증
    for (int i = 0; i < 5; i++) {
        Controller(blocked, false);
        tick_index++;
    }

    // 5 Tick 동안 후진함을 검증함 
    Controller(blocked, false);
    tick_index++;
    TEST_ASSERT_EQUAL(MOTOR_BACKWARD, motor_log[5]);
}

// IT-07 후진 후 좌측 개방 시 좌회전 전이 검증
void test_IT07_Backward_To_TurnLeft(void) {
    Obstacle blocked = { 1, 1, 1 };
    Obstacle left_open = { 0, 0, 1 }; 
   

    for (int i = 0; i < 5; i++) {
        Controller(blocked, false);
        tick_index++;
    }

    Controller(left_open, false);
    tick_index++;
    TEST_ASSERT_EQUAL(MOTOR_TURN_LEFT, motor_log[5]);
}

// IT-08 모터 회전 중 청소기 동시 동작 차단 검증
void test_IT08_Motor_Cleaner_Concurrency(void) {
    Obstacle turn = { 1, 0, 0 }; //회전을 해야하는 상황
    bool dust = true;          

    Controller(turn, dust);
    tick_index++;

    TEST_ASSERT_EQUAL(MOTOR_TURN_LEFT, motor_log[0]);
    TEST_ASSERT_EQUAL(CLEAN_OFF, cleaner_log[0]); // 회전 중에는 클리너 오프 검증
}

// IT-09PowerUp 상태에서 후진 발생 시 청소기 OFF 전환 검증
void test_IT09_PowerUp_To_Off_By_Backward(void) {
    Controller((Obstacle) { 0, 0, 0 }, true);
    tick_index++;
    TEST_ASSERT_EQUAL(CLEAN_POWERUP, cleaner_log[0]);

    Controller((Obstacle) { 1, 1, 1 }, false);
    tick_index++;
    TEST_ASSERT_EQUAL(MOTOR_BACKWARD, motor_log[1]);
    TEST_ASSERT_EQUAL(CLEAN_OFF, cleaner_log[1]);
}

// IT-10 연속 먼지 감지 시 PowerUp 타이머 연장 검증
void test_IT10_PowerUp_Extension(void) {
    Obstacle o = { 0,0,0 };

    Controller(o, true);
    tick_index++;
    Controller(o, false);
    tick_index++;// Tick 2에서는 먼지가 감지되지 않음 
    Controller(o, true);
    tick_index++;// Tick 3에서 먼지가 다시 감지됨, 타이머 리셋

    // Tick 3 부터 5 tick동안 파워업이 유지 되어야 함 
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL(CLEAN_POWERUP, cleaner_log[2 + i]);
        Controller(o, false);
        tick_index++;
    }
}