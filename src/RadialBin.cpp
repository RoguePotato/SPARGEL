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

RadialBin::RadialBin
(Parameters *params,
 FLOAT starMass,
 FLOAT in,
 FLOAT out,
 FLOAT width) :
  mParams(params),
  mStarMass(starMass * MSUN_TO_KG), mIn(in), mOut(out), mWidth(width) {
}

RadialBin::~RadialBin() {
  for (int i = 0; i < mVerticalBins.size(); ++i) {
    delete mVerticalBins[i];
  }
  mVerticalBins.clear();
}

void RadialBin::CalculateValues(void) {
  if (mParticles.size() <= 0) return;

  // // Create vertical bins
  FLOAT height_lo = mParams->GetFloat("HEIGHT_LO");
  FLOAT height_hi = mParams->GetFloat("HEIGHT_HI");
  int vert_bins = mParams->GetInt("VERTICAL_BINS");
  FLOAT bin_height = (height_hi - height_lo) / vert_bins;
  for (FLOAT i = height_lo; i < height_hi; i += bin_height) {
    mVerticalBins.push_back(new VerticalBin(i, i + bin_height, bin_height));
  }

  for (int i = 0; i < mParticles.size(); ++i) {
    Particle *p = mParticles[i];
    FLOAT z = fabs(p->GetX().z);

    mAverages[0] += p->GetD();
    mAverages[1] += p->GetT();
    mAverages[2] += p->GetV().Norm();
    mAverages[3] += p->GetQ();
    mAverages[4] += p->GetP();
    mAverages[5] += p->GetTau();
    mAverages[6] += p->GetRealTau();
    mAverages[7] += p->GetSigma();
    mAverages[8] += p->GetRealSigma();
    mAverages[9] += p->GetDUDT();
    mAverages[10] += p->GetRealDUDT();

    // Vertical bins
    for (int j = 0; j < mVerticalBins.size(); ++j) {
      FLOAT lo = mVerticalBins[j]->GetLow();
      FLOAT hi = mVerticalBins[j]->GetHigh();

      if (z > lo && z <= hi) mVerticalBins.at(j)->AddParticle(p);
    }
  }

  for (int i = 0; i < mVerticalBins.size(); ++i) {
    mVerticalBins[i]->CalculateValues();
  }

  for (int i = 0; i < 16; ++i) {
    mAverages[i] /= mParticles.size();
  }
}
