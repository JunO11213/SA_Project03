#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include <stdlib.h>
#include "rvc.h"

#define ACTION_DURATION_TICK     5
#define POWERUP_MAINTAIN_TICK    5
#define BACKWARD_SPEED           0.2
#define MAP_SIZE_X               50.0
#define MAP_SIZE_Y               50.0


typedef enum {
    ST_MOVE_FORWARD = 0,
    ST_MOVE_BACKWARD,
    ST_TURN_LEFT,
    ST_TURN_RIGHT
} State;

static State current_state = ST_MOVE_FORWARD;
static RobotState robot = { 0.0, 0.0, NORTH };
static int no_dust_timer = POWERUP_MAINTAIN_TICK;
static int action_timer = 0;

// 이전 과제에서 하드웨어를 대신해 출력으로 실행을 검증하기 위한 입력값 인터페이스, 테스팅에서는 미사용함
bool FrontSensor_Interface(void)
{
    return (GetAsyncKeyState(VK_UP) & 0x8000) != 0;
}

bool LeftSensor_Interface(void)
{
    return (GetAsyncKeyState(VK_LEFT) & 0x8000) != 0;
}

bool RightSensor_Interface(void)
{
    return (GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0;
}

bool DustSensor_Interface(void)
{
    return (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
}

Obstacle Determine_Obstacle_Location(bool front, bool left, bool right)
{
    Obstacle o = { front, left, right };
    return o;
}

bool Determine_Dust_Existence(bool dust)
{
    return dust;
}

// Motor & Cleaner 인터페이스
// 테스팅을 진행하는 과정에서는  Stub이 대신하게 된다
#ifndef USE_STUB
void Motor_Interface(MotorCommand cmd, RobotState state)
{
    const char* dir_name[] = { "North", "East", "South", "West" };

    switch (cmd) {
    case MOTOR_FORWARD:
        printf("[Motor] Forward     (%5.1f, %5.1f) %s ",
            state.x, state.y, dir_name[state.heading]);
        break;

    case MOTOR_BACKWARD:
        printf("[Motor] Backward    (%5.1f, %5.1f) %s ",
            state.x, state.y, dir_name[state.heading]);
        break;

    case MOTOR_TURN_LEFT:
        printf("[Motor] Turn Left   (%5.1f, %5.1f) %s ",
            state.x, state.y, dir_name[state.heading]);
        break;

    case MOTOR_TURN_RIGHT:
        printf("[Motor] Turn Right  (%5.1f, %5.1f) %s ",
            state.x, state.y, dir_name[state.heading]);
        break;
    }
}
#endif

#ifndef USE_STUB
void Cleaner_Interface(CleanerCommand cmd, int cnt)
{
    switch (cmd) {
    case CLEAN_OFF:
        printf("| [Cleaner] OFF\n");
        break;
    case CLEAN_ON:
        printf("| [Cleaner] ON\n");
        break;
    case CLEAN_POWERUP:
        printf("| [Cleaner] POWER UP (%d)\n", cnt);
        break;
    }
}
#endif

// Controller (FSM)
void Controller(Obstacle obs, bool dust)
{
    MotorCommand motor_cmd = MOTOR_FORWARD;
    CleanerCommand cleaner_cmd = CLEAN_OFF;
    int display_powerup = 0;

    switch (current_state)
    {
    case ST_MOVE_FORWARD:
        if (obs.IsFront && obs.IsLeft && obs.IsRight) {
            current_state = ST_MOVE_BACKWARD;
            action_timer = ACTION_DURATION_TICK;
        }
        else if (obs.IsFront && !obs.IsLeft) {
            current_state = ST_TURN_LEFT;
            action_timer = ACTION_DURATION_TICK;
        }
        else if (obs.IsFront && obs.IsLeft && !obs.IsRight) {
            current_state = ST_TURN_RIGHT;
            action_timer = ACTION_DURATION_TICK;
        }
        break;

    case ST_MOVE_BACKWARD:
        action_timer--;
        if (action_timer <= 0) {
            if (obs.IsLeft && obs.IsRight) {
                current_state = ST_MOVE_BACKWARD;
                action_timer = ACTION_DURATION_TICK;
            }
            else if (!obs.IsLeft) {
                current_state = ST_TURN_LEFT;
                action_timer = ACTION_DURATION_TICK;
            }
            else if (obs.IsLeft && !obs.IsRight) {
                current_state = ST_TURN_RIGHT;
                action_timer = ACTION_DURATION_TICK;
            }
            else {
                current_state = ST_MOVE_FORWARD;
            }
        }
        break;

    case ST_TURN_LEFT:
        action_timer--;
        if (action_timer <= 0) {
            robot.heading = (robot.heading + 3) % 4;
            current_state = ST_MOVE_FORWARD;
        }
        break;

    case ST_TURN_RIGHT:
        action_timer--;
        if (action_timer <= 0) {
            robot.heading = (robot.heading + 1) % 4;
            current_state = ST_MOVE_FORWARD;
        }
        break;
    }

    //이동 처리 
    double next_x = robot.x;
    double next_y = robot.y;
    bool moving = false;

    switch (current_state)
    {
    case ST_MOVE_FORWARD:
        motor_cmd = MOTOR_FORWARD;
        if (robot.heading == NORTH) next_y++;
        else if (robot.heading == EAST) next_x++;
        else if (robot.heading == SOUTH) next_y--;
        else if (robot.heading == WEST) next_x--;
        moving = true;
        break;

    case ST_MOVE_BACKWARD:
        motor_cmd = MOTOR_BACKWARD;
        if (robot.heading == NORTH) next_y -= BACKWARD_SPEED;
        else if (robot.heading == EAST) next_x -= BACKWARD_SPEED;
        else if (robot.heading == SOUTH) next_y += BACKWARD_SPEED;
        else if (robot.heading == WEST) next_x += BACKWARD_SPEED;
        moving = true;
        break;

    case ST_TURN_LEFT:
        motor_cmd = MOTOR_TURN_LEFT;
        break;

    case ST_TURN_RIGHT:
        motor_cmd = MOTOR_TURN_RIGHT;
        break;
    }

    if (moving) {
        if (next_x < -MAP_SIZE_X || next_x > MAP_SIZE_X ||
            next_y < -MAP_SIZE_Y || next_y > MAP_SIZE_Y)
        {
            exit(0);
        }
        robot.x = next_x;
        robot.y = next_y;
    }

    
    if (current_state == ST_MOVE_FORWARD) {
        if (dust) {
            cleaner_cmd = CLEAN_POWERUP;
            no_dust_timer = 1;
            display_powerup = POWERUP_MAINTAIN_TICK;
        }
        else {
            if (no_dust_timer < POWERUP_MAINTAIN_TICK) {
                no_dust_timer++;
                display_powerup = POWERUP_MAINTAIN_TICK - no_dust_timer + 1;
                cleaner_cmd = CLEAN_POWERUP;
            }
            else {
                cleaner_cmd = CLEAN_ON;
            }
        }
    }
    else {
        cleaner_cmd = CLEAN_OFF;
        no_dust_timer = POWERUP_MAINTAIN_TICK;
    }

    Motor_Interface(motor_cmd, robot);
    Cleaner_Interface(cleaner_cmd, display_powerup);
}

// 테스트마다 결과가 독립적으로 도출되도록 매번 컨트롤러를 초기화 하는 함수를 추가함
void Controller_Reset(void)
{
    current_state = ST_MOVE_FORWARD;
    robot.x = 0;
    robot.y = 0;
    robot.heading = NORTH;
    no_dust_timer = POWERUP_MAINTAIN_TICK;
    action_timer = 0;
}


// 실제 RVC 실행 (테스트 빌드에서는 제외)
#ifdef BUILD_RVC_APP
int main(void)
{
    while (1)
    {
        bool front = FrontSensor_Interface();
        bool left = LeftSensor_Interface();
        bool right = RightSensor_Interface();
        bool dust = DustSensor_Interface();

        Obstacle obs = Determine_Obstacle_Location(front, left, right);
        bool dust_ex = Determine_Dust_Existence(dust);

        Controller(obs, dust_ex);
        Sleep(1000);
    }
    return 0;
}
#endif
