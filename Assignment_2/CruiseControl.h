#ifndef CRUISECONTROL_DATA_H
#define CRUISECONTROL_DATA_H

#include <stdbool.h>

/* Constants */
#define SPEED_MIN   30.0f
#define SPEED_MAX  150.0f
#define SPEED_INC    2.5f
#define PEDALS_MIN   3.0f

/* Functions called from Esterel */
float regulateThrottle(bool isGoingOn, float cruiseSpeed, float vehicleSpeed);
bool  isAccelPressed(float accel);
bool  isBrakePressed(float brake);
bool  isSpeedInRange(float speed);
float clampSpeed(float speed);
float setCruiseSpeed(float speed);
float incrCruiseSpeed(float cruiseSpeed);
float decrCruiseSpeed(float cruiseSpeed);

/* Output signal handlers */
void CruiseControl_O_CruiseSpeed(float val);
void CruiseControl_O_ThrottleCmd(float val);
void CruiseControl_O_CruiseState(int val);

#endif
