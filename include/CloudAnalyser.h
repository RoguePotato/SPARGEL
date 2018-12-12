//===-- CloudAnalyser.h ---------------------------------------------------===//
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
/// CloudAnalyser.h allows the analysis of a collapsing cloud snapshots where
/// the central density and temperature are recorded.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Definitions.h"
#include "File.h"
#include "Particle.h"

struct CentralValue {
  float density = 0.0;
  float temperature = 0.0;
};

class CloudAnalyser {
public:
  CloudAnalyser(NameData nd, const int avg);
  ~CloudAnalyser();

  void FindCentralQuantities(SnapshotFile *file);
  void CenterAroundDensest(SnapshotFile *file);
  bool Write();

private:
  NameData mNameData;
  int mAverage;
  std::vector<CentralValue> mMaxima;
  std::ofstream mOutStream;
};
