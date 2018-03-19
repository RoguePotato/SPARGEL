//===-- VerticalBin.h -----------------------------------------------------===//
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
/// VerticalBin.h stores particles in containers constrained by height above the
/// disc. Average quantites are calculated within this class. The average
/// indices are as thus:
/// 0 : Density
/// 1 : Temperature
/// 2 : Mass
/// 3 : -
/// 4 : Optical depth
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Constants.h"
#include "Definitions.h"
#include "File.h"
#include "Particle.h"

class VerticalBin {
public:
  VerticalBin(FLOAT in, FLOAT out, FLOAT width);
  ~VerticalBin();

  void AddParticle(Particle *p) { mParticles.push_back(p); }

  void CalculateValues(void);

  FLOAT GetMid() { return mIn + (mWidth / 2.0); }
  FLOAT GetLow() { return mIn; }
  FLOAT GetHigh() { return mOut; }
  FLOAT GetAverage(int index) { return mAverages[index]; }
  int GetNumParticles() { return mParticles.size(); }

private:
  FLOAT mIn = 0;
  FLOAT mOut = 0;
  FLOAT mWidth = 0.0;

  std::vector<Particle *> mParticles;
  FLOAT mAverages[RADIAL_QUANTITIES] = {};
};
