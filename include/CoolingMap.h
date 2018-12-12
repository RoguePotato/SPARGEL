//===-- CoolingMap.h ------------------------------------------------------===//
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
/// CoolingMap.h callows the generation of a du/dt heatmap based on the cooling
/// method of Stamatellos or Lombardi. Opacity modifications can be applied.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Constants.h"
#include "Definitions.h"
#include "OpacityTable.h"
#include "Parameters.h"

class CoolingMap {
public:
  CoolingMap(OpacityTable *opacity, Parameters *params);
  ~CoolingMap();

  void Output();

private:
  const float MOD_ARRAY[3] = {0.1f, 1.0f, 10.0f};
  OpacityTable *mOpacity = NULL;
  float mDensMin = 0.0f;
  float mDensMax = 0.0f;
  float mTempMin = 0.0f;
  float mTempMax = 0.0f;
  int mDensBins = 0;
  int mTempBins = 0;

  std::string mName = "";
  std::vector<float> mDensities;
  std::vector<float> mTemperatures;

  void FillVectors();
  float CalculateDUDT(const float dens, const float temp, const float kappa);
};