//===-- Generator.h -------------------------------------------------------===//
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
/// Generator.h contains the functions to create an SPH snapshot based off of
/// the parameters specified in a given parameters file.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Definitions.h"
#include "Octree.h"
#include "OpacityTable.h"
#include "Parameters.h"
#include "Particle.h"

class Generator {
public:
  Generator(Parameters *params, OpacityTable *opacity);
  ~Generator();

  void Create(void);

  std::vector<Particle *> GetParticles() { return mParticles; }
  std::vector<Sink *> GetSinks() { return mSinks; }

private:
  void SetupParams(void);
  void GenerateRandoms(void);
  void CreateDisc(void);
  void CreateCloud(void);

  void CreateStars(void);
  void CreatePlanet(void);
  void CalculateVelocity(void);

  Parameters *mParams = NULL;
  OpacityTable *mOpacity = NULL;
  std::vector<Particle *> mParticles;
  std::vector<Sink *> mSinks;
  Octree *mOctree = NULL;
  OctreePoint *mOctreePoints = NULL;

  int mSeed = 0;
  int mNumHydro = 0;
  int mDim = 0;

  FLOAT mMStar = 0.0;
  FLOAT mMDisc = 0.0;
  FLOAT mMBinary = 0.0;
  FLOAT mMTotal = 0.0;
  FLOAT mBinarySep = 0.0;
  FLOAT mBinaryEcc = 0.0;
  FLOAT mBinaryInc = 0.0;
  FLOAT mRin = 0.0;
  FLOAT mRout = 0.0;
  FLOAT mR0 = 0.0;
  FLOAT mT0 = 0.0;
  FLOAT mTinf = 0.0;
  int mNumNeigh = 0;

  FLOAT mP = 0.0;
  FLOAT mQ = 0.0;

  FLOAT mSinkRadius = 0.0;

  FLOAT mRands[3];
  FLOAT mOmegaIn = 0.0;
  FLOAT mOmegaOut = 0.0;
  FLOAT mSigma0 = 0.0;

  int mPlanet = 0;
  FLOAT mPlanetMass = 0.0;
  FLOAT mPlanetRadius = 0.0;
  FLOAT mPlanetEcc = 0.0;
  FLOAT mPlanetInc = 0.0;

  FLOAT mCloudRadius = 0.0;
  FLOAT mCloudMass = 0.0;
  FLOAT mCloudVol = 0.0;
};
