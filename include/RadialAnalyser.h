//===-- RadialAnalyser.h --------------------------------------------------===//
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
/// RadialAnalyser.h allows the azimuthally-averaged radial analysis around
/// the origin of the system. This can be combined with disc centering for
/// example to analyse the region around the central object or a formed sink.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Definitions.h"
#include "File.h"
#include "RadialBin.h"

class RadialAnalyser {
public:
  RadialAnalyser(int in, int out, int bins);
  ~RadialAnalyser();

  void Run(SnapshotFile *file);

private:
  int mIn = 0;
  int mOut = 0;
  int mBins = 0;
  FLOAT mWidth = 0.0f;

  std::vector<RadialBin *> mRadialBins;

  int GetBinID(FLOAT r);
};
