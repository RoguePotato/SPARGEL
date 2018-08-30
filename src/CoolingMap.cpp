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
  for (int m = 0; m < 3; ++m) {
    out.open(mName + "_" + std::to_string(m) + ".dat");
    for (int t = 0; t < mTemperatures.size(); ++t) {
      FLOAT temp = mTemperatures[t];
      for (int d = 0; d < mDensities.size(); ++d) {
        FLOAT dens = mDensities[d];
        FLOAT kappa = mOpacity->GetKappa(dens, temp) * MOD_ARRAY[m];
        out << dens << "\t" << temp << "\t"
            << log10(CalculateDUDT(dens, temp, kappa)) << "\n";
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
  FLOAT sigma = dens * AU_TO_CM;
  FLOAT numer = 4.0f * SB * powf(temp, 4.0f);
  FLOAT denom = (sigma * sigma * kappa) + (1.0f / kappa);
  return numer / denom;
}