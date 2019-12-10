#include "triangle.h"

Triangle::Triangle(unsigned in1 ,unsigned in2, unsigned in3) : v1(in1), v2(in2), v3(in3)
{

}


std::ostream& operator<< (std::ostream& os, const Triangle& v) {
    os << std::fixed;
    os << v.v1 << " " << v.v2 << " " << v.v3;

    return os;
}
