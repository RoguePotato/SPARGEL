//===-- CloudAnalyser.cpp -------------------------------------------------===//
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
/// CloudAnalyser.cpp
///
//===----------------------------------------------------------------------===//

#include "CloudAnalyser.h"

CloudAnalyser::CloudAnalyser(std::string fileName) : mFileName(fileName) {

}

CloudAnalyser::~CloudAnalyser() {
  mMaxima.clear();
}

void CloudAnalyser::FindCentralQuantities(SnapshotFile *file) {
  std::vector<Particle *> part = file->GetParticles();

  std::sort(part.begin(), part.end(),
            [](Particle *a, Particle *b) { return b->GetD() < a->GetD(); });

  CentralValue m;
  for (int i = 0; i < 128; ++i) {
    m.density += part[i]->GetD();
    m.temperature += part[i]->GetT();
  }
  m.density /= 128.0;
  m.temperature /= 128.0;
  mMaxima.push_back(m);
}

bool CloudAnalyser::Write() {
  mOutStream.open(mFileName, std::ios::out);
  if (!mOutStream.is_open()) {
    std::cout << "   Could not open file " << mFileName
              << " for writing!\n";
    return false;
  }

  std::sort(mMaxima.begin(), mMaxima.end(),
            [](CentralValue a,
               CentralValue b) { return b.density > a.density; });

  for (int i = 0; i < mMaxima.size(); ++i) {
    CentralValue cur = mMaxima[i];
    mOutStream << cur.density << "\t" << cur.temperature << "\n";
  }
  mOutStream.close();
}
