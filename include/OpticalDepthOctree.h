//===-- OpticalDepthOctree.h ----------------------------------------------===//
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
/// OpticalDepthOctree.h contains the functions to create a traverse the data
/// structure primarily used to calculate optical depths parallel to the z-axis.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Constants.h"
#include "Definitions.h"
#include "OpacityTable.h"
#include "Particle.h"
#include "Vec.h"

// The constraint for particles per leaf is 1 for this data structure. This is
// firstly because we would like to retain accuracy, but mainly because it makes
// my life easier.

struct OpticalDepthPoint {
  Vec3 pos;
  FLOAT dens;
  FLOAT temp;
};

class OpticalDepthOctree {
public:
  OpticalDepthOctree(const Vec3 &O, const Vec3 &HD, OpticalDepthOctree *Next,
                     OpticalDepthOctree *More);
  OpticalDepthOctree(const OpticalDepthOctree &Copy);
  ~OpticalDepthOctree();

  void Construct(std::vector<Particle *> Particles);
  void Walk(std::vector<Particle *> &Particles, OpacityTable *opacity);

  void Insert(OpticalDepthPoint *Point);
  void LinkTree(std::vector<OpticalDepthOctree *> &List);
  void TraverseTree(const Vec3 ParticlePos, FLOAT &Sigma, FLOAT &Tau,
                    OpacityTable *opacity);
  int GetOctantContainingPoint(const Vec3 &Point) const;
  bool IsLeafNode() const;
  bool Intersects(OpticalDepthOctree *Cell, const Vec3 ParticlePos) const;

  Vec3 Origin = Vec3(0.0, 0.0, 0.0);
  Vec3 HalfDimension = Vec3(0.0, 0.0, 0.0);

  OpticalDepthOctree *Children[8];
  OpticalDepthPoint *Data = NULL;
  unsigned int TotalPoints = 0;

  OpticalDepthOctree *_Next = NULL;
  OpticalDepthOctree *_More = NULL;
};
