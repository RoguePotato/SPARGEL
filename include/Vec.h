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
Vec3 operator*(FLOAT r, const Vec3 &v);

struct Vec3 {
  union {
    struct {
      FLOAT x;
      FLOAT y;
      FLOAT z;
    };
    FLOAT D[3];
  };

  Vec3() {}

  Vec3(FLOAT _x, FLOAT _y, FLOAT _z) : x(_x), y(_y), z(_z) {}

  FLOAT &operator[](unsigned int i) { return D[i]; }

  const FLOAT &operator[](unsigned int i) const { return D[i]; }

  Vec3 operator+(const Vec3 &r) const {
    return Vec3(x + r.x, y + r.y, z + r.z);
  }

  Vec3 operator-(const Vec3 &r) const {
    return Vec3(x - r.x, y - r.y, z - r.z);
  }

  Vec3 operator*(FLOAT r) const { return Vec3(x * r, y * r, z * r); }

  Vec3 operator^(const Vec3 &r) const {
    return Vec3(y * r.z - z * r.y, z * r.x - x * r.z, x * r.y - y * r.x);
  }

  Vec3 operator/(FLOAT r) const { return Vec3(x / r, y / r, z / r); }

  Vec3 cmul(const Vec3 &r) const { return Vec3(x * r.x, y * r.y, z * r.z); }

  Vec3 cdiv(const Vec3 &r) const { return Vec3(x / r.x, y / r.y, z / r.z); }

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

  Vec3 &operator*=(FLOAT r) {
    x *= r;
    y *= r;
    z *= r;
    return *this;
  }

  Vec3 &operator/=(FLOAT r) {
    x /= r;
    y /= r;
    z /= r;
    return *this;
  }

  FLOAT Norm() const { return sqrt(x * x + y * y + z * z); }

  FLOAT Norm2() const { return sqrt(x * x + y * y); }

  FLOAT NormSquared() const { return x * x + y * y + z * z; }

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

inline Vec3 operator*(FLOAT r, const Vec3 &v) {
  return Vec3(v.x * r, v.y * r, v.z * r);
}
