//===-- MassAnalyser.cpp --------------------------------------------------===//
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
/// MassAnalyser.cpp
///
//===----------------------------------------------------------------------===//

#include "MassAnalyser.h"

MassAnalyser::MassAnalyser(std::string filename) : mFileName(filename) {}

MassAnalyser::~MassAnalyser() {}

void MassAnalyser::ExtractValues(SnapshotFile *file) {
  std::vector<Particle *> part = file->GetParticles();
  std::vector<Sink *> sinks = file->GetSinks();
  MassComponent mc = {};
  mc.time = file->GetTime();

  for (int i = 0; i < part.size(); ++i) {
    Particle *p = part[i];
    if (p->GetType() == GAS_TYPE) {
      mc.gas_mass += p->GetM();
      mc.gas_num++;
    } else if (p->GetType() == DUST_TYPE) {
      mc.dust_mass += p->GetM();
      mc.dust_num++;
    }
    mc.tot_mass += p->GetM();
  }

  for (int i = 0; i < sinks.size(); ++i) {
    mc.tot_mass += sinks[i]->GetM();
    mc.sink_mass += sinks[i]->GetM();
    mc.unique_sink_mass[i] = sinks[i]->GetM();
    mc.sink_num++;
  }

  mMasses.push_back(mc);
}

bool MassAnalyser::Write() {
  std::sort(mMasses.begin(), mMasses.end(),
            [](MassComponent a, MassComponent b) { return b.time > a.time; });

  mOutStream.open(mFileName);
  for (int i = 0; i < mMasses.size(); ++i) {
    MassComponent mc = mMasses[i];
    mOutStream << mc.time << "\t" << mc.tot_mass << "\t" << mc.gas_mass << "\t"
               << mc.dust_mass << "\t" << mc.sink_mass << "\t" << mc.gas_num
               << "\t" << mc.dust_num << "\t" << mc.sink_num << "\t";
    for (int j = 0; j < 16; ++j) {
      mOutStream << mc.unique_sink_mass[j] << "\t";
    }
    mOutStream << "\n";
  }
  mOutStream.close();
}