//===-- Application.cpp ---------------------------------------------------===//
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
/// Application.cpp
///
//===----------------------------------------------------------------------===//

#include "Application.h"

Analyser::Analyser(Arguments *args) : mArgs(args) {}

Analyser::~Analyser() {
  // Only delete if they exist
  // delete mArgs;
  // delete mParams;
  // delete mRA;
  // delete mFNE;
  // delete mEOS;
}

bool Analyser::Initialise() {
  mParams = new Parameters();
  mParams->Read(mArgs->GetArgument(0));

  mConvert = mParams->GetInt("CONVERT");
  mInFormat = mParams->GetString("IN_FORMAT");
  mOutFormat = mParams->GetString("OUT_FORMAT");
  mEosFilePath = mParams->GetString("EOS_TABLE");
  mCenter = mParams->GetInt("CENTER_DISC");
  mRadial = mParams->GetInt("RADIAL_AVG");
}

void Analyser::Run() {

}

void Analyser::ConvertFile(File *file, NameData nameData) {

}

void Analyser::CenterDisc(File *file, uint32 sinkID) {

}
