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

  int GetID() { return mID; }
  Vec3 GetX() { return mX; }
  Vec3 GetV() { return mV; }
  double GetR() { return mX.Norm(); }
  double GetS() { return mV.Norm(); }
  double GetT() { return mT; }
  double GetH() { return mH; }
  double GetD() { return mD; }
  double GetM() { return mM; }
  double GetU() { return mU; }
  int GetType() { return mType; }

  void SetID(int id) { mID = id; }
  void SetX(Vec3 x) { mX = x; }
  void SetV(Vec3 v) { mV = v; }
  void SetT(double T) { mT = T; }
  void SetH(double H) { mH = H; }
  void SetD(double D) { mD = D; }
  void SetM(double M) { mM = M; }
  void SetU(double U) { mU = U; }
  void SetType(int type) { mType = type; }

private:
  int mID = 0;
  Vec3 mX = Vec3(0.0, 0.0, 0.0);
  Vec3 mV = Vec3(0.0, 0.0, 0.0);
  double mR = 0.0;
  double mS = 0.0;
  double mT = 0.0;
  double mH = 0.0;
  double mD = 0.0;
  double mM = 0.0;
  double mU = 0.0;
  double mQ = 0.0;
  double mGamma = 0.0;
  double mMu = 0.0;
  int mType = 1;
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

  double GetData(int index) { return mSerenData[index]; }
  void SetData(int index, double data) { mSerenData[index] = data; }
private:
  double mSerenData[255] = {0.0};
};
