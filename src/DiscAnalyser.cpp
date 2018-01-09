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

DiscAnalyser::DiscAnalyser(Parameters *params) : mParams(params) {

}

DiscAnalyser::~DiscAnalyser() {

}

void DiscAnalyser::Center(SnapshotFile *file, int sinkIndex, Vec3 posCenter) {
  std::vector<Particle *> part = file->GetParticles();
  std::vector<Sink *> sink = file->GetSinks();
  Vec3 dX = { 0.0, 0.0, 0.0 };
  std::string appendage = ".centered.";

  // Return if there is no position to center around
  if (posCenter.Norm() == 0.0 && (sinkIndex < 0 || sinkIndex >= sink.size())) {
    return;
  }
  if (posCenter.Norm() == 0.0) {
    dX = sink[sinkIndex]->GetX();
    appendage += std::to_string(sinkIndex);
  }
  else {
    dX = posCenter;
    appendage += mParams->GetString("CENTER_LABEL");
  }
  if (dX.z == 0.0) dX.z = sink[sinkIndex]->GetX().z;

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
  file->SetNameDataAppend(appendage);
}
