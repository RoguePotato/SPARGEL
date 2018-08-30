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
  const FLOAT MOD_ARRAY[3] = {0.1f, 1.0f, 10.0f};
  OpacityTable *mOpacity = NULL;
  FLOAT mDensMin = 0.0f;
  FLOAT mDensMax = 0.0f;
  FLOAT mTempMin = 0.0f;
  FLOAT mTempMax = 0.0f;
  int mDensBins = 0;
  int mTempBins = 0;

  std::string mName = "";
  std::vector<FLOAT> mDensities;
  std::vector<FLOAT> mTemperatures;

  void FillVectors();
  FLOAT CalculateDUDT(const FLOAT dens, const FLOAT temp, const FLOAT kappa);
};