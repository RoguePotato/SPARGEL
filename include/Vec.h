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
Vec3 operator*(real64 r, const Vec3 &v);

struct Vec3 {
  union {
    struct {
      real64 x;
      real64 y;
      real64 z;
    };
    real64 D[3];
  };

  Vec3() {}

  Vec3(real64 _x, real64 _y, real64 _z) : x(_x), y(_y), z(_z) {}

  real64 &operator[](unsigned int i) { return D[i]; }

  const real64 &operator[](unsigned int i) const { return D[i]; }

  Vec3 operator+(const Vec3 &r) const {
    return Vec3(x + r.x, y + r.y, z + r.z);
  }

  Vec3 operator-(const Vec3 &r) const {
    return Vec3(x - r.x, y - r.y, z - r.z);
  }

  Vec3 operator*(real64 r) const { return Vec3(x * r, y * r, z * r); }

  Vec3 operator^(const Vec3 &r) const {
    return Vec3(y * r.z - z * r.y, z * r.x - x * r.z, x * r.y - y * r.x);
  }

  Vec3 operator/(real64 r) const { return Vec3(x / r, y / r, z / r); }

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

  Vec3 &operator*=(real64 r) {
    x *= r;
    y *= r;
    z *= r;
    return *this;
  }

  Vec3 &operator/=(real64 r) {
    x /= r;
    y /= r;
    z /= r;
    return *this;
  }

  real64 Norm() const { return sqrt(x * x + y * y + z * z); }

  real64 Norm2() const { return sqrt(x * x + y * y); }

  real64 NormSquared() const { return x * x + y * y + z * z; }

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

inline Vec3 operator*(real64 r, const Vec3 &v) {
  return Vec3(v.x * r, v.y * r, v.z * r);
}
