#include "unity.h"

// ===== System Test  =====
void test_ST01_AllClear_Forward(void);
void test_ST02_ObstacleFront_TurnLeft(void);
void test_ST03_AllBlocked_Backward5Ticks(void);
void test_ST04_BackwardThenTurnLeft(void);
void test_ST05_ObstacleFrontRightOpen_TurnRight(void);
void test_ST06_DustPowerUpMaintain(void);
void test_ST07_NoDust_CleanerON(void);
void test_ST08_TurnLeftThenForwardSequence(void);
void test_ST09_ComplexScenario(void);
void test_ST10_TurnRightSequence(void);

// ===== Unit Test  =====
void test_UT01_DetermineObstacle_Basic(void);

// ===== Unity Hooks =====
void setUp(void) {}
void tearDown(void) {}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_ST01_AllClear_Forward);
    RUN_TEST(test_ST02_ObstacleFront_TurnLeft);
    RUN_TEST(test_ST03_AllBlocked_Backward5Ticks);
    RUN_TEST(test_ST04_BackwardThenTurnLeft);
    RUN_TEST(test_ST05_ObstacleFrontRightOpen_TurnRight);
    RUN_TEST(test_ST06_DustPowerUpMaintain);
    RUN_TEST(test_ST07_NoDust_CleanerON);
    RUN_TEST(test_ST08_TurnLeftThenForwardSequence);
    RUN_TEST(test_ST09_ComplexScenario);
    RUN_TEST(test_ST10_TurnRightSequence);
    printf("\n\n");
    RUN_TEST(test_UT01_DetermineObstacle_Basic);
}