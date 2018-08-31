//===-- CoolingMap.cpp ----------------------------------------------------===//
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
/// CoolingMap.cpp
///
//===----------------------------------------------------------------------===//

#include "CoolingMap.h"

CoolingMap::CoolingMap(OpacityTable *opacity, Parameters *params)
    : mOpacity(opacity) {
  mDensMin = params->GetFloat("DENS_MIN");
  mDensMax = params->GetFloat("DENS_MAX");
  mTempMin = params->GetFloat("TEMP_MIN");
  mTempMax = params->GetFloat("TEMP_MAX");
  mDensBins = params->GetInt("DENS_BINS");
  mTempBins = params->GetInt("TEMP_BINS");
  mName = params->GetString("COOLING_MAP_NAME");

  this->FillVectors();
}

CoolingMap::~CoolingMap() {}

void CoolingMap::Output() {
  std::ofstream out;
  // TODO: User based opacity modifiers.

  // Output heatmap
  for (int m = 0; m < 3; ++m) {
    out.open(mName + "_" + std::to_string(m) + ".dat");
    for (int t = 0; t < mTemperatures.size(); ++t) {
      FLOAT temp = mTemperatures[t];
      for (int d = 0; d < mDensities.size(); ++d) {
        FLOAT dens = mDensities[d];
        FLOAT kappa = mOpacity->GetKappa(dens, temp) * MOD_ARRAY[m];
        FLOAT dudt = CalculateDUDT(dens, temp, kappa);
        out << dens << "\t" << temp << "\t" << log10(dudt) << "\n";
      }
    }
    out.close();
  }

  // Output optically thin/thick contour
  for (int m = 0; m < 3; ++m) {
    out.open(mName + "_" + std::to_string(m) + "_contour.dat");
    for (int t = 0; t < mTemperatures.size(); ++t) {
      FLOAT temp = mTemperatures[t];
      FLOAT cur_tau = 0.0f;
      for (int d = 0; d < mDensities.size(); ++d) {
        FLOAT dens = mDensities[d];
        FLOAT kappa = mOpacity->GetKappa(dens, temp) * MOD_ARRAY[m];
        FLOAT tau = kappa * dens * AU_TO_CM;
        if (cur_tau < 1.0f && tau > 1.0f) {
          out << 0.5f * (dens + mDensities[d - 1]) << "\t"
              << 0.5f * (temp + mTemperatures[t - 1]) << "\t"
              << "\n";
          break;
        }
        cur_tau = tau;
      }
    }
    out.close();
  }
}

void CoolingMap::FillVectors() {
  float dens_step = fabs(mDensMax - mDensMin) / (float)mDensBins;
  for (int i = 0; i < mDensBins; ++i) {
    mDensities.push_back(powf(10.0, mDensMin + i * dens_step));
  }

  float temp_step = fabs(mTempMax - mTempMin) / (float)mTempBins;
  for (int i = 0; i < mTempBins; ++i) {
    mTemperatures.push_back(powf(10.0, mTempMin + i * temp_step));
  }
}

FLOAT CoolingMap::CalculateDUDT(const FLOAT dens, const FLOAT temp,
                                const FLOAT kappa) {
  const FLOAT sigma = dens * AU_TO_CM;
  const FLOAT numer = 4.0f * SB * powf(temp, 4.0f);
  const FLOAT denom = (sigma * sigma * kappa) + (1.0f / kappa);
  return numer / denom;
}