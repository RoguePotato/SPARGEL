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

  mNumThreads = mParams->GetInt("THREADS");
  mMaxThreads = std::thread::hardware_concurrency();
  if (mMaxThreads <= 0) {
    std::cout << "Number of threads not detected, exiting...\n";
    return false;
  }
  if (mNumThreads < 0 || mNumThreads > mMaxThreads) mNumThreads = mMaxThreads;

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
    else if (mInFormat == "sf"){
      mFiles.push_back(new SerenFile(curArg, true));
    }
    else if (mInFormat == "du"){
      mFiles.push_back(new DragonFile(curArg, false));
    }
    else if (mInFormat == "df"){
      mFiles.push_back(new DragonFile(curArg, true));
    }
    else {
      std::cout << "Unrecognised input file format, exiting...\n";
      return false;
    }
  }

  // TODO: Handle case where user does not want to read files just generate
  // one. Possible run generator above here, and create the file, then
  // perform analysis. If no analysis is selected, it will just write.
  if (mFiles.size() < 1) {
    std::cout << "No files selected, exiting...\n";
    return false;
  }

  // Set up file batches
  if (mFiles.size() < mNumThreads) mNumThreads = mFiles.size();
  mFilesPerThread = mFiles.size() / (mNumThreads - 1);
  mRemainder = mFiles.size() % (mNumThreads - 1);
  std::thread threads[mNumThreads];

  // Run threaded analysis, remainder on thread 0
  threads[0] = std::thread(&Application::Run, this, 0, 0, mRemainder);
  for (int i = 1; i < mNumThreads; ++i) {
    threads[i] = std::thread(&Application::Run,
                             this,
                             i,
                             mRemainder + (i - 1) * mFilesPerThread,
                             mRemainder + (i + 0) * mFilesPerThread);
  }

  // Join the threads
  for (int i = 0; i < mNumThreads; ++i) {
    threads[i].join();
  }

  return true;
}

void Application::Run(int task, int start, int end) {
  for (int i = start; i < end; ++i) {
    mFiles.at(i)->Read();

  }
}

void Application::ConvertFile(File *file, NameData nameData) {

}

void Application::CenterDisc(File *file, int sinkID) {

}
