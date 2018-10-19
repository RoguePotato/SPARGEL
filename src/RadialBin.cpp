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

RadialBin::RadialBin(Parameters *params, FLOAT starMass, FLOAT in, FLOAT out,
                     FLOAT width)
    : mParams(params), mStarMass(starMass * MSUN_TO_KG), mIn(in), mOut(out),
      mWidth(width) {}

RadialBin::~RadialBin() {
  for (int i = 0; i < mVerticalBins.size(); ++i) {
    delete mVerticalBins[i];
  }
  mVerticalBins.clear();
}

void RadialBin::CalculateValues() {
  if (mParticles.size() <= 0)
    return;

  // Create vertical bins
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
    mAverages[2] += p->GetH();
    mAverages[3] += p->GetV().Norm();
    mAverages[4] += p->GetQ();
    mAverages[5] += p->GetP();
    mAverages[6] += p->GetTau();
    mAverages[7] += p->GetSigma();
    mAverages[8] += p->GetDUDT();
    mAverages[9] += p->GetCS();
    mAverages[10] += p->GetOmega();
    mAverages[11] += p->GetM();
    mAverages[12] += p->GetBeta();
    mAverages[13] += p->GetU();
    for (int j = 0; j < EXTRA_DATA; ++j) {
      mAverages[RADIAL_QUAN + j] += p->GetExtra(j);
    }

    // Vertical bins
    for (int j = 0; j < mVerticalBins.size(); ++j) {
      FLOAT lo = mVerticalBins[j]->GetLow();
      FLOAT hi = mVerticalBins[j]->GetHigh();

      if (z > lo && z <= hi)
        mVerticalBins.at(j)->AddParticle(p);
    }
  }

  for (int i = 0; i < mVerticalBins.size(); ++i) {
    mVerticalBins[i]->CalculateValues();
  }
  
  // TODO: Calculate Toomre parameter properly.
  mAverages[4] = (mAverages[9] * mAverages[10]) / (PI * G * mAverages[7]);

  for (int i = 0; i < TOT_RAD_QUAN; ++i) {
    mAverages[i] /= mParticles.size();
  }
}
