#pragma once
#ifndef ROBOTSTATE_H
#define ROBOTSTATE_H

class RobotState
{
public:
    RobotState(void);
    ~RobotState(void);

public:
    // velocity
    float vx;
    float vy;
    float theta;
    float frequency;

    // head
    float headYaw;
    float headPitch;

};

#endif //ROBOTSTATE_H
