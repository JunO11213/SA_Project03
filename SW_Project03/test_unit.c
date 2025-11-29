#include "unity.h"
#include "rvc.h"

// Stub���� ����ϴ� �ܺ� ����(�α� ����)
extern MotorCommand motor_log[1000];
extern CleanerCommand cleaner_log[1000];
extern int powerup_log[1000];
extern int tick_index;

// Controller ���� �ʱ�ȭ �Լ�
//void setUp(void)
//{
//    Controller_Reset();
//    tick_index = 0;
//}
//
//void tearDown(void)
//{
//}

// =======================================================
//  UT01: Determine_Obstacle_Location ���� ���� ����
// =======================================================
void test_UT01_DetermineObstacle_Basic(void)
{
    Obstacle o = Determine_Obstacle_Location(1, 0, 1);
    TEST_ASSERT_EQUAL(true, o.IsFront);
    TEST_ASSERT_EQUAL(false, o.IsLeft);
    TEST_ASSERT_EQUAL(true, o.IsRight);
}

// =======================================================
//  UT02: Determine_Dust_Existence ���� �Է� Ȯ��
// =======================================================
void test_UT02_DetermineDust(void)
{
    TEST_ASSERT_EQUAL(true, Determine_Dust_Existence(true));
    TEST_ASSERT_EQUAL(false, Determine_Dust_Existence(false));
}

// =======================================================
//  UT03: ��ֹ� ���� �� Controller�� Forward ����
// =======================================================
void test_UT03_Controller_NoObstacle_Forward(void)
{
    Obstacle o = { 0,0,0 };
    Controller(o, 0);

    TEST_ASSERT_EQUAL(MOTOR_FORWARD, motor_log[0]);
}

// =======================================================
//  UT04: �տ� ��ֹ��� �� TurnLeft ����
// =======================================================
void test_UT04_Controller_FrontOnly_TurnLeft(void)
{
    Obstacle o = { 1,0,0 };
    Controller(o, 0);

    TEST_ASSERT_EQUAL(MOTOR_TURN_LEFT, motor_log[0]);
}

// =======================================================
//  UT05: �� + ���� �� TurnRight ����
// =======================================================
void test_UT05_Controller_FrontLeft_TurnRight(void)
{
    Obstacle o = { 1,1,0 };
    Controller(o, 0);

    TEST_ASSERT_EQUAL(MOTOR_TURN_RIGHT, motor_log[0]);
}

// =======================================================
//  UT06: ��/��/�� ��� ���� �� Backward ����
// =======================================================
void test_UT06_Controller_AllBlocked_Backward(void)
{
    Obstacle o = { 1,1,1 };
    Controller(o, 0);

    TEST_ASSERT_EQUAL(MOTOR_BACKWARD, motor_log[0]);
}

// =======================================================
//  UT07: ���� ���� �� CLEAN_POWERUP
// =======================================================
void test_UT07_Controller_DustDetected_PowerUp(void)
{
    Obstacle o = { 0,0,0 };

    Controller(o, 1);  // ���� ����

    TEST_ASSERT_EQUAL(CLEAN_POWERUP, cleaner_log[0]);
}

// =======================================================
//  UT08: ���� ���� �� CLEAN_ON ����
// =======================================================
void test_UT08_Controller_NoDust_CleanerON(void)
{
    Obstacle o = { 0,0,0 };

    // 5 tick ��� dust=0 �� powerup ���� �� �� �� CLEAN_ON
    for (int i = 0; i < 5; i++)
    {
        Controller(o, 0);
        tick_index++;
    }

    TEST_ASSERT_EQUAL(CLEAN_ON, cleaner_log[4]);
}

