//===-- RadialBin.h -------------------------------------------------------===//
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
/// RadialBin.cpp
///
//===----------------------------------------------------------------------===//

#include "RadialBin.h"

RadialBin::RadialBin(FLOAT starMass, int in, int out, FLOAT width) :
  mStarMass(starMass * MSUN_TO_KG), mIn(in), mOut(out), mWidth(width) {
}

RadialBin::~RadialBin() {

}

void RadialBin::CalculateValues(void) {
  if (mParticles.size() <= 0) return;

  for (int i = 0; i < mParticles.size(); ++i) {
    Particle *p = mParticles[i];

    mAverages[0] += p->GetD();
    // Midplane temperature
    if (p->GetX().z < 0.05 * p->GetR()) {
      mAverages[1] += p->GetT();
      ++mMidplanes;
    }
    mAverages[2] += p->GetM() * MSUN_TO_KG;
  }

  // Toomre, requires total bin mass
  for (int i = 0; i < mParticles.size(); ++i) {
    Particle *p = mParticles[i];

    // omega = sqrt(GM / R^3) or v / R?
    FLOAT omega = sqrt((G * mStarMass) /
                    pow(p->GetR() * AU_TO_M, 3.0));
    // FLOAT omega = (p->GetS() * KMPERS_TO_MPERS) /
    //                 (p->GetR() * AU_TO_M);

    FLOAT cs = sqrt((K * p->GetT()) / (MU * M_P));

    FLOAT cd = mAverages[2] / (PI * (pow(mOut * AU_TO_M, 2.0) -
                                     pow(mIn * AU_TO_M, 2.0)));

    mAverages[3] += (omega * cs) / (PI * G * cd);
  }

  for (int i = 0; i < 16; ++i) {
    mAverages[i] /= ((i != 1) ? mParticles.size() : mMidplanes);
  }
}