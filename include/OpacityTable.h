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

#include "Definitions.h"
#include "File.h"

class OpacityTable : public File {
public:
  OpacityTable(std::string fileName, bool formatted);
  ~OpacityTable();

  bool Read();
  bool Write(std::string fileName, bool formatted) { return true; }

  int GetNDens() { return mNumDens; }

  FLOAT GetKappa(FLOAT density, FLOAT temperature);
  FLOAT GetKappar(FLOAT density, FLOAT temperature);
  FLOAT GetEnergy(FLOAT density, FLOAT temperature);
  FLOAT GetMuBar(FLOAT density, FLOAT temperature);
  FLOAT GetGamma(FLOAT density, FLOAT temperature);
  FLOAT GetGamma1(FLOAT density, FLOAT temperature);
  FLOAT GetTemp(FLOAT density, FLOAT energy);

private:
  int mNumDens = 0;
  int mNumTemp = 0;
  FLOAT mFcol = 0.0;
  FLOAT *mDens;
  FLOAT *mTemp;
  FLOAT **mEnergy;
  FLOAT **mMu;
  FLOAT **mKappa;
  FLOAT **mKappar;
  FLOAT **mKappap;
  FLOAT **mGamma;
  FLOAT **mGamma1;

  int GetIDens(const FLOAT density);
  int GetITemp(const FLOAT temperature);
};
