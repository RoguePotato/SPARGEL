//===-- VerticalBin.h -----------------------------------------------------===//
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
/// VerticalBin.cpp
///
//===----------------------------------------------------------------------===//

#include "VerticalBin.h"

VerticalBin::VerticalBin(float in, float out, float width)
    : mIn(in), mOut(out), mWidth(width) {}

VerticalBin::~VerticalBin() {}

void VerticalBin::CalculateValues() {
  if (mParticles.size() <= 0)
    return;

  for (int i = 0; i < mParticles.size(); ++i) {
    Particle *p = mParticles[i];

    mAverages[0] += p->GetD();
    mAverages[1] += p->GetT();
    mAverages[2] += p->GetV().Norm();
    mAverages[3] += p->GetQ();
    mAverages[4] += p->GetP();
    mAverages[5] += p->GetTau();
    mAverages[7] += p->GetSigma();
    mAverages[9] += p->GetDUDT();
    mAverages[11] += p->GetCS();
    mAverages[12] += p->GetOmega();
    mAverages[13] += p->GetM();
    mAverages[14] += p->GetBeta();
    mAverages[15] += p->GetU();
  }

  for (int i = 0; i < TOT_RAD_QUAN; ++i) {
    mAverages[i] /= mParticles.size();
  }
}
