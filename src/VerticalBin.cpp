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

VerticalBin::VerticalBin(FLOAT in, FLOAT out, FLOAT width) :
  mIn(in), mOut(out), mWidth(width) {
}

VerticalBin::~VerticalBin() {

}

void VerticalBin::CalculateValues(void) {
  if (mParticles.size() <= 0) return;

  for (int i = 0; i < mParticles.size(); ++i) {
    Particle *p = mParticles[i];

    mAverages[0] += p->GetD();
    mAverages[1] += p->GetT();
    mAverages[2] += p->GetM() * MSUN_TO_KG;
    mAverages[4] += p->GetTau();
  }

  for (int i = 0; i < 16; ++i) {
    mAverages[i] /= mParticles.size();
  }
}
