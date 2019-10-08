#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include "visualobject.h"

class BezierCurve : public VisualObject
{
public:
    BezierCurve(const std::vector<gsl::Vector3D>& points = std::vector<gsl::Vector3D>(), float curveSmoothness=10, bool drawCurve = false, bool drawControl = false);
    ~BezierCurve() override;
    virtual void init() override;
    virtual void draw() override;

    gsl::Vector3D GetCurvePosition();

    void ResetCurrentT();

private:
    void DrawCurve();
    void DrawControl();
    gsl::Vector3D Calculate(float t);

    gsl::Vector3D m_CurveColor;
    gsl::Vector3D m_ControlColor;

    float m_CurveSmoothness;
    std::vector<gsl::Vector3D> m_Points;

    float m_CurrentT = 0;
    bool bIncrementT = true;
    float m_Speed = 0.1f;

};

#endif // BEZIERCURVE_H
