#ifndef GAME_CAMERA_CAMERA_DAMPING_H
#define GAME_CAMERA_CAMERA_DAMPING_H

static inline void Dampen(float& current, const float& target,
    float& currentVelocity, float smoothTime, float deltaTime)
{
    float omega = 2.0f / smoothTime;
    float x = omega * deltaTime;
    float exp = 1.0f / ((0.48f * x * x + (1.0f + x)) + x * (0.235f * x * x));
    float change = current - target;
    float temp = deltaTime * (omega * change + currentVelocity);
    currentVelocity = exp * (currentVelocity - omega * temp);
    current = exp * (change + temp) + target;
}

#endif // GAME_CAMERA_CAMERA_DAMPING_H
