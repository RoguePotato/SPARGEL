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
  mFileName = fileName;
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

bool OpacityTable::Read() {
  mInStream.open(mFileName.c_str(), std::ios::in);

  if (!mInStream.good()) return false;

  std::cout << "Reading opacity table: " << mFileName << "\n";

  std::string line;
  int i, j, l;
  double dens, temp, energy, mu, kappa, kappar, kappap, gamma;

  getline(mInStream, line);
  std::istringstream istr(line);
  istr >> mNumDens >> mNumTemp >> mFcol;

  mDens   = new double[mNumDens];
  mTemp   = new double[mNumTemp];
  mEnergy = new double*[mNumDens];
  mMu     = new double*[mNumDens];
  mGamma  = new double*[mNumDens];
  mKappa  = new double*[mNumDens];
  mKappar = new double*[mNumDens];
  mKappap = new double*[mNumDens];

  for (i = 0; i < mNumDens; ++i) {
    mEnergy[i] = new double[mNumTemp];
    mMu[i]     = new double[mNumTemp];
    mGamma[i]  = new double[mNumTemp];
    mKappa[i]  = new double[mNumTemp];
    mKappar[i] = new double[mNumTemp];
    mKappap[i] = new double[mNumTemp];
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

double OpacityTable::GetMuBar(double density, double temperature) {
  return mMu[GetIDens(density)][GetITemp(temperature)];
}

double OpacityTable::GetGamma(double density, double temperature) {
  return mGamma[GetIDens(density)][GetITemp(temperature)];
}

double OpacityTable::GetEnergy(double density, double temperature) {
  return mEnergy[GetIDens(density)][GetITemp(temperature)];
}

double OpacityTable::GetTemp(double density, double energy) {
  double result = 0.0;
  energy *= 1E4; // cgs conversion

  double logdens = log10(density);
  int idens = GetIDens(logdens);

  if (energy == 0.0) return result;

  // gets nearest temperature in table from density and specific
  // internal energy
  int tempIndex = -1;
  double diff = 1e20;
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

int OpacityTable::GetIDens(const double density) {
  int idens = GetClosestIndex(mDens, mDens + mNumDens, density);

  if (density < mDens[idens]) {
    idens = std::max((int) idens - 1, 0);
  }
  return idens;
}

int OpacityTable::GetITemp(const double temperature) {
  int itemp = GetClosestIndex(mTemp, mTemp + mNumTemp , temperature);

  if (temperature <= mTemp[itemp]) {
    itemp = std::max((int) itemp - 1, 0);
  }
  return itemp;
}
