#ifndef ROLLINGBALL_H
#define ROLLINGBALL_H

#include "sphere.h"

class RollingBall : public Sphere
{
public:
    RollingBall();

    void move(VisualObject *plane);

    void calculateBarycentricCoordinates(VisualObject *plane);
    gsl::Vector3D inputVector{};
private:
    float radius = 1.f;
    float speed = 0.001f;
    float mass = 1.f;
    gsl::Vector3D velocity{};
    gsl::Vector3D acceleration{};
    gsl::Vector3D gravity{0,-9.81f,0};

    gsl::Vector3D lastLocation{};
    bool isFirstCollision = true;
    gsl::Vector3D prevTriangleNormal{0};
};

#endif // ROLLINGBALL_H
