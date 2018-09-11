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

CloudAnalyser::CloudAnalyser(NameData nd) : mNameData(nd) {
  mNameData.append += "cloud";
}

CloudAnalyser::~CloudAnalyser() { mMaxima.clear(); }

void CloudAnalyser::FindCentralQuantities(SnapshotFile *file) {
  std::vector<Particle *> part = file->GetParticles();

  std::sort(part.begin(), part.end(),
            [](Particle *a, Particle *b) { return b->GetD() < a->GetD(); });

  int avgNum = (int)sqrtf(part.size());
  CentralValue m;
  for (int i = 0; i < avgNum; ++i) {
    m.density += part[i]->GetD();
    m.temperature += part[i]->GetT();
  }
  m.density /= avgNum;
  m.temperature /= avgNum;
  mMaxima.push_back(m);

  file->SetParticles(part);
}

void CloudAnalyser::CenterAroundDensest(SnapshotFile *file) {
  std::vector<Particle *> part = file->GetParticles();

  Vec3 pos = part[0]->GetX();
  Vec3 vel = part[0]->GetV();

  for (int i = 0; i < part.size(); ++i) {
    Vec3 posDiff = part[i]->GetX() - pos;
    Vec3 velDiff = part[i]->GetV() - vel;
    part[i]->SetX(posDiff);
    part[i]->SetV(velDiff);
  }

  file->SetParticles(part);
}

bool CloudAnalyser::Write() {
  std::string outputName = mNameData.dir + "/SPARGEL." + mNameData.id + "." +
                           mNameData.append + ".dat";
  mOutStream.open(outputName, std::ios::out);
  if (!mOutStream.is_open()) {
    std::cout << "   Could not open file " << outputName << " for writing!\n";
    return false;
  }

  std::sort(mMaxima.begin(), mMaxima.end(), [](CentralValue a, CentralValue b) {
    return b.density > a.density;
  });

  for (int i = 0; i < mMaxima.size(); ++i) {
    CentralValue cur = mMaxima[i];
    mOutStream << cur.density << "\t" << cur.temperature << "\n";
  }
  mOutStream.close();

  std::cout << "   File output      : " << outputName << "\n";
  return true;
}
