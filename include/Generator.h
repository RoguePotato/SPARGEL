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

  int mSeed = 0;
  int mNumHydro = 0;
  int mDim = 0;

  double mMStar = 0.0;
  double mMDisc = 0.0;
  double mRin = 0.0;
  double mRout = 0.0;
  double mR0 = 0.0;
  double mT0 = 0.0;
  double mTinf = 0.0;
  int mNumNeigh = 0;

  double mP = 0.0;
  double mQ = 0.0;

  double mRands[3];
  double mOmegaIn = 0.0;
  double mOmegaOut = 0.0;
  double mSigma0 = 0.0;
};
