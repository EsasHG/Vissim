#ifndef ROLLINGBALL_H
#define ROLLINGBALL_H

#include "sphere.h"

class RollingBall : public Sphere
{
public:
    RollingBall();

    void move(VisualObject *plane);

    bool CalculateBarycentricCoordinates(VisualObject *plane);
    gsl::Vector3D inputVector{};
private:
    float radius = 1.f;
    gsl::Vector3D Velocity{};
    gsl::Vector3D Acceleration{};
    gsl::Vector3D Gravity{0,-0.1,0};

    gsl::Vector3D LastLocation{};

    gsl::Vector3D CurrentTriangleNormal{};
};

#endif // ROLLINGBALL_H
