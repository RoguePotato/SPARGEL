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
  VerticalBin(float in, float out, float width);
  ~VerticalBin();

  void AddParticle(Particle *p) { mParticles.push_back(p); }

  void CalculateValues();

  float GetMid() { return mIn + (mWidth / 2.0); }
  float GetLow() { return mIn; }
  float GetHigh() { return mOut; }
  float GetAverage(int index) { return mAverages[index]; }
  int GetNumParticles() { return mParticles.size(); }

private:
  float mIn = 0;
  float mOut = 0;
  float mWidth = 0.0;

  std::vector<Particle *> mParticles;
  float mAverages[TOT_RAD_QUAN] = {};
};
