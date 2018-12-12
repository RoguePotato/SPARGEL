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

  void Create();

  std::vector<Particle *> GetParticles() { return mParticles; }
  std::vector<Sink *> GetSinks() { return mSinks; }

private:
  void SetupParams();
  void GenerateRandoms();
  void CreateDisc();
  void CreateCloud();

  void CreateStars();
  void CreatePlanet();
  void CalculateVelocity();

  Parameters *mParams = NULL;
  OpacityTable *mOpacity = NULL;
  std::vector<Particle *> mParticles;
  std::vector<Sink *> mSinks;
  Octree *mOctree = NULL;
  OctreePoint *mOctreePoints = NULL;

  int mSeed = 0;
  int mNumHydro = 0;
  int mDim = 0;

  float mMStar = 0.0;
  float mMDisc = 0.0;
  float mMBinary = 0.0;
  float mMTotal = 0.0;
  float mBinarySep = 0.0;
  float mBinaryEcc = 0.0;
  float mBinaryInc = 0.0;
  float mRin = 0.0;
  float mRout = 0.0;
  float mR0 = 0.0;
  float mT0 = 0.0;
  float mTinf = 0.0;
  int mNumNeigh = 0;

  float mP = 0.0;
  float mQ = 0.0;

  float mStarSmoothing = 0.0;
  float mPlanetSmoothing = 0.0;

  float mRands[3];
  float mOmegaIn = 0.0;
  float mOmegaOut = 0.0;
  float mSigma0 = 0.0;

  int mPlanet = 0;
  float mPlanetMass = 0.0;
  float mPlanetRadius = 0.0;
  float mPlanetEcc = 0.0;
  float mPlanetInc = 0.0;

  float mCloudRadius = 0.0;
  float mCloudMass = 0.0;
  float mCloudVol = 0.0;
};
