#ifndef BSPLINECURVE_H
#define BSPLINECURVE_H

#include <vector>
#include "visualobject.h"

class TriangleSurface;
class BSplineCurve : public VisualObject
{
public:
    // ikke alle disse funksjonene er nødvendige
    BSplineCurve(std::vector<gsl::Vector3D> controlpoints, int degree=2);
    virtual void draw() override;
    virtual void init() override;
    void setKnotsAndControlPoints(std::vector<float> knots, std::vector<gsl::Vector3D> points);
    int findKnotInterval(float x);
    gsl::Vector3D evaluateBSpline(int my, float x);
    gsl::Vector3D evaluateBSpline(int degree, int startKnot, float x);
    unsigned getNumberOfControlPoints() {return mControlPoints.size();}

    void DrawCurve();
    void DrawControl();
    void generateKnots();
    
    std::vector<gsl::Vector3D>  mControlPoints;      // control points
    
private:
    std::vector<float> mKnots;   // knots
    int numKnots;          //n = number of knots
    int d;          //d = degree
    float mCurveSmoothness = 100.f;
    gsl::Vector3D m_ControlColor = gsl::Vector3D{0,0,1};
    gsl::Vector3D curveColor = gsl::Vector3D{1,0,0};

};

#endif // BSPLINECURVE_H
