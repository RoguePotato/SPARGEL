//===-- OpacityTable.h ----------------------------------------------------===//
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
/// OpacityTable.h contains the functions to read in a given opacity table.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Constants.h"
#include "Definitions.h"
#include "File.h"

class OpacityTable : public File {
public:
  OpacityTable(std::string fileName, bool formatted, float opacityMod);
  ~OpacityTable();

  bool Read();
  bool Write(std::string fileName, bool formatted) { return true; }

  int GetNDens() { return mNumDens; }

  float GetKappa(float density, float temperature);
  float GetKappar(float density, float temperature);
  float GetEnergy(float density, float temperature);
  float GetMuBar(float density, float temperature);
  float GetGamma(float density, float temperature);
  float GetGamma1(float density, float temperature);
  float GetTemp(float density, float energy);

private:
  int mNumDens = 0;
  int mNumTemp = 0;
  float mFcol = 0.0;
  float *mDens;
  float *mTemp;
  float **mEnergy;
  float **mMu;
  float **mKappa;
  float **mKappar;
  float **mKappap;
  float **mGamma;
  float **mGamma1;
  float mOpacityMod = 1.0;

  int GetIDens(const float density);
  int GetITemp(const float temperature);
};
