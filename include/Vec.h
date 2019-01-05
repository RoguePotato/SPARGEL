//===-- Vec.h -------------------------------------------------------------===//
//
//                                  SPARGEL
//                   Smoothed Particle Generator and Loader
//
// This file is distributed under the GNU General Public License. See LICENSE
// for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Vec.h defines the vector construct for 2 and 3 dimesions, including all
/// required operations, some of which will be overloaded.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Definitions.h"

struct Vec3;
Vec3 operator*(float r, const Vec3 &v);

struct Vec3 {
  union {
    struct {
      float x;
      float y;
      float z;
    };
    float D[3];
  };

  Vec3() {}

  Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

  float &operator[](unsigned int i) { return D[i]; }

  const float &operator[](unsigned int i) const { return D[i]; }

  Vec3 operator+(const Vec3 &r) const {
    return Vec3(x + r.x, y + r.y, z + r.z);
  }

  Vec3 operator-(const Vec3 &r) const {
    return Vec3(x - r.x, y - r.y, z - r.z);
  }

  Vec3 operator*(float r) const { return Vec3(x * r, y * r, z * r); }

  Vec3 operator^(const Vec3 &r) const {
    return Vec3(y * r.z - z * r.y, z * r.x - x * r.z, x * r.y - y * r.x);
  }

  Vec3 operator/(float r) const { return Vec3(x / r, y / r, z / r); }

  Vec3 cmul(const Vec3 &r) const { return Vec3(x * r.x, y * r.y, z * r.z); }

  Vec3 cdiv(const Vec3 &r) const { return Vec3(x / r.x, y / r.y, z / r.z); }

  float dot(const Vec3 &r) const { return x * r.x + y * r.y + z * r.z; }

  Vec3 &operator+=(const Vec3 &r) {
    x += r.x;
    y += r.y;
    z += r.z;
    return *this;
  }

  Vec3 &operator-=(const Vec3 &r) {
    x -= r.x;
    y -= r.y;
    z -= r.z;
    return *this;
  }

  Vec3 &operator*=(float r) {
    x *= r;
    y *= r;
    z *= r;
    return *this;
  }

  Vec3 &operator/=(float r) {
    x /= r;
    y /= r;
    z /= r;
    return *this;
  }

  float Norm() const { return sqrt(x * x + y * y + z * z); }

  float Norm2() const { return sqrt(x * x + y * y); }

  float NormSquared() const { return x * x + y * y + z * z; }

  Vec3 Normalized() const { return *this / Norm(); }

  bool IsNaN() const {
    if (x != x)
      return true;
    if (y != y)
      return true;
    if (z != z)
      return true;
    return false;
  }
};

inline Vec3 operator*(float r, const Vec3 &v) {
  return Vec3(v.x * r, v.y * r, v.z * r);
}
