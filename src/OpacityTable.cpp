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

OpacityTable::OpacityTable(std::string fileName, bool formatted) {
  mNameData.name = fileName;
}

OpacityTable::~OpacityTable() {
  for (int i = 0; i < mNumDens; ++i) {
    delete[] mEnergy[i];
    delete[] mMu[i];
    delete[] mKappa[i];
    delete[] mKappar[i];
    delete[] mKappap[i];
    delete[] mGamma[i];
    delete[] mGamma1[i];
  }

  delete[] mEnergy;
  delete[] mMu;
  delete[] mKappa;
  delete[] mKappar;
  delete[] mKappap;
  delete[] mGamma;
  delete[] mGamma1;
  delete[] mTemp;
  delete[] mDens;
}

bool OpacityTable::Read() {
  mInStream.open(mNameData.name.c_str(), std::ios::in);

  if (!mInStream.is_open()) {
    std::cout << "Could not open EOS table " << mNameData.name
              << " for reading!\n\n";
    return false;
  }

  if (!mInStream.good()) {
    return false;
  }

  std::string line;
  int i, j, l;
  FLOAT dens, temp, energy, mu, kappa, kappar, kappap, gamma, gamma1;

  do {
    getline(mInStream, line);
  } while (line[0] == '#');
  std::istringstream istr(line);
  istr >> mNumDens >> mNumTemp >> mFcol;

  mDens = new FLOAT[mNumDens];
  mTemp = new FLOAT[mNumTemp];
  mEnergy = new FLOAT *[mNumDens];
  mMu = new FLOAT *[mNumDens];
  mKappa = new FLOAT *[mNumDens];
  mKappar = new FLOAT *[mNumDens];
  mKappap = new FLOAT *[mNumDens];
  mGamma = new FLOAT *[mNumDens];
  mGamma1 = new FLOAT *[mNumDens];

  for (i = 0; i < mNumDens; ++i) {
    mEnergy[i] = new FLOAT[mNumTemp];
    mMu[i] = new FLOAT[mNumTemp];
    mKappa[i] = new FLOAT[mNumTemp];
    mKappar[i] = new FLOAT[mNumTemp];
    mKappap[i] = new FLOAT[mNumTemp];
    mGamma[i] = new FLOAT[mNumTemp];
    mGamma1[i] = new FLOAT[mNumTemp];
  }

  // read table
  i = 0;
  l = 0;
  j = 0;

  //---------------------------------------------------------------------------------------------
  while (getline(mInStream, line)) {
    std::istringstream istr(line);

    if (istr >> dens >> temp >> energy >> mu >> kappa >> kappar >> kappap >>
        gamma >> gamma1) {

      mEnergy[i][j] = energy;
      mMu[i][j] = mu;
      mKappa[i][j] = kappa;
      mKappar[i][j] = kappar;
      mKappap[i][j] = kappap;
      mGamma[i][j] = gamma;
      mGamma1[i][j] = gamma1;

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

FLOAT OpacityTable::GetKappa(FLOAT density, FLOAT temperature) {
  return mKappa[GetIDens(log10(density))][GetITemp(log10(temperature))];
}

FLOAT OpacityTable::GetKappar(FLOAT density, FLOAT temperature) {
  return mKappar[GetIDens(log10(density))][GetITemp(log10(temperature))];
}

FLOAT OpacityTable::GetMuBar(FLOAT density, FLOAT temperature) {
  return mMu[GetIDens(log10(density))][GetITemp(log10(temperature))];
}

FLOAT OpacityTable::GetGamma(FLOAT density, FLOAT temperature) {
  return mGamma[GetIDens(log10(density))][GetITemp(log10(temperature))];
}

FLOAT OpacityTable::GetGamma1(FLOAT density, FLOAT temperature) {
  return mGamma1[GetIDens(log10(density))][GetITemp(log10(temperature))];
}

FLOAT OpacityTable::GetEnergy(FLOAT density, FLOAT temperature) {
  return mEnergy[GetIDens(log10(density))][GetITemp(log10(temperature))] *
         ERGPERG_TO_JPERKG;
}

FLOAT OpacityTable::GetTemp(FLOAT density, FLOAT energy) {
  FLOAT result = 0.0;
  energy /= ERGPERG_TO_JPERKG;

  FLOAT logdens = log10(density);
  int idens = GetIDens(logdens);

  if (energy == 0.0)
    return result;

  // gets nearest temperature in table from density and specific
  // internal energy
  int tempIndex = -1;
  FLOAT diff = 1e20;
  for (int i = 0; i < mNumTemp; ++i) {
    if (fabs(energy - mEnergy[idens][i]) < diff) {
      diff = fabs(energy - mEnergy[idens][i]);
      tempIndex = i;
    }
  }

  if (tempIndex > mNumTemp - 1)
    tempIndex = mNumTemp - 1;

  result = pow(10.0, mTemp[tempIndex]);

  return result;
}

template <typename BidirectionalIterator, typename T>
BidirectionalIterator GetClosest(BidirectionalIterator first,
                                 BidirectionalIterator last, const T &value) {
  BidirectionalIterator before = std::lower_bound(first, last, value);

  if (before == first)
    return first;
  if (before == last)
    return --last;

  BidirectionalIterator after = before;
  --before;

  return (*after - value) < (value - *before) ? after : before;
}

template <typename BidirectionalIterator, typename T>
std::size_t GetClosestIndex(BidirectionalIterator first,
                            BidirectionalIterator last, const T &value) {
  return std::distance(first, GetClosest(first, last, value));
}

int OpacityTable::GetIDens(const FLOAT density) {
  int idens = GetClosestIndex(mDens, mDens + mNumDens, density);

  if (density < mDens[idens]) {
    idens = std::max((int)idens - 1, 0);
  }
  return idens;
}

int OpacityTable::GetITemp(const FLOAT temperature) {
  int itemp = GetClosestIndex(mTemp, mTemp + mNumTemp, temperature);

  if (temperature <= mTemp[itemp]) {
    itemp = std::max((int)itemp - 1, 0);
  }
  return itemp;
}
