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
#include "Parameters.h"
#include "RadialBin.h"
#include "VerticalBin.h"

class RadialAnalyser {
public:
  RadialAnalyser(Parameters *params);
  ~RadialAnalyser();

  void Run(SnapshotFile *file);

private:
  Parameters *mParams = NULL;

  FLOAT mIn = 0.0;
  FLOAT mOut = 0.0;
  int mBins = 0;
  int mLog = 0;
  int mVert = 0;
  FLOAT mWidth = 0.0f;

  std::vector<RadialBin *> mRadialBins;

  int GetBinID(FLOAT r);
};
