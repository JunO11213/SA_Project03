#ifndef RVC_H
#define RVC_H

#include <stdbool.h>

// ==========================
// Data Structures
// ==========================

// Sensor �� Controller Obstacle Information
typedef struct {
    bool IsFront;
    bool IsLeft;
    bool IsRight;
} Obstacle;

// Motor Commands
typedef enum {
    MOTOR_FORWARD = 0,
    MOTOR_BACKWARD,
    MOTOR_TURN_LEFT,
    MOTOR_TURN_RIGHT
} MotorCommand;

// Cleaner Commands
typedef enum {
    CLEAN_OFF = 0,
    CLEAN_ON,
    CLEAN_POWERUP
} CleanerCommand;

// Heading Direction
typedef enum {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3
} Direction;

// Robot Position & Direction
typedef struct {
    double x;
    double y;
    Direction heading;
} RobotState;

// ==========================
// External Interface Functions
// ==========================
Obstacle Determine_Obstacle_Location(bool front, bool left, bool right);
bool Determine_Dust_Existence(bool dust);

void Motor_Interface(MotorCommand cmd, RobotState state);
void Cleaner_Interface(CleanerCommand cmd, int cnt);

void Controller(Obstacle obs, bool dust);
void Controller_Reset(void);

#endif
