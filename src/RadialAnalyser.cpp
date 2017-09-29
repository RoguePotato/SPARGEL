//===-- RadialAnalyser.cpp --------------------------------------------------===//
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
/// RadialAnalyser.cpp
///
//===----------------------------------------------------------------------===//

#include "RadialAnalyser.h"

RadialAnalyser::RadialAnalyser(int in, int out, int bins) :
  mIn(in), mOut(out), mBins(bins) {
    mWidth = (mOut - mIn) / mBins;
}

RadialAnalyser::~RadialAnalyser() {
  for (int i = 0; i < mRadialBins.size(); ++i) {
    delete mRadialBins[i];
  }
  mRadialBins.clear();
}

void RadialAnalyser::Run(SnapshotFile *file) {
  // Create bins
  for (int i = mIn; i < mOut; i += mWidth) {
    FLOAT inner = mIn + (i * mWidth);
    FLOAT outer = mIn + ((i + 1) * mWidth);
    mRadialBins.push_back(new RadialBin(file->GetSinks()[0]->GetM(),
                                        inner, outer, mWidth));
  }

  // Allocate particles to bins
  std::vector<Particle *> part = file->GetParticles();
  std::vector<Sink *> sink = file->GetSinks();
  for (int i = 0; i < part.size(); ++i) {
    FLOAT r = part[i]->GetX().Norm();

    int binID = GetBinID(r - mIn);

    if (binID < mRadialBins.size() && binID >= 0) {
      mRadialBins.at(binID)->AddParticle(part[i]);
    }
  }

  // Calculate radial averages
  for (int i = 0; i < mRadialBins.size(); ++i) {
    mRadialBins[i]->CalculateValues();
  }

  // Output
  // 1: Radius
  // 2: Density
  // 3: Temperature
  // 4: Mass
  // 5: Toomre
  NameData nd = file->GetNameData();
  std::string outputName = nd.dir + "/" + nd.id + "." +
  nd.format + "." + nd.snap + nd.append + ".radial";

  std::ofstream out;
  out.open(outputName);
  for (int i = 0; i < mRadialBins.size(); ++i) {
    RadialBin *b = mRadialBins[i];
    if (b->GetNumParticles() <= 16) continue;
    out << b->GetMid() << "\t";
    for (int j = 0; j < 16; ++j) {
      out << b->GetAverage(j) << "\t";
    }
    out << "\n";
  }
  out.close();
}

int RadialAnalyser::GetBinID(FLOAT r) {
  int intRadius = r * 1E10;
  int intWidth = mWidth * 1E10;

  int diff = intRadius % intWidth;
  FLOAT floatDiff = diff / 1E10;

  FLOAT rounded = r - floatDiff;

  return (rounded / mWidth);
}
