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
  FLOAT GetR() { return mR; }
  FLOAT GetS() { return mS; }
  FLOAT GetT() { return mT; }
  FLOAT GetH() { return mH; }
  FLOAT GetD() { return mD; }
  FLOAT GetM() { return mM; }
  FLOAT GetU() { return mU; }
  FLOAT GetP() { return mP; }
  FLOAT GetCS() { return mCS; }
  FLOAT GetOmega() { return mOmega; }
  FLOAT GetQ() { return mQ; }
  FLOAT GetKappa() { return mKappa; }
  FLOAT GetSigma() { return mSigma; }
  FLOAT GetTau() { return mTau; }
  FLOAT GetDUDT() { return mDUDT; }
  FLOAT GetBeta() { return mBeta; }
  int GetType() { return mType; }

  void SetID(int id) { mID = id; }
  void SetX(Vec3 x) { mX = x; }
  void SetR(FLOAT r) { mR = r; }
  void SetV(Vec3 v) { mV = v; }
  void SetT(FLOAT T) { mT = T; }
  void SetH(FLOAT H) { mH = H; }
  void SetD(FLOAT D) { mD = D; }
  void SetM(FLOAT M) { mM = M; }
  void SetU(FLOAT U) { mU = U; }
  void SetP(FLOAT P) { mP = P; }
  void SetCS(FLOAT cs) { mCS = cs; }
  void SetOmega(FLOAT o) { mOmega = o; }
  void SetQ(FLOAT Q) { mQ = Q; }
  void SetKappa(FLOAT k) { mKappa = k; }
  void SetSigma(FLOAT s) { mSigma = s; }
  void SetTau(FLOAT tau) { mTau = tau; }
  void SetDUDT(FLOAT dudt) { mDUDT = dudt; }
  void SetBeta(FLOAT beta) { mBeta = beta; }
  void SetType(int type) { mType = type; }

private:
  int mID = 0;
  Vec3 mX = Vec3(0.0, 0.0, 0.0);
  Vec3 mV = Vec3(0.0, 0.0, 0.0);
  FLOAT mR = 0.0;
  FLOAT mS = 0.0;
  FLOAT mT = 0.0;
  FLOAT mH = 0.0;
  FLOAT mD = 0.0;
  FLOAT mM = 0.0;
  FLOAT mU = 0.0;
  FLOAT mP = 0.0;
  FLOAT mCS = 0.0;
  FLOAT mOmega = 0.0;
  FLOAT mQ = 0.0;
  FLOAT mGamma = 0.0;
  FLOAT mMu = 0.0;
  FLOAT mKappa = 0.0;
  FLOAT mSigma = 0.0;
  FLOAT mTau = 0.0;
  FLOAT mDUDT = 0.0;
  FLOAT mBeta = 0.0;
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

  FLOAT *GetAllData() { return mSerenData; }
  FLOAT GetData(int index) { return mSerenData[index]; }
  void SetData(int index, FLOAT data) { mSerenData[index] = data; }

private:
  FLOAT mSerenData[255] = {0.0};
};
