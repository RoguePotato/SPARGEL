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

  double GetEnergy(double density, double temperature);
  double GetMuBar(double density, double temperature);
  double GetGamma(double density, double temperature);
  double GetTemp(double density, double energy);

private:
  int mNumDens = 0;
  int mNumTemp = 0;
  double mFcol = 0.0;
  double *mDens;
  double *mTemp;
  double **mEnergy;
  double **mMu;
  double **mKappa;
  double **mKappar;
  double **mKappap;
  double **mGamma;

  int GetIDens(const double density);
  int GetITemp(const double temperature);
};
