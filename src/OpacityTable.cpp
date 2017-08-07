//===-- OpacityTable.cpp --------------------------------------------------===//
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
/// OpacityTable.cpp
///
//===----------------------------------------------------------------------===//

#include "OpacityTable.h"

OpacityTable::OpacityTable() {

}

OpacityTable::~OpacityTable() {
  for (int i = 0; i < mNumDens; ++i) {
    delete[] mEnergy[i];
    delete[] mMu[i];
    delete[] mKappa[i];
    delete[] mKappar[i];
    delete[] mKappap[i];
    delete[] mGamma[i];
  }

  delete[] mEnergy;
  delete[] mMu;
  delete[] mKappa;
  delete[] mKappar;
  delete[] mKappa;
  delete[] mGamma;
}

bool OpacityTable::Read(std::string fileName, bool formatted) {
  mInStream.open(fileName.c_str(), std::ios::in);

  if (!mInStream.good()) return false;

  std::string line;
  uint32 i, j, l;
  real64 dens, temp, energy, mu, kappa, kappar, kappap, gamma;

  getline(mInStream, line);
  std::istringstream istr(line);
  istr >> mNumDens >> mNumTemp >> mFcol;

  mDens   = new real64[mNumDens];
  mTemp   = new real64[mNumTemp];
  mEnergy = new real64*[mNumDens];
  mMu     = new real64*[mNumDens];
  mGamma  = new real64*[mNumDens];
  mKappa  = new real64*[mNumDens];
  mKappar = new real64*[mNumDens];
  mKappap = new real64*[mNumDens];

  for (i = 0; i < mNumDens; ++i) {
    mEnergy[i] = new real64[mNumTemp];
    mMu[i]     = new real64[mNumTemp];
    mGamma[i]  = new real64[mNumTemp];
    mKappa[i]  = new real64[mNumTemp];
    mKappar[i] = new real64[mNumTemp];
    mKappap[i] = new real64[mNumTemp];
  }

  // read table
  i = 0;
  l = 0;
  j = 0;

  //---------------------------------------------------------------------------------------------
  while (getline(mInStream, line)) {
    std::istringstream istr(line);

    if (istr >> dens >> temp >> energy >> mu >> kappa >> kappar >> kappap >> gamma) {

      mEnergy[i][j] = energy;
      mMu[i][j]     = mu;
      mKappa[i][j]  = kappa;
      mKappar[i][j] = kappar;
      mKappap[i][j] = kappap;
      mGamma[i][j]  = gamma;

      if (l < mNumTemp) {
        mTemp[l] = log10(temp);
      }

      ++l;
      ++j;

      if (!(l % mNumTemp)) {
        mDens[i] = log10(dens);
        ++i;
        j = 0;
      }
    }
  }
  mInStream.close();

  return true;
}

real64 OpacityTable::GetMuBar(real64 density, real64 temperature) {
  return mMu[GetIDens(density)][GetITemp(temperature)];
}

real64 OpacityTable::GetGamma(real64 density, real64 temperature) {
  return mGamma[GetIDens(density)][GetITemp(temperature)];
}

real64 OpacityTable::GetTemp(real64 density, real64 energy) {
  real64 result = 0.0;
  energy *= 1E4; // cgs conversion

  real64 logdens = log10(density);
  int idens = GetIDens(logdens);

  if (energy == 0.0) return result;

  // gets nearest temperature in table from density and specific
  // internal energy
  int tempIndex = -1;
  real64 diff = 1e20;
  for (int i = 0; i < mNumTemp; ++i) {
    if (fabs(energy - mEnergy[idens][i]) < diff) {
      diff = fabs(energy - mEnergy[idens][i]);
      tempIndex = i;
    }
  }

  if (tempIndex > mNumTemp - 1) tempIndex = mNumTemp - 1;

  result = pow10(mTemp[tempIndex]);

  return result;
}

template <typename BidirectionalIterator, typename T>
BidirectionalIterator GetClosest(BidirectionalIterator first,
                                 BidirectionalIterator last,
                                 const T &value)
{
  BidirectionalIterator before = std::lower_bound(first, last, value);

  if (before == first) return first;
  if (before == last)  return --last;

  BidirectionalIterator after = before;
  --before;

  return (*after - value) < (value - *before) ? after : before;
}


template <typename BidirectionalIterator, typename T>
std::size_t GetClosestIndex(BidirectionalIterator first,
                            BidirectionalIterator last,
                            const T &value)
{
  return std::distance(first, GetClosest(first, last, value));
}

uint32 OpacityTable::GetIDens(const real64 density) {
  uint32 idens = GetClosestIndex(mDens, mDens + mNumDens, density);

  if (density < mDens[idens]) {
    idens = std::max((int) idens - 1, 0);
  }
  return idens;
}

uint32 OpacityTable::GetITemp(const real64 temperature) {
  uint32 itemp = GetClosestIndex(mTemp, mTemp + mNumTemp , temperature);

  if (temperature <= mTemp[itemp]) {
    itemp = std::max((int) itemp - 1, 0);
  }
  return itemp;
}
