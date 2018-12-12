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

#include "Constants.h"
#include "Definitions.h"
#include "Vec.h"

class Particle {
public:
  Particle() {};
  ~Particle() {};

  int GetID() { return mID; }
  Vec3 GetX() { return mX; }
  Vec3 GetV() { return mV; }
  float GetR() { return mR; }
  float GetS() { return mS; }
  float GetT() { return mT; }
  float GetH() { return mH; }
  float GetD() { return mD; }
  float GetM() { return mM; }
  float GetU() { return mU; }
  float GetP() { return mP; }
  float GetCS() { return mCS; }
  float GetOmega() { return mOmega; }
  float GetQ() { return mQ; }
  float GetKappa() { return mKappa; }
  float GetSigma() { return mSigma; }
  float GetTau() { return mTau; }
  float GetDUDT() { return mDUDT; }
  float GetBeta() { return mBeta; }
  int GetType() { return mType; }
  float GetExtra(int index) { return mExtra[index]; }

  void SetID(int id) { mID = id; }
  void SetX(Vec3 x) { mX = x; }
  void SetR(float r) { mR = r; }
  void SetV(Vec3 v) { mV = v; }
  void SetT(float T) { mT = T; }
  void SetH(float H) { mH = H; }
  void SetD(float D) { mD = D; }
  void SetM(float M) { mM = M; }
  void SetU(float U) { mU = U; }
  void SetP(float P) { mP = P; }
  void SetCS(float cs) { mCS = cs; }
  void SetOmega(float o) { mOmega = o; }
  void SetQ(float Q) { mQ = Q; }
  void SetKappa(float k) { mKappa = k; }
  void SetSigma(float s) { mSigma = s; }
  void SetTau(float tau) { mTau = tau; }
  void SetDUDT(float dudt) { mDUDT = dudt; }
  void SetBeta(float beta) { mBeta = beta; }
  void SetType(int type) { mType = type; }
  void SetExtra(int index, float value) { mExtra[index] = value; }

private:
  int mID = 0;
  Vec3 mX = Vec3(0.0, 0.0, 0.0);
  Vec3 mV = Vec3(0.0, 0.0, 0.0);
  float mR = 0.0;
  float mS = 0.0;
  float mT = 0.0;
  float mH = 0.0;
  float mD = 0.0;
  float mM = 0.0;
  float mU = 0.0;
  float mP = 0.0;
  float mCS = 0.0;
  float mOmega = 0.0;
  float mQ = 0.0;
  float mGamma = 0.0;
  float mMu = 0.0;
  float mKappa = 0.0;
  float mSigma = 0.0;
  float mTau = 0.0;
  float mDUDT = 0.0;
  float mBeta = 0.0;
  int mType = 1;
  float mExtra[EXTRA_DATA] = {0.0};
};

class Sink : public Particle {
public:
  Sink() {};
  ~Sink() {};

  float *GetAllData() { return mSerenData; }
  float GetData(int index) { return mSerenData[index]; }
  void SetData(int index, float data) { mSerenData[index] = data; }

  float GetClumpR() { return mClumpR; }
  float SetClumpR(float r) { mClumpR = r; }
  float GetClumpM() { return mClumpM; }
  float SetClumpM(float m) { mClumpM = m; }

private:
  float mSerenData[255] = {0.0};
  float mClumpR = 0.0;
  float mClumpM = 0.0;
};
