//===-- Heatmap.h ---------------------------------------------------------===//
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
/// Heatmap.h allows the output of heatmap data based on a given quantity.
/// Multiple grid resolutions can be used to fill in gaps of particle sparse
/// areas.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Definitions.h"
#include "File.h"
#include "Particle.h"

class Heatmap {
public:
  Heatmap(const int res);
  ~Heatmap();

  void Create(SnapshotFile *file);
  void Output();

private:
  std::string mFileName;
  int mRes = 0;
  std::vector<std::vector<float>> mGrid;
  std::vector<std::vector<int>> mGridNum;

  int Bin(float d, float x);
};