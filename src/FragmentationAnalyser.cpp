//===-- FragmentationAnalyser.cpp -----------------------------------------===//
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
/// FragmentationAnalyser.cpp
///
//===----------------------------------------------------------------------===//

#include "FragmentationAnalyser.h"

FragmentationAnalyser::FragmentationAnalyser(NameData nd) : mNameData(nd) {
  mNameData.append += "fragmentation";
}

FragmentationAnalyser::~FragmentationAnalyser() {}

void FragmentationAnalyser::Append(SnapshotFile *file) {
  std::vector<Particle *> part = file->GetParticles();
  std::vector<Sink *> sink = file->GetSinks();
  Record r;
  r.time = file->GetTime();
  r.disc_mass = file->GetNumGas() * part[0]->GetM();
  r.star_mass = (sink.size() > 0) ? sink[0]->GetM() : 0.0f;

  for (int i = 0; i < part.size(); ++i) {
    Particle *p = part[i];
    if (p->GetD() > r.max_dens) {
      r.max_dens = p->GetD();
    }
    if (p->GetT() > r.max_temp) {
      r.max_temp = p->GetT();
    }
  }

  mRecords.push_back(r);
}

bool FragmentationAnalyser::Write() {
  std::string outputName = mNameData.dir + "/SPARGEL." + mNameData.id + "." +
                           mNameData.append + ".dat";
  mOutStream.open(outputName, std::ios::out);
  if (!mOutStream.is_open()) {
    std::cout << "   Could not open file " << outputName << " for writing!\n";
    return false;
  }

  for (int i = 0; i < mRecords.size(); ++i) {
    Record r = mRecords[i];
    mOutStream << r.time << "\t" << r.disc_mass << "\t" << r.star_mass << "\t"
               << r.max_dens << "\t" << r.max_temp << "\n";
  }
  mOutStream.close();

  std::cout << "   Successfully wrote " << outputName << "!\n";
  return true;
}