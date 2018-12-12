//===-- FragmentationAnalyser.h -------------------------------------------===//
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
/// FragmentationAnalyser.h pertains to discs and outputs are series from a set
/// of snapshots which record the time, maximum density and temperature as well
/// as the disc mass and stellar mass.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Constants.h"
#include "Definitions.h"
#include "File.h"
#include "Particle.h"

class FragmentationAnalyser {
public:
  FragmentationAnalyser(NameData nd);
  ~FragmentationAnalyser();

  void Append(SnapshotFile *file);
  bool Write();

private:
  struct Record {
    float time = 0.0f;
    float max_dens = 0.0f;
    float max_temp = 0.0f;
    float disc_mass = 0.0f;
    float star_mass = 0.0f;
  };

  NameData mNameData;
  std::vector<Record> mRecords;
  std::ofstream mOutStream;
};