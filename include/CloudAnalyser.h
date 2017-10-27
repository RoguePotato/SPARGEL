//===-- CloudAnalyser.h -----------------------------------------------------===//
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
  double density;
  double temperature;
};

class CloudAnalyser {
public:
  CloudAnalyser(std::string fileName);
  ~CloudAnalyser();

  void FindCentralQuantities(SnapshotFile *file);
  void CenterAroundDensest(SnapshotFile *file);
  bool Write();

private:
  std::ofstream mOutStream;
  std::string mFileName;
  std::vector<CentralValue> mMaxima;
};
