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

  mThreads = mParams->GetInt("THREADS");
  mMaxThreads = std::thread::hardware_concurrency();
  if (mMaxThreads <= 0) {
    std::cout << "Number of threads not detected, exiting...\n";
    return false;
  }
  if (mThreads < 0 || mThreads > mMaxThreads) mThreads = mMaxThreads;

  std::cout << mThreads << '\n';

  mConvert = mParams->GetInt("CONVERT");
  mInFormat = mParams->GetString("IN_FORMAT");
  mOutFormat = mParams->GetString("OUT_FORMAT");
  mEosFilePath = mParams->GetString("EOS_TABLE");
  mCenter = mParams->GetInt("CENTER_DISC");
  mRadial = mParams->GetInt("RADIAL_AVG");

  mOpacity = new OpacityTable(mEosFilePath, true);
  mOpacity->Read();

  // mGenerator = new Generator(mParams, mOpacity);

  // Create files
  for (int i = 1; i < mArgs->GetNumArgs() - 1; ++i) {
    std::string curArg = mArgs->GetArgument(i);
    if (mInFormat == "su") {
      mFiles.push_back(new SerenFile(curArg, false));
    }
    else if (mInFormat == "df"){

    }
  }
  for (int i = 0; i < mFiles.size(); ++i) {
    std::cout << mFiles.at(i)->GetFileName() << "\n";
  }

  std::thread threads[mThreads];
  for (int i = 0; i < mThreads; ++i) {
    std::thread t1(&Application::Run, this, i);
    t1.join();
  }

  return true;
}

void Application::Run(int task) {
  std::cout << "Running task " << task << '\n';
}

void Application::ConvertFile(File *file, NameData nameData) {

}

void Application::CenterDisc(File *file, int sinkID) {

}
