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
  OpacityTable();
  ~OpacityTable();

  bool Read(std::string fileName, bool formatted = true);
  bool Write(std::string fileName, bool formatted) { return true; }

  uint32 GetNDens() { return mNumDens; }

  real64 GetEnergy(real64 density, real64 temperature);
  real64 GetMuBar(real64 density, real64 temperature);
  real64 GetGamma(real64 density, real64 temperature);
  real64 GetTemp(real64 density, real64 energy);

private:
  uint32 mNumDens = 0;
  uint32 mNumTemp = 0;
  real64 mFcol = 0.0;
  real64 *mDens;
  real64 *mTemp;
  real64 **mEnergy;
  real64 **mMu;
  real64 **mKappa;
  real64 **mKappar;
  real64 **mKappap;
  real64 **mGamma;

  uint32 GetIDens(const real64 density);
  uint32 GetITemp(const real64 temperature);
};
