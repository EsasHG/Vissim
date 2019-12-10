#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <QOpenGLFunctions_4_1_Core>
#include <vector>
#include <matrix4x4.h>

class Triangle
{
public:
    Triangle() {}
    Triangle(unsigned in1, unsigned in2, unsigned in3);


    friend std::ostream& operator<< (std::ostream&, const Triangle&);

    unsigned v1;
    unsigned v2;
    unsigned v3;
};

#endif // TRIANGLE_H
