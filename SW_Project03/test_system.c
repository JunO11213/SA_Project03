#include "unity.h"
#include "rvc.h"

// Stub���� ����ϴ� ����������
extern MotorCommand motor_log[1000];
extern CleanerCommand cleaner_log[1000];
extern int powerup_log[1000];
extern int tick_index;

//void setUp(void)
//{
//    Controller_Reset();
//    tick_index = 0;
//}
//
//void tearDown(void)
//{
//}

// =========================================
// Helper �ó����� ���� �Լ�
// =========================================
void RunScenario(Obstacle obs_seq[], bool dust_seq[], int ticks)
{
    for (int i = 0; i < ticks; i++)
    {
        Controller(obs_seq[i], dust_seq[i]);
        tick_index++;
    }
}

// =========================================
// SYSTEM TESTS (10��)
// =========================================

// ST01: ��ֹ� ���� �� 5 Tick ��� Forward
void test_ST01_AllClear_Forward(void)
{
    Obstacle seq[5] = {
        {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}
    };
    bool dust_seq[5] = { 0 };

    RunScenario(seq, dust_seq, 5);

    for (int i = 0; i < 5; i++)
        TEST_ASSERT_EQUAL(MOTOR_FORWARD, motor_log[i]);
}

// ST02: �տ� ��ֹ� �� �ٷ� TurnLeft
void test_ST02_ObstacleFront_TurnLeft(void)
{
    Obstacle seq[2] = {
        {1,0,0},  // TurnLeft�� ����
        {0,0,0}   // ���� Tick���� Forward ����
    };
    bool dust_seq[2] = { 0 };

    RunScenario(seq, dust_seq, 2);

    TEST_ASSERT_EQUAL(MOTOR_TURN_LEFT, motor_log[0]);
    TEST_ASSERT_EQUAL(MOTOR_FORWARD, motor_log[1]);
}

// ST03: F/L/R ��� ��ֹ� �� Backward ����
void test_ST03_AllBlocked_Backward5Ticks(void)
{
    Obstacle seq[5] = {
        {1,1,1},{1,1,1},{1,1,1},{1,1,1},{1,1,1}
    };
    bool dust_seq[5] = { 0 };

    RunScenario(seq, dust_seq, 5);

    for (int i = 0; i < 5; i++)
        TEST_ASSERT_EQUAL(MOTOR_BACKWARD, motor_log[i]);
}

// ST04: ���� 5 Tick �� ���� ���� �� TurnLeft
void test_ST04_BackwardThenTurnLeft(void)
{
    Obstacle seq[6] = {
        {1,1,1}, // backward ����
        {1,1,1},
        {1,1,1},
        {1,1,1},
        {1,1,1},
        {0,0,1}  // ���� ���� �� TurnLeft
    };
    bool dust_seq[6] = { 0 };

    RunScenario(seq, dust_seq, 6);

    TEST_ASSERT_EQUAL(MOTOR_TURN_LEFT, motor_log[5]);
}

// ST05: ������ ���� �� TurnRight
void test_ST05_ObstacleFrontRightOpen_TurnRight(void)
{
    Obstacle seq[2] = {
        {1,1,0},   // ��ȸ�� ����
        {0,0,0}
    };
    bool dust_seq[2] = { 0 };

    RunScenario(seq, dust_seq, 2);

    TEST_ASSERT_EQUAL(MOTOR_TURN_RIGHT, motor_log[0]);
    TEST_ASSERT_EQUAL(MOTOR_FORWARD, motor_log[1]);
}

// ST06: ���� ���� �� PowerUp 5 Tick ����
void test_ST06_DustPowerUpMaintain(void)
{
    Obstacle seq[5] = {
        {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}
    };
    bool dust_seq[5] = { 1,0,0,0,0 };  // ù Tick���� ���� ����

    RunScenario(seq, dust_seq, 5);

    TEST_ASSERT_EQUAL(CLEAN_POWERUP, cleaner_log[0]);
    TEST_ASSERT_EQUAL(CLEAN_POWERUP, cleaner_log[1]);
    TEST_ASSERT_EQUAL(CLEAN_POWERUP, cleaner_log[2]);
    TEST_ASSERT_EQUAL(CLEAN_POWERUP, cleaner_log[3]);
    TEST_ASSERT_EQUAL(CLEAN_POWERUP, cleaner_log[4]);
}

// ST07: ���� X �� Cleaner ON ����
void test_ST07_NoDust_CleanerON(void)
{
    Obstacle seq[5] = {
        {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}
    };
    bool dust_seq[5] = { 0 };

    RunScenario(seq, dust_seq, 5);

    for (int i = 0; i < 5; i++)
        TEST_ASSERT_EQUAL(CLEAN_ON, cleaner_log[i]);
}

// ST08: ��ֹ� �� �� ��ȸ�� �� ���� 3 Tick
void test_ST08_TurnLeftThenForwardSequence(void)
{
    Obstacle seq[4] = {
        {1,0,0},  // TurnLeft
        {0,0,0},  // Forward
        {0,0,0},  // Forward
        {0,0,0}   // Forward
    };
    bool dust_seq[4] = { 0 };

    RunScenario(seq, dust_seq, 4);

    TEST_ASSERT_EQUAL(MOTOR_TURN_LEFT, motor_log[0]);
    TEST_ASSERT_EQUAL(MOTOR_FORWARD, motor_log[1]);
    TEST_ASSERT_EQUAL(MOTOR_FORWARD, motor_log[2]);
    TEST_ASSERT_EQUAL(MOTOR_FORWARD, motor_log[3]);
}

// ST09: ���� �Ŀ��� �� ��ֹ� ȸ�� ���� �ó�����
void test_ST09_ComplexScenario(void)
{
    Obstacle seq[7] = {
        {0,0,0},  // FWD + Dust
        {1,0,0},  // TurnLeft
        {0,0,0},  // FWD
        {1,1,1},  // Backward
        {1,1,1},  // Backward
        {0,1,1},  // TurnLeft
        {0,0,0}   // FWD
    };
    bool dust_seq[7] = { 1,0,0,0,0,0,0 };

    RunScenario(seq, dust_seq, 7);

    TEST_ASSERT_EQUAL(MOTOR_FORWARD, motor_log[0]);
    TEST_ASSERT_EQUAL(MOTOR_TURN_LEFT, motor_log[1]);
    TEST_ASSERT_EQUAL(MOTOR_FORWARD, motor_log[2]);
    TEST_ASSERT_EQUAL(MOTOR_BACKWARD, motor_log[3]);
    TEST_ASSERT_EQUAL(MOTOR_BACKWARD, motor_log[4]);
    TEST_ASSERT_EQUAL(MOTOR_TURN_LEFT, motor_log[5]);
    TEST_ASSERT_EQUAL(MOTOR_FORWARD, motor_log[6]);
}

// ST10: ���� TurnRight �� ����
void test_ST10_TurnRightSequence(void)
{
    Obstacle seq[4] = {
        {1,1,0},  // TurnRight
        {1,1,0},  // TurnRight
        {0,0,0},  // Forward
        {0,0,0}   // Forward
    };
    bool dust_seq[4] = { 0 };

    RunScenario(seq, dust_seq, 4);

    TEST_ASSERT_EQUAL(MOTOR_TURN_RIGHT, motor_log[0]);
    TEST_ASSERT_EQUAL(MOTOR_TURN_RIGHT, motor_log[1]);
    TEST_ASSERT_EQUAL(MOTOR_FORWARD, motor_log[2]);
    TEST_ASSERT_EQUAL(MOTOR_FORWARD, motor_log[3]);
}
