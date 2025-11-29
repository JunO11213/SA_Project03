#include "rvc.h"

MotorCommand motor_log[1000];
CleanerCommand cleaner_log[1000];
int powerup_log[1000];
int tick_index = 0;

void Motor_Interface(MotorCommand cmd, RobotState state)
{
    motor_log[tick_index] = cmd;
}

void Cleaner_Interface(CleanerCommand cmd, int cnt)
{
    cleaner_log[tick_index] = cmd;
    powerup_log[tick_index] = cnt;
}
