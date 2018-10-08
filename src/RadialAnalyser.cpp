//===-- RadialAnalyser.cpp ------------------------------------------------===//
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

RadialAnalyser::RadialAnalyser(Parameters *params) : mParams(params) {
  mIn = mParams->GetFloat("RADIUS_IN");
  mOut = mParams->GetFloat("RADIUS_OUT");
  mBins = mParams->GetInt("RADIAL_BINS");
  mLog = mParams->GetInt("RADIAL_LOG");
  mVert = mParams->GetInt("VERTICAL_ANALYSIS");
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
  if (mLog) {
    for (FLOAT i = mIn; i < mOut; i += mWidth) {
      FLOAT inner = pow(10.0, i);
      FLOAT outer = pow(10.0, i + mWidth);
      mRadialBins.push_back(new RadialBin(mParams, 0.0, inner, outer, mWidth));
    }
  } else {
    for (FLOAT i = mIn; i < mOut; i += mWidth) {
      if (file->GetSinks().size() > 1) {
        mRadialBins.push_back(new RadialBin(
            mParams, file->GetSinks()[0]->GetM(), i, i + mWidth, mWidth));
      } else {
        mRadialBins.push_back(
            new RadialBin(mParams, 0.0, i, i + mWidth, mWidth));
      }
    }
  }

  // Allocate particles to bins
  std::vector<Particle *> part = file->GetParticles();
  std::vector<Sink *> sink = file->GetSinks();
  for (int i = 0; i < part.size(); ++i) {
    FLOAT r = part[i]->GetX().Norm();
    if (mLog)
      r = log10(r);

    int binID = GetBinID(r - mIn);

    if (binID < mRadialBins.size() && binID >= 0) {
      mRadialBins.at(binID)->AddParticle(part[i]);
    }
  }

  // Calculate radial averages
  for (int i = 0; i < mRadialBins.size(); ++i) {
    mRadialBins[i]->CalculateValues();
  }

  // Output azimuthally-averaged values
  NameData nd = file->GetNameData();
  if (nd.dir == "")
    nd.dir = ".";
  std::string outputName = nd.dir + "/SPARGEL." + nd.id + "." + nd.format +
                           "." + nd.snap + nd.append + ".radial";

  std::ofstream out;
  out.open(outputName);
  for (int i = 0; i < mRadialBins.size(); ++i) {
    RadialBin *b = mRadialBins[i];
    if (b->GetNumParticles() <= 0) {
      continue;
    }

    out << b->GetMid() << "\t";
    for (int j = 0; j < TOT_RAD_QUAN; ++j) {
      out << b->GetAverage(j) << "\t";
    }
    out << "\n";
  }
  out.close();

  // Output vertical bins
  if (mVert) {
    for (int r = 0; r < mRadialBins.size(); ++r) {
      RadialBin *b = mRadialBins[r];
      if (b->GetNumParticles() <= 0)
        continue;

      if (nd.dir == "")
        nd.dir = ".";
      outputName = nd.dir + "/SPARGEL." + nd.id + "." + nd.format + "." +
                   nd.snap + nd.append + ".vertical." + std::to_string(r);
      out.open(outputName);
      for (int i = 0; i < b->GetVerticalBins().size(); ++i) {
        VerticalBin *v = b->GetVerticalBins()[i];
        if (v->GetNumParticles() <= 0)
          continue;

        out << v->GetMid() << "\t";
        for (int j = 0; j < TOT_RAD_QUAN; ++j) {
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
