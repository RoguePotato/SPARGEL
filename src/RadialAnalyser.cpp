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

RadialAnalyser::RadialAnalyser(int in, int out, int bins, int log, int vert) :
  mIn(in), mOut(out), mBins(bins), mLog(log), mVert(vert) {
    mWidth = (FLOAT) (mOut - mIn) / mBins;
}

RadialAnalyser::~RadialAnalyser() {
  for (int i = 0; i < mRadialBins.size(); ++i) {
    delete mRadialBins[i];
  }
  mRadialBins.clear();
}

void RadialAnalyser::Run(SnapshotFile *file) {
  // Create bins
  if (mLog) {
    for (FLOAT i = mIn; i < mOut; i += mWidth) {
      FLOAT inner = pow(10.0, i);
      FLOAT outer = pow(10.0, i +  mWidth);
      mRadialBins.push_back(new RadialBin(0.0, inner, outer, mWidth));
    }
  }
  else {
    for (FLOAT i = mIn; i < mOut; i += mWidth) {
      FLOAT inner = mIn + (i * mWidth);
      FLOAT outer = mIn + ((i + 1) * mWidth);
      mRadialBins.push_back(new RadialBin(file->GetSinks()[0]->GetM(),
                                          inner, outer, mWidth));
    }
  }

  // Allocate particles to bins
  std::vector<Particle *> part = file->GetParticles();
  std::vector<Sink *> sink = file->GetSinks();
  for (int i = 0; i < part.size(); ++i) {
    FLOAT r = part[i]->GetX().Norm();
    if (mLog) r = log10(r);

    int binID = GetBinID(r - mIn);

    if (binID < mRadialBins.size() && binID >= 0) {
      mRadialBins.at(binID)->AddParticle(part[i]);
    }
  }

  // Calculate radial averages
  for (int i = 0; i < mRadialBins.size(); ++i) {
    mRadialBins[i]->CalculateValues();
  }

  // Output radial
  // 1: Radius
  // 2: Density
  // 3: Temperature
  // 4: Velocity
  // 5: Mass
  // 6: Pressure
  // 7: Optical depth
  // 8: Hydrodynamical acceleration
  NameData nd = file->GetNameData();
  if (nd.dir == "") nd.dir = ".";
  std::string outputName = nd.dir + "/SPARGEL." + nd.id + "." +
  nd.format + "." + nd.snap + nd.append + ".radial";

  std::ofstream out;
  out.open(outputName);
  for (int i = 0; i < mRadialBins.size(); ++i) {
    RadialBin *b = mRadialBins[i];
    if (b->GetNumParticles() <= 0) continue;
    out << b->GetMid() << "\t";
    for (int j = 0; j < 16; ++j) {
      out << b->GetAverage(j) << "\t";
    }
    out << "\n";
  }
  out.close();

  // Output vertical bins
  if (mVert) {
    for (int r = 0; r < mRadialBins.size(); ++r) {
      RadialBin *b = mRadialBins[r];
      if (b->GetNumParticles() <= 0) continue;

      if (nd.dir == "") nd.dir = ".";
      outputName = nd.dir + "/SPARGEL." + nd.id + "." +
      nd.format + "." + nd.snap + nd.append + ".vertical." + std::to_string(r);
      out.open(outputName);
      for (int i = 0; i < b->GetVerticalBins().size(); ++i) {
        VerticalBin *v = b->GetVerticalBins()[i];
        if (v->GetNumParticles() <= 0) continue;

        out << v->GetMid() << "\t";
        for (int j = 0; j < 16; ++j) {
          out << v->GetAverage(j) << "\t";
        }
        out << "\n";
      }
      out.close();
    }
  }
}

int RadialAnalyser::GetBinID(FLOAT r) {
  int intRadius = r * 1E10;
  int intWidth = mWidth * 1E10;

  int diff = intRadius % intWidth;
  FLOAT floatDiff = diff / 1E10;

  FLOAT rounded = r - floatDiff;

  return (rounded / mWidth);
}
