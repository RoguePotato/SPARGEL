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

RadialBin::RadialBin(FLOAT starMass, FLOAT in, FLOAT out, FLOAT width) :
  mStarMass(starMass * MSUN_TO_KG), mIn(in), mOut(out), mWidth(width) {
}

RadialBin::~RadialBin() {

}

void RadialBin::CalculateValues(void) {
  if (mParticles.size() <= 0) return;

  // Create vertical bins
  for (int i = 0; i < 10; ++i) {
    mVerticalBins.push_back(new VerticalBin(i/10.0, (i + 1)/10.0, 0.1));
  }

  for (int i = 0; i < mParticles.size(); ++i) {
    Particle *p = mParticles[i];

    mAverages[0] += p->GetD();
    mAverages[1] += p->GetT();
    mAverages[2] += p->GetV().Norm();
    mAverages[3] += p->GetM() * MSUN_TO_KG;
    mAverages[4] += p->GetP();
    mAverages[5] += p->GetTau();
    mAverages[6] += p->GetHydroAcc();
    mAverages[7] += p->GetSigma();
    mAverages[8] += p->GetRealSigma();
    mAverages[9] += p->GetRealTau();

    // Vertical bins
    for (int j = 0; j < mVerticalBins.size(); ++j) {
      if (p->GetX().z > (j/10.0) && p->GetX().z < (j + 1)/10.0) {
        mVerticalBins.at(j)->AddParticle(p);
      }
    }
  }

  for (int i = 0; i < mVerticalBins.size(); ++i) {
    mVerticalBins[i]->CalculateValues();
  }

  // Toomre, requires total bin mass
  // for (int i = 0; i < mParticles.size(); ++i) {
  //   Particle *p = mParticles[i];
  //
  //   // omega = sqrt(GM / R^3) or v / R?
  //   FLOAT omega = sqrt((G * mStarMass) /
  //                   pow(p->GetR() * AU_TO_M, 3.0));
  //   // FLOAT omega = (p->GetS() * KMPERS_TO_MPERS) /
  //   //                 (p->GetR() * AU_TO_M);
  //
  //   FLOAT cs = sqrt((K * p->GetT()) / (MU * M_P));
  //
  //   FLOAT cd = mAverages[2] / (PI * (pow(mOut * AU_TO_M, 2.0) -
  //                                    pow(mIn * AU_TO_M, 2.0)));
  //
  //   mAverages[3] += (omega * cs) / (PI * G * cd);
  // }

  for (int i = 0; i < 16; ++i) {
    mAverages[i] /= mParticles.size();
  }
}
