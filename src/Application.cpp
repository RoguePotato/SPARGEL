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
  // if (mGenerator) delete mGenerator;
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

  mGenerator = new Generator(mParams, mOpacity);


  NameData dragName;
  dragName.name = ("/home/anthony/Documents/roguepotato/SPARGEL/disc.dat");
  mDragonFile = new DragonFile(dragName, true);

  NameData nameData;
  nameData.name = ("/home/anthony/Documents/disc_tests/32k/RAD.su.00001");
  mSerenFile = new SerenFile(nameData, false);
  mSerenFile->Read();

  return true;
}

void Application::Run() {
  // mGenerator->Create();

  mDragonFile->SetParticles(mSerenFile->GetParticles());
  mDragonFile->Write("/home/anthony/Documents/roguepotato/SPARGEL/disc.dat", true);


}

void Application::ConvertFile(File *file, NameData nameData) {

}

void Application::CenterDisc(File *file, int sinkID) {

}
