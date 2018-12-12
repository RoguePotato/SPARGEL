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
/// RadialAnalyser.h allows the azimuthally-averaged/spherical radial analysis
/// around the origin of the system. This can be combined with disc centering
/// for example to analyse the region around the central object or a formed
/// sink or a particle at the center of a dense clump.
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

  int mSpherical = 0.0;
  float mIn = 0.0;
  float mOut = 0.0;
  int mBins = 0;
  int mLog = 0;
  int mVert = 0;
  float mWidth = 0.0f;

  std::vector<RadialBin *> mRadialBins;

  int GetBinID(float r);
};
