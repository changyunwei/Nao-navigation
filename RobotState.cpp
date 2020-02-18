#include "stdafx.h"
#include "RobotState.h"

RobotState::RobotState(void)
   : vx(0),
     vy(0),
     theta(0),
     frequency(0),
     headYaw(0),
     headPitch(0)
{
}

RobotState::~RobotState(void)
{
}
