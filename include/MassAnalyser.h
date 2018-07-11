//===-- MassAnalyser.h ----------------------------------------------------===//
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
/// MassAnalyser.h retrieves the mass of different components from a simulation
/// and outputs the results into a single file.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Definitions.h"
#include "File.h"
#include "Particle.h"

struct MassComponent {
  FLOAT time = 0.0;
  FLOAT tot_mass = 0.0;
  FLOAT gas_mass = 0.0;
  FLOAT dust_mass = 0.0;
  FLOAT sink_mass = 0.0;
  FLOAT unique_sink_mass[16] = {0.0};
  FLOAT mdot = 0.0;
  int gas_num = 0;
  int dust_num = 0;
  int sink_num = 0;
};

class MassAnalyser {
public:
  MassAnalyser(std::string filename);
  ~MassAnalyser();

  void ExtractValues(SnapshotFile *file);
  void CalculateAccretionRate(void);
  bool Write(void);

private:
  std::ofstream mOutStream;
  std::string mFileName;
  std::vector<MassComponent> mMasses;
};
