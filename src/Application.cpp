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

Application::Application(Arguments *args) : mArgs(args) {}

Application::~Application() {
  // Only delete if they exist
  // if (mArgs) delete mArgs;
  // if (mParams) delete mParams;
  // if (mOpacity) delete mOpacity;
  // delete mRA;
  // delete mFNE;
}

bool Application::Initialise() {
  if (mArgs->GetNumArgs() < 2) {
    std::cout << "A parameter file must be specified, exiting...\n";
    return false;
  }
  
  mParams = new Parameters();
  mParams->Read(mArgs->GetArgument(0));

  mConvert = mParams->GetInt("CONVERT");
  mInFormat = mParams->GetString("IN_FORMAT");
  mOutFormat = mParams->GetString("OUT_FORMAT");
  mEosFilePath = mParams->GetString("EOS_TABLE");
  mCenter = mParams->GetInt("CENTER_DISC");
  mRadial = mParams->GetInt("RADIAL_AVG");

  mOpacity = new OpacityTable();
  mOpacity->Read(mEosFilePath);
}

void Application::Run() {

}

void Application::ConvertFile(File *file, NameData nameData) {

}

void Application::CenterDisc(File *file, uint32 sinkID) {

}
