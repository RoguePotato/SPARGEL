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

DiscAnalyser::DiscAnalyser(Parameters *params) : mParams(params) {}

DiscAnalyser::~DiscAnalyser() {}

void DiscAnalyser::Center(SnapshotFile *file, int sinkIndex, Vec3 posCenter,
                          int densest) {
  std::vector<Particle *> part = file->GetParticles();
  std::vector<Sink *> sink = file->GetSinks();
  Vec3 dX = {0.0, 0.0, 0.0};
  std::string appendage = ".centered.";

  // Return if there is no position to center around
  if (posCenter.Norm() == 0.0 && (sinkIndex < 0 || sinkIndex >= sink.size()) &&
      !densest) {
    return;
  }

  if (densest) {
    // Find the densest particle position.
    float max_dens = -1E30;
    for (int i = 0; i < part.size(); ++i) {
      Particle *p = part.at(i);
      if (p->GetD() > max_dens) {
        max_dens = p->GetD();
        dX = p->GetX();
      }
    }
    appendage += "densest";
  } else {
    if (posCenter.Norm() == 0.0) {
      dX = sink[sinkIndex]->GetX();
      appendage += std::to_string(sinkIndex);
    } else {
      dX = posCenter;
      appendage += mParams->GetString("CENTER_LABEL");
    }
    if (dX.z == 0.0) {
      dX.z = sink[sinkIndex]->GetX().z;
    }
  }

  // TODO: Instead of using R, maybe replace all with GetX().Norm().
  for (int i = 0; i < part.size(); ++i) {
    Vec3 newX = part[i]->GetX() - dX;
    part[i]->SetX(newX);
    part[i]->SetR(part[i]->GetX().Norm());
  }

  for (int i = 0; i < sink.size(); ++i) {
    Vec3 newX = sink[i]->GetX() - dX;
    sink[i]->SetX(newX);
    sink[i]->SetR(sink[i]->GetX().Norm());
  }

  file->SetParticles(part);
  file->SetSinks(sink);
  file->SetNameDataAppend(appendage);
}
