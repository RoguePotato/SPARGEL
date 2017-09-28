//===-- DiscAnalyser.cpp --------------------------------------------------===//
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
/// DiscAnalyser.cpp
///
//===----------------------------------------------------------------------===//

#include "DiscAnalyser.h"

DiscAnalyser::DiscAnalyser() {

}

DiscAnalyser::~DiscAnalyser() {

}

void DiscAnalyser::Center(SnapshotFile *file, int center) {
  std::vector<Particle *> part = file->GetParticles();
  std::vector<Sink *> sink = file->GetSinks();
  Vec3 dX = sink[center]->GetX();
  if (center < 0 || center >= sink.size()) return; // TODO: error message

  for (int i = 0; i < part.size(); ++i) {
    Vec3 newX = part[i]->GetX() - dX;
    part[i]->SetX(newX);
  }

  for (int i = 0; i < sink.size(); ++i) {
    Vec3 newX = sink[i]->GetX() - dX;
    sink[i]->SetX(newX);
  }

  file->SetParticles(part);
  file->SetSinks(sink);
  file->SetNameDataAppend(".centered." + std::to_string(center));
}

void DiscAnalyser::Radial(SnapshotFile *file) {
  
}
