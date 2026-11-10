// compiler directive
#ifndef TRIANGLE_4620    /// ifndef: if not defined
#define TRIANGLE_4620

#include "Vector3.hpp"

/// class representing a triangle using three 3D vertices
class Triangle
{
    public:
    Vector3 vertex1, vertex2, vertex3;
};

#endif