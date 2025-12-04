#include "unity.h"
#include "rvc.h" // Controller_Reset() ����� ���� �ʿ�


// 1. Unit Tests (test_unit.c)
extern void test_UT01_Determine_Obstacle_Front(void);
extern void test_UT02_Determine_Obstacle_All(void);
extern void test_UT03_Determine_Dust_True(void);
extern void test_UT04_Determine_Dust_False(void);
extern void test_UT05_Controller_NoObstacle_Forward(void);
extern void test_UT06_Controller_FrontBlocked_TurnLeft(void);
extern void test_UT07_Controller_FrontLeftBlocked_TurnRight(void);
extern void test_UT08_Controller_AllBlocked_Backward(void);
extern void test_UT09_Controller_DustDetected_PowerUp(void);
extern void test_UT10_Controller_Backward_CleanerOff(void);

// 2. Integration Tests (test_integration.c)
extern void test_IT01_TurnLeft_Timer_Logic(void);
extern void test_IT02_TurnRight_Timer_Logic(void);
extern void test_IT03_Backward_Timer_Logic(void);
extern void test_IT04_PowerUp_Timer_Logic(void);
extern void test_IT05_Ignore_Sensor_While_Turning(void);
extern void test_IT06_Backward_Loop_Logic(void);
extern void test_IT07_Backward_To_TurnLeft(void);
extern void test_IT08_Motor_Cleaner_Concurrency(void);
extern void test_IT09_PowerUp_To_Off_By_Backward(void);
extern void test_IT10_PowerUp_Extension(void);

// 3. System Tests (test_system.c)
extern void test_ST01_Basic_Cruise(void);
extern void test_ST02_Simple_Avoidance(void);
extern void test_ST03_DeadEnd_Escape(void);
extern void test_ST04_Corner_Trap(void);
extern void test_ST05_Back_And_Turn_Left(void);
extern void test_ST06_Back_And_Turn_Right(void);
extern void test_ST07_Dusty_Path(void);
extern void test_ST08_ZigZag_Obstacles(void);
extern void test_ST09_Cleaning_Collision(void);
extern void test_ST10_Infinite_Trap(void);

// ==========================================
// [�߿�] Unity �ʼ� Hook �Լ�
// ��� �׽�Ʈ ���� ������ ���������� �����
// ==========================================
extern int tick_index; // Stub�� �ε��� �ʱ�ȭ�� ���� �ʿ�

void setUp(void)
{
    // �� �׽�Ʈ���� ��Ʈ�ѷ� ���¿� �α� �ε����� �ʱ�ȭ�ؾ�
    // ���� �׽�Ʈ�� ����(Side Effect)�� ���� �ʽ��ϴ�.
    Controller_Reset();
    tick_index = 0;
}

void tearDown(void)
{
    // �׽�Ʈ ���� �� ���� �۾� (����� �ʿ� ����)
}

// ==========================================
// ���� ���� �Լ�
// ==========================================
int main(void)
{
    UNITY_BEGIN(); // �׽�Ʈ ����

    // ------------------------------------
    // 1. Unit Test ����
    // ------------------------------------
    UnityPrint("--- Running Unit Tests ---");
    UNITY_PRINT_EOL();

    RUN_TEST(test_UT01_Determine_Obstacle_Front);
    RUN_TEST(test_UT02_Determine_Obstacle_All);
    RUN_TEST(test_UT03_Determine_Dust_True);
    RUN_TEST(test_UT04_Determine_Dust_False);
    RUN_TEST(test_UT05_Controller_NoObstacle_Forward);
    RUN_TEST(test_UT06_Controller_FrontBlocked_TurnLeft);
    RUN_TEST(test_UT07_Controller_FrontLeftBlocked_TurnRight);
    RUN_TEST(test_UT08_Controller_AllBlocked_Backward);
    RUN_TEST(test_UT09_Controller_DustDetected_PowerUp);
    RUN_TEST(test_UT10_Controller_Backward_CleanerOff);

    // ------------------------------------
    // 2. Integration Test ����
    // ------------------------------------
    UnityPrint("--- Running Integration Tests ---");
    UNITY_PRINT_EOL();

    RUN_TEST(test_IT01_TurnLeft_Timer_Logic);
    RUN_TEST(test_IT02_TurnRight_Timer_Logic);
    RUN_TEST(test_IT03_Backward_Timer_Logic);
    RUN_TEST(test_IT04_PowerUp_Timer_Logic);
    RUN_TEST(test_IT05_Ignore_Sensor_While_Turning);
    RUN_TEST(test_IT06_Backward_Loop_Logic);
    RUN_TEST(test_IT07_Backward_To_TurnLeft);
    RUN_TEST(test_IT08_Motor_Cleaner_Concurrency);
    RUN_TEST(test_IT09_PowerUp_To_Off_By_Backward);
    RUN_TEST(test_IT10_PowerUp_Extension);

    // ------------------------------------
    // 3. System Test ����
    // ------------------------------------
    UnityPrint("--- Running System Tests ---");
    UNITY_PRINT_EOL();

    RUN_TEST(test_ST01_Basic_Cruise);
    RUN_TEST(test_ST02_Simple_Avoidance);
    RUN_TEST(test_ST03_DeadEnd_Escape);
    RUN_TEST(test_ST04_Corner_Trap);
    RUN_TEST(test_ST05_Back_And_Turn_Left);
    RUN_TEST(test_ST06_Back_And_Turn_Right);
    RUN_TEST(test_ST07_Dusty_Path);
    RUN_TEST(test_ST08_ZigZag_Obstacles);
    RUN_TEST(test_ST09_Cleaning_Collision);
    RUN_TEST(test_ST10_Infinite_Trap);

    return UNITY_END(); // ��� ��� �� ����
}