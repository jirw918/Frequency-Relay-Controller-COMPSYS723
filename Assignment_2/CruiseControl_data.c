#include "CruiseControl.h"
#include <stdio.h>
#include <stdbool.h>

/* ============================================================
   PI Regulation (from cruiseregulation.c)
   ============================================================ */

static const float THROTTLESATMAX = 45.0f;
static const float KP = 8.113f;
static const float KI = 0.5f;

static float saturateThrottle(float throttleIn, bool *saturate)
{
    if (throttleIn > THROTTLESATMAX) {
        *saturate = true;
        return THROTTLESATMAX;
    } else if (throttleIn < 0.0f) {
        *saturate = true;
        return 0.0f;
    } else {
        *saturate = false;
        return throttleIn;
    }
}

float regulateThrottle(bool isGoingOn, float cruiseSpeed, float vehicleSpeed)
{
    static bool saturate = true;
    static float iterm = 0.0f;

    if (isGoingOn) {
        iterm = 0.0f;
        saturate = true;
    }
    float error = cruiseSpeed - vehicleSpeed;
    float proportionalAction = error * KP;
    if (saturate)
        error = 0.0f;
    iterm = iterm + error;
    float integralAction = KI * iterm;
    return saturateThrottle(proportionalAction + integralAction, &saturate);
}

/* ============================================================
   Helper predicates and speed management
   ============================================================ */

bool isAccelPressed(float accel)
{
    return accel > PEDALS_MIN;
}

bool isBrakePressed(float brake)
{
    return brake > PEDALS_MIN;
}

bool isSpeedInRange(float speed)
{
    return speed >= SPEED_MIN && speed <= SPEED_MAX;
}

static float clampSpeed(float speed)
{
    if (speed < SPEED_MIN) return SPEED_MIN;
    if (speed > SPEED_MAX) return SPEED_MAX;
    return speed;
}

float setCruiseSpeed(float speed)
{
    return clampSpeed(speed);
}

float incrCruiseSpeed(float cruiseSpeed)
{
    return clampSpeed(cruiseSpeed + SPEED_INC);
}

float decrCruiseSpeed(float cruiseSpeed)
{
    return clampSpeed(cruiseSpeed - SPEED_INC);
}

/* ============================================================
   Output signal handlers (called by generated Esterel C code)
   ============================================================ */

void CruiseControl_O_CruiseSpeed(float val)
{
    printf("CruiseSpeed: %f\n", val);
}

void CruiseControl_O_ThrottleCmd(float val)
{
    printf("ThrottleCmd: %f\n", val);
}

void CruiseControl_O_CruiseState(int val)
{
    printf("CruiseState: %d\n", val);
}
