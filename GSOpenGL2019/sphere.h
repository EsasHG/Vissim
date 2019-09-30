#ifndef SPHERE_H
#define SPHERE_H

#include "visualobject.h"
class Vertex;
typedef gsl::Vector3D Vec3;
class Sphere : public VisualObject {

public:
    Sphere(int n=0);
    void init() override;
    void draw() override;

private:
    int m_rekursjoner;
    int m_indeks;               // brukes i rekursjon, til å bygge m_vertices
    void lagTriangel(const Vec3& v1, const Vec3& v2, const Vec3& v3);
    void subDivide(const Vec3& a, const Vec3& b, const Vec3& c, int n);
    void oktaederUnitBall();


};


#endif // SPHERE_H
