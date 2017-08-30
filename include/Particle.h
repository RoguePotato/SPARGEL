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

  uint32 GetID() { return mID; }
  Vec3 GetX() { return mX; }
  Vec3 GetV() { return mV; }
  real64 GetR() { return mX.Norm(); }
  real64 GetS() { return mV.Norm(); }
  real64 GetT() { return mT; }
  real64 GetH() { return mH; }
  real64 GetD() { return mD; }
  real64 GetM() { return mM; }
  real64 GetU() { return mU; }
  int32 GetType() { return mType; }

  void SetID(uint32 id) { mID = id; }
  void SetX(Vec3 x) { mX = x; }
  void SetV(Vec3 v) { mV = v; }
  void SetT(real64 T) { mT = T; }
  void SetH(real64 H) { mH = H; }
  void SetD(real64 D) { mD = D; }
  void SetM(real64 M) { mM = M; }
  void SetU(real64 U) { mU = U; }
  void SetType(int32 type) { mType = type; }

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
  int32 mType = 1;
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

  real64 GetData(uint32 index) { return mSerenData[index]; }
  void SetData(uint32 index, real64 data) { mSerenData[index] = data; }
private:
  real64 mSerenData[255] = {0.0};
};
