#ifndef _AIUTIL_H_
#define _AIUTIL_H_

#include "NL/nlMath.h"

char** GetPowerupNamesArray();
char* GetPowerupName(int powerup, bool useSpecificName);
void MakeRandomDirection2D(nlVector3& direction, float length);
void SortToMinOrMaxTotalSum(unsigned int* result, float (*data)[4], bool findMin);
nlVector3 GetClosestPointOnLineABFromPointC(const nlVector3& a, const nlVector3& b, const nlVector3& c);
nlVector2 GetClosestPointOnLineABFromPointC(const nlVector2& a, const nlVector2& b, const nlVector2& c);
float AIsgn(float value);
float NormalizeVal(float value, float min, float max);
float NormalizeVal(float value, const nlVector2& extrema);
float InterpolateRangeClamped(float resultMin, float resultMax, float inputMin, float inputMax, float input);
float InterpolateRange(float resultMin, float resultMax, float inputMin, float inputMax, float input);
float InterpolateClamped(float min, float max, float percent);
float Interpolate(float min, float max, float percent);
void RotateVectorZAxis(nlVector3& out, const nlVector3& in, unsigned short angle);
void GetWorldPoint(nlVector3& out, const nlVector3& local, const nlVector3& reference, unsigned short angle);
void GetLocalPoint(nlVector3& out, const nlVector3& world, const nlVector3& reference, unsigned short angle);
float GetClosingSpeed(const nlVector3& pos1, const nlVector3& vel1, const nlVector3& pos2, const nlVector3& vel2);
float GetClosingSpeed2D(const nlVector3& pos1, const nlVector3& vel1, const nlVector3& pos2, const nlVector3& vel2);
float Exp(float k);
bool TestCollision(float rp, const nlVector3& p1, const nlVector3& p2, float rq, const nlVector3& q1, const nlVector3& q2);
void CalcInterceptXY(const nlVector3& pos1, float speed1, float speed2, const nlVector3& pos2, const nlVector3& vel, int& count, float* times);
float SeekSpeed(float current, float desired, float acceleration, float deceleration, float dt);
unsigned short SeekDirection(unsigned short current, unsigned short desired, float speed, float falloff, float dt);
bool IsPointInCone(const nlVector3& point, const nlVector3& pivot, const nlVector3& plane1, const nlVector3& plane2);
void MakePerpendicularPlane(const nlVector3& position, const nlVector3& normal, nlVector4& plane, float offset);
void MakePerpendicularPlane(const nlVector3& position, unsigned short normalAngle, nlVector4& plane, float offset);

#endif
