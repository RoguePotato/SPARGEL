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
  mSpherical = mParams->GetInt("SPHERICAL");
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
  // Create the bins.
  float cur_inner = mIn;
  float cur_outer = mOut;
  for (int i = 0; i < mBins; ++i) {
    mRadialBins.push_back(
        new RadialBin(mParams, cur_inner, cur_outer, mWidth));
    cur_outer = cur_inner;
    cur_inner += mWidth;
  }

  // Allocate particles to bins
  std::vector<Particle *> part = file->GetParticles();
  std::vector<Sink *> sink = file->GetSinks();
  for (int i = 0; i < part.size(); ++i) {
    float r = 0.0;
    if (mSpherical) {
      r = part[i]->GetX().Norm();
    } else {
      r = part[i]->GetX().Norm2();
    }

    if (mLog) {
      r = log10(r);
    }

    int binID = GetBinID(r - mIn);

    if (binID < mRadialBins.size() && binID >= 0) {
      mRadialBins.at(binID)->AddParticle(part[i]);
    }
  }

  // Calculate radial averages
  for (int i = 0; i < mRadialBins.size(); ++i) {
    mRadialBins[i]->CalculateValues();
  }

  // Find the cumulative mass of the bins.
  float total_mass = 0.0f;
  for (int i = 0; i < mRadialBins.size(); ++i) {
    total_mass +=
        mRadialBins[i]->GetAverage(11) * mRadialBins[i]->GetNumParticles();
    mRadialBins[i]->SetAverage(total_mass, 11);
  }

  // Output azimuthally-averaged values
  NameData nd = file->GetNameData();
  if (nd.dir == "")
    nd.dir = ".";
  std::string outputName = "";

  if (mSpherical) {
    outputName = nd.dir + "/SPARGEL." + nd.id + "." + nd.format + "." +
                 nd.snap + nd.append + ".spherical";
  } else {
    outputName = nd.dir + "/SPARGEL." + nd.id + "." + nd.format + "." +
                 nd.snap + nd.append + ".radial";
  }

  std::ofstream out;
  out.open(outputName);
  for (int i = 0; i < mRadialBins.size(); ++i) {
    RadialBin *b = mRadialBins[i];
    if (b->GetNumParticles() <= 0) {
      continue;
    }

    if (mLog) {
      out << pow(10.0, b->GetMid()) << "\t";
    } else {
      out << b->GetMid() << "\t";
    }

    for (int j = 0; j < 32; ++j) {
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

int RadialAnalyser::GetBinID(float r) {
  int intRadius = r * 1E10;
  int intWidth = mWidth * 1E10;

  int diff = intRadius % intWidth;
  float floatDiff = diff / 1E10;

  float rounded = r - floatDiff;

  return (rounded / mWidth);
}
