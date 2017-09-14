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

  return true;
}

void Application::Run() {
  mGenerator->Create();

  // SEREN file create from generator
  mSerenFile = new SerenFile();
  mSerenFile->SetParticles(mGenerator->GetParticles());
  mSerenFile->SetSinks(mGenerator->GetSinks());
  mSerenFile->CreateHeader();
  mSerenFile->Write("/home/anthony/Documents/roguepotato/SPARGEL/TEST.su.00001", false);

  // DRAGON file create from SEREN file
  mDragonFile = new DragonFile();
  mDragonFile->SetParticles(mSerenFile->GetParticles());
  mDragonFile->SetSinks(mSerenFile->GetSinks());
  mDragonFile->SetNumGas(mSerenFile->GetParticles().size());
  mDragonFile->SetNumSinks(mSerenFile->GetSinks().size());
  mDragonFile->SetNumTot(mSerenFile->GetParticles().size() +
                         mSerenFile->GetSinks().size());
  mDragonFile->SetTime(mSerenFile->GetTime());
  mDragonFile->CreateHeader();
  mDragonFile->Write("/home/anthony/Documents/roguepotato/SPARGEL/TEST.df.00001", true);
}

void Application::ConvertFile(File *file, NameData nameData) {

}

void Application::CenterDisc(File *file, int sinkID) {

}
