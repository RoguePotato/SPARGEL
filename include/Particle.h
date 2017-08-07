//===-- Particle.h --------------------------------------------------------===//
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
/// Particle.h defines all particle classes and corresponding data.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Definitions.h"
#include "Vec.h"

class Particle {
public:
  Particle();
  ~Particle();

private:
  uint32 mID = 0;
  Vec3 mX = Vec3(0.0, 0.0, 0.0);
  Vec3 mV = Vec3(0.0, 0.0, 0.0);
  real64 mR = 0.0;
  real64 mS = 0.0;
  real64 mT = 0.0;
  real64 mH = 0.0;
  real64 mD = 0.0;
  real64 mM = 0.0;
  real64 mU = 0.0;
  real64 mQ = 0.0;
  real64 mGamma = 0.0;
  real64 mMu = 0.0;
  int8 mType = 1;
};

class Gas : public Particle {
public:
  Gas();
  ~Gas();

private:

};

class Sink : public Particle {
public:
  Sink();
  ~Sink();

private:

};
