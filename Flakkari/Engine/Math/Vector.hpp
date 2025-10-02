/**************************************************************************
 * Flakkari Library v0.6.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file Vector.hpp
 * @brief Vector class for Math. (Vector2, Vector3, Vector4) and Color.
 *
 * @warning Not Pedantic. (Not Standard)
 * cause: "ISO C++ forbids anonymous structs [-Wpedantic]"
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.6.0
 * @date 2024-01-05
 **************************************************************************/

#ifndef FLAKKARI_VECTOR_HPP_
#define FLAKKARI_VECTOR_HPP_

#include <cmath>
#include <iostream>

#include "flakkari_config.h"

namespace Flakkari::Engine::Math {

template <typename Type> struct Vector {

    LPL_PACKED_START
    union {
        struct {
            Type x;
            Type y;
            Type z;
            Type w;
        } vec;
        struct {
            Type r;
            Type g;
            Type b;
            Type a;
        } color;
        struct {
            Type width;
            Type height;
            Type depth;
            Type time;
        } dimension;
        Type v[4];
    };

    Vector() : v{0, 0, 0, 1} {};
    Vector(Type x, Type y, Type z, Type w) : v{x, y, z, w} {};
    Vector(Type x, Type y, Type z) : v{x, y, z, 1} {};
    Vector(Type x, Type y) : v{x, y, 0, 1} {};
    Vector(Type x) : v{x, 0, 0, 1} {};
    Vector(const Vector<Type> &other) : v{other.v[0], other.v[1], other.v[2], other.v[3]} {};
    LPL_PACKED_END

    Vector<Type> &operator=(const Vector<Type> &other)
    {
        v[0] = other.v[0];
        v[1] = other.v[1];
        v[2] = other.v[2];
        v[3] = other.v[3];
        return *this;
    }

    Vector<Type> operator+(const Vector<Type> &other) const
    {
        return Vector<Type>(v[0] + other.v[0], v[1] + other.v[1], v[2] + other.v[2], v[3] + other.v[3]);
    }

    Vector<Type> operator-(const Vector<Type> &other) const
    {
        return Vector<Type>(v[0] - other.v[0], v[1] - other.v[1], v[2] - other.v[2], v[3] - other.v[3]);
    }

    Vector<Type> operator*(const Vector<Type> &other) const
    {
        return Vector<Type>(v[0] * other.v[0], v[1] * other.v[1], v[2] * other.v[2], v[3] * other.v[3]);
    }

    Vector<float> multiplyWithFloatVector(const Vector<float> &other) const
    {
        return Vector<float>((float) v[0] * other.v[0], (float) v[1] * other.v[1], (float) v[2] * other.v[2],
                             (float) v[3] * other.v[3]);
    }

    Vector<Type> operator/(const Vector<Type> &other) const
    {
        return Vector<Type>(v[0] / other.v[0], v[1] / other.v[1], v[2] / other.v[2], v[3] / other.v[3]);
    }

    Vector<Type> operator+(const Type &other) const
    {
        return Vector<Type>(v[0] + other, v[1] + other, v[2] + other, v[3] + other);
    }

    Vector<Type> operator-(const Type &other) const
    {
        return Vector<Type>(v[0] - other, v[1] - other, v[2] - other, v[3] - other);
    }

    Vector<Type> operator*(const Type &other) const
    {
        return Vector<Type>(v[0] * other, v[1] * other, v[2] * other, v[3] * other);
    }

    Vector<Type> operator/(const Type &other) const
    {
        return Vector<Type>(v[0] / other, v[1] / other, v[2] / other, v[3] / other);
    }

    Vector<Type> &operator+=(const Vector<Type> &other)
    {
        v[0] += other.v[0];
        v[1] += other.v[1];
        v[2] += other.v[2];
        v[3] += other.v[3];
        return *this;
    }

    Vector<Type> &operator-=(const Vector<Type> &other)
    {
        v[0] -= other.v[0];
        v[1] -= other.v[1];
        v[2] -= other.v[2];
        v[3] -= other.v[3];
        return *this;
    }

    Vector<Type> &operator*=(const Vector<Type> &other)
    {
        v[0] *= other.v[0];
        v[1] *= other.v[1];
        v[2] *= other.v[2];
        v[3] *= other.v[3];
        return *this;
    }

    Vector<Type> &operator/=(const Vector<Type> &other)
    {
        v[0] /= other.v[0];
        v[1] /= other.v[1];
        v[2] /= other.v[2];
        v[3] /= other.v[3];
        return *this;
    }

    Vector<Type> &operator+=(const Type &other)
    {
        v[0] += other;
        v[1] += other;
        v[2] += other;
        v[3] += other;
        return *this;
    }

    Vector<Type> &operator-=(const Type &other)
    {
        v[0] -= other;
        v[1] -= other;
        v[2] -= other;
        v[3] -= other;
        return *this;
    }

    Vector<Type> &operator*=(const Type &other)
    {
        v[0] *= other;
        v[1] *= other;
        v[2] *= other;
        v[3] *= other;
        return *this;
    }

    Vector<Type> &operator/=(const Type &other)
    {
        v[0] /= other;
        v[1] /= other;
        v[2] /= other;
        v[3] /= other;
        return *this;
    }

    bool operator==(const Vector<Type> &other) const
    {
        return v[0] == other.v[0] && v[1] == other.v[1] && v[2] == other.v[2] && v[3] == other.v[3];
    }

    bool operator!=(const Vector<Type> &other) const
    {
        return v[0] != other.v[0] || v[1] != other.v[1] || v[2] != other.v[2] || v[3] != other.v[3];
    }

    Type &operator[](const int &index) { return v[index]; }

    Type operator[](const int &index) const { return v[index]; }

    Type length() const { return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]); }

    Vector<Type> &normalize()
    {
        Type len = length();
        if (len == 0)
            return *this;
        v[0] /= len;
        v[1] /= len;
        v[2] /= len;
        return *this;
    }

    Vector<Type> normalized() const
    {
        Type len = length();
        if (len == 0)
            return *this;
        return Vector<Type>(v[0] / len, v[1] / len, v[2] / len);
    }

    Type dot(const Vector<Type> &other) const
    {
        return v[0] * other.v[0] + v[1] * other.v[1] + v[2] * other.v[2] + v[3] * other.v[3];
    }

    Vector<Type> cross(const Vector<Type> &other) const
    {
        return Vector<Type>(v[1] * other.v[2] - v[2] * other.v[1], v[2] * other.v[0] - v[0] * other.v[2],
                            v[0] * other.v[1] - v[1] * other.v[0], 0);
    }

    Vector<Type> &lerp(const Vector<Type> &other, const Type &t)
    {
        v[0] += (other.v[0] - v[0]) * t;
        v[1] += (other.v[1] - v[1]) * t;
        v[2] += (other.v[2] - v[2]) * t;
        v[3] += (other.v[3] - v[3]) * t;
        return *this;
    }

    Vector<Type> lerped(const Vector<Type> &other, const Type &t) const
    {
        return Vector<Type>(v[0] + (other.v[0] - v[0]) * t, v[1] + (other.v[1] - v[1]) * t,
                            v[2] + (other.v[2] - v[2]) * t, v[3] + (other.v[3] - v[3]) * t);
    }

    Vector<Type> &lerp(const Vector<Type> &other, const Vector<Type> &t)
    {
        v[0] += (other.v[0] - v[0]) * t.v[0];
        v[1] += (other.v[1] - v[1]) * t.v[1];
        v[2] += (other.v[2] - v[2]) * t.v[2];
        v[3] += (other.v[3] - v[3]) * t.v[3];
        return *this;
    }

    Vector<Type> lerped(const Vector<Type> &other, const Vector<Type> &t) const
    {
        return Vector<Type>(v[0] + (other.v[0] - v[0]) * t.v[0], v[1] + (other.v[1] - v[1]) * t.v[1],
                            v[2] + (other.v[2] - v[2]) * t.v[2], v[3] + (other.v[3] - v[3]) * t.v[3]);
    }

    Vector<Type> &lerp(const Vector<Type> &other, const Type &t, const Type &dt)
    {
        v[0] += (other.v[0] - v[0]) * t * dt;
        v[1] += (other.v[1] - v[1]) * t * dt;
        v[2] += (other.v[2] - v[2]) * t * dt;
        v[3] += (other.v[3] - v[3]) * t * dt;
        return *this;
    }

    Vector<Type> lerped(const Vector<Type> &other, const Type &t, const Type &dt) const
    {
        return Vector<Type>(v[0] + (other.v[0] - v[0]) * t * dt, v[1] + (other.v[1] - v[1]) * t * dt,
                            v[2] + (other.v[2] - v[2]) * t * dt, v[3] + (other.v[3] - v[3]) * t * dt);
    }

    Vector<Type> &lerp(const Vector<Type> &other, const Vector<Type> &t, const Type &dt)
    {
        v[0] += (other.v[0] - v[0]) * t.v[0] * dt;
        v[1] += (other.v[1] - v[1]) * t.v[1] * dt;
        v[2] += (other.v[2] - v[2]) * t.v[2] * dt;
        v[3] += (other.v[3] - v[3]) * t.v[3] * dt;
        return *this;
    }

    Vector<Type> lerped(const Vector<Type> &other, const Vector<Type> &t, const Type &dt) const
    {
        return Vector<Type>(v[0] + (other.v[0] - v[0]) * t.v[0] * dt, v[1] + (other.v[1] - v[1]) * t.v[1] * dt,
                            v[2] + (other.v[2] - v[2]) * t.v[2] * dt, v[3] + (other.v[3] - v[3]) * t.v[3] * dt);
    }

    Vector<Type> &lerp(const Vector<Type> &other, const Type &t, const Vector<Type> &dt)
    {
        v[0] += (other.v[0] - v[0]) * t * dt.v[0];
        v[1] += (other.v[1] - v[1]) * t * dt.v[1];
        v[2] += (other.v[2] - v[2]) * t * dt.v[2];
        v[3] += (other.v[3] - v[3]) * t * dt.v[3];
        return *this;
    }

    void rotate(const Type &angleRadians)
    {
        Type c = cos(angleRadians);
        Type s = sin(angleRadians);
        Type x = v[0] * c - v[1] * s;
        Type y = v[0] * s + v[1] * c;
        v[0] = x;
        v[1] = y;
    }

    void quaternionRotate(Type x, Type y, Type z, Type w)
    {
        v[0] = v[3] * x + v[0] * w + v[1] * z - v[2] * y;
        v[1] = v[3] * y - v[0] * z + v[1] * w + v[2] * x;
        v[2] = v[3] * z + v[0] * y - v[1] * x + v[2] * w;
        v[3] = v[3] * w - v[0] * x - v[1] * y - v[2] * z;
    }

    /**
     * @brief Rotate the vector around an axis by an angle in radians.
     *
     * @param axis  The axis to rotate around.
     * @param angleRadians  The angle in radians.
     */
    void rotate(const Vector<Type> &axis, float angleDegrees)
    {
        const double _pi = 3.14159265358979323846;
        double angleRadians = (angleDegrees * _pi / 180) / 2.0;
        double sinHalfAngle = sin(angleRadians);
        auto normalizedAxis = axis.normalized();
        quaternionRotate(normalizedAxis.v[0] * sinHalfAngle, normalizedAxis.v[1] * sinHalfAngle,
                         normalizedAxis.v[2] * sinHalfAngle, cos(angleRadians));
    }
};

using Vector2f = Vector<float>;
using Vector2d = Vector<double>;
using Vector2i = Vector<int>;
using Vector2u = Vector<unsigned int>;
using Vector3f = Vector<float>;
using Vector3d = Vector<double>;
using Vector3i = Vector<int>;
using Vector3u = Vector<unsigned int>;
using Vector4f = Vector<float>;
using Vector4d = Vector<double>;
using Vector4i = Vector<int>;
using Vector4u = Vector<unsigned int>;
using Quaternion = Vector4d;
using Color = Vector4f;

template <typename Type> std::ostream &operator<<(std::ostream &os, const Vector<Type> &vector);

} // namespace Flakkari::Engine::Math

#endif /* !FLAKKARI_VECTOR_HPP_ */
