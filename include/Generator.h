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

private:
  void SetupParams(void);
  void GenerateRandoms(void);
  void CreateDisc(void);
  void CreateCloud(void);

  void CreateStars(void);
  void CalculateVelocity(void);

  Parameters *mParams = NULL;
  OpacityTable *mOpacity = NULL;
  std::vector<Particle *> mParticles;
  Octree *mOctree = NULL;
  OctreePoint *mOctreePoints = NULL;

  uint32 mSeed = 0;
  uint32 mNumHydro = 0;
  uint32 mDim = 0;

  real64 mMStar = 0.0;
  real64 mMDisc = 0.0;
  real64 mRin = 0.0;
  real64 mRout = 0.0;
  real64 mR0 = 0.0;
  real64 mT0 = 0.0;
  real64 mTinf = 0.0;
  uint32 mNumNeigh = 0;

  real64 mP = 0.0;
  real64 mQ = 0.0;

  real64 mRands[3];
  real64 mOmegaIn = 0.0;
  real64 mOmegaOut = 0.0;
  real64 mSigma0 = 0.0;
};
