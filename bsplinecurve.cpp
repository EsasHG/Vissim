#include "bsplinecurve.h"

void BSplineCurve::generateKnots()
{
    mKnots.clear();
    int numKnots = d + static_cast<int>(mControlPoints.size()) + 1;
//    if(diff>0)
//        numKnots+=diff;
    const int interpolateNum = numKnots - ((d+1)*2);

    for(int i = 0; i < numKnots; ++i)
    {
        if(i < d + 1)
        {
            mKnots.push_back(0);
        }
        else if(i >= interpolateNum + (d + 1))
        {
            mKnots.push_back(1);
        }
        else
        {
            mKnots.push_back(static_cast<float>((i - (d))) * (1.f / static_cast<float>((interpolateNum + 1))));
        }
    }
}

BSplineCurve::BSplineCurve(std::vector<gsl::Vector3D> controlpoints, int degree)
    : mControlPoints(controlpoints), d(degree)
{

//    const int diff = controlpoints.size() - (degree+1);


    mMatrix.setToIdentity();
}

void BSplineCurve::init()
{
    initializeOpenGLFunctions();

    generateKnots();
    DrawCurve();
    DrawControl();

    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);

    glBindVertexArray(mVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(mVertices.size()) * sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

}

int BSplineCurve::findKnotInterval(float x)
{
    for (int i=0; i<mKnots.size()-1; i++ )
    {
        if(mKnots[i]<=x && x<mKnots[i+1])
            return i;
    }
    return -1;
}

void BSplineCurve::draw()
{

    glBindVertexArray(mVAO);
    //glLineWidth(10.f);
    glDrawArrays(GL_LINE_STRIP,0,static_cast<GLint>(mVertices.size()));

    glBindVertexArray(0);
    glUseProgram(0);
}

void BSplineCurve::DrawCurve()
{
    mVertices.clear();
    for (GLfloat i = 0.f; i <= 1.f; i+= (1.f/mCurveSmoothness))
    {
        if(findKnotInterval(i)<0)
        {
            qDebug() << "invalid interval!";
            continue;
        }
        mVertices.push_back({evaluateBSpline(findKnotInterval(i),i), curveColor, {}});
    }
    //evaluateBSpline(2,0.4f);
}

void BSplineCurve::DrawControl()
{
    for(int i = mControlPoints.size() - 1; i >= 0 ; --i)
    {
        mVertices.push_back(Vertex(mControlPoints[i], {0, 0, 0}, {0, 0}));
    }
}
// Parametre inn:
// x - en parameterverdi på skjøtvektor
// my - et tall slik at t[my] <= x < t[my+1]
// Returverdi: et punkt på splinekurven
// b,n,d,t er objektvariable i klassen BSplineCurve
gsl::Vector3D BSplineCurve::evaluateBSpline(int my, float x)
{
    gsl::Vector3D a[20]; // forutsetter da at n+d+1 <= 20
    for (int j=0; j<=d; j++)
    {
        a[d-j] = mControlPoints[my-j];
    }
    for (int k=d; k>0; k--)
    {
        int j = my-k;
        for (int i=0; i<k; i++)
        {
            j++;
            float w = (x-mKnots[j])/(mKnots[j+k]-mKnots[j]);
            a[i] = a[i] * (1-w) + a[i+1] * w;
        }
    }
    return a[0];
}


