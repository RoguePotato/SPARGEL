//===-- RadialBin.h -------------------------------------------------------===//
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
/// RadialBin.h stores particles (and sinks if chosen) in containers constrained
/// by radial value. Average quantites are calculated within this class. The
/// average indices are as thus:
/// 0 : Density
/// 1 : Temperature
/// 2 : Mass
/// 3 : Toomre parameter
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Constants.h"
#include "Definitions.h"
#include "File.h"
#include "Parameters.h"
#include "Particle.h"
#include "VerticalBin.h"

class RadialBin {
public:
  RadialBin(Parameters *params, float starMass, float in, float out,
            float width);
  ~RadialBin();

  void AddParticle(Particle *p) { mParticles.push_back(p); }
  void AddSink(Sink *s) { mSinks.push_back(s); }

  void CalculateValues();

  float GetMid() { return mIn + (mWidth / 2.0); }
  float GetAverage(int index) { return mAverages[index]; }
  int GetNumParticles() { return mParticles.size(); }
  std::vector<VerticalBin *> GetVerticalBins() { return mVerticalBins; }

private:
  Parameters *mParams = NULL;

  float mIn = 0;
  float mOut = 0;
  float mWidth = 0.0;
  float mStarMass = 0.0;
  int mMidplanes = 0;

  std::vector<Particle *> mParticles;
  std::vector<Sink *> mSinks;
  std::vector<VerticalBin *> mVerticalBins;
  float mAverages[TOT_RAD_QUAN] = {};
};
