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
  mFiles.clear();
  // if (mArgs != NULL) delete mArgs;
  // if (mParams != NULL) delete mParams;
  // if (mOpacity != NULL) delete mOpacity;
  // if (mFNE != NULL) delete mFNE;
  // delete mRA;
}

void Application::StartSplash(void) {
  std::cout << "\n";
  for (int i = 0; i < 16; ++i) std::cout << "=====";
  std::cout << "\n\n";
  std::cout << "   SParGeL\n\n";
  std::cout << "   Smoothed Particle Generator and Loader\n\n";
  for (int i = 0; i < 16; ++i) std::cout << "=====";
  std::cout << "\n\n";
}

void Application::EndSplash(void) {
  for (int i = 0; i < 16; ++i) std::cout << "=====";
  std::cout << "\n";
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
   if (!mOpacity->Read()) return false;

  // mGenerator = new Generator(mParams, mOpacity);

  // Create files
  for (int i = 1; i < mArgs->GetNumArgs() - 1; ++i) {
    std::string curArg = mArgs->GetArgument(i);
    mFNE = new FileNameExtractor(curArg);
    NameData nd = mFNE->GetNameData();

    if (mInFormat == "su") {
      mFiles.push_back(new SerenFile(nd, false));
    }
    else if (mInFormat == "sf"){
      mFiles.push_back(new SerenFile(nd, true));
    }
    else if (mInFormat == "du"){
      mFiles.push_back(new DragonFile(nd, false));
    }
    else if (mInFormat == "df"){
      mFiles.push_back(new DragonFile(nd, true));
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

  return true;
}

void Application::Run() {
  // Set up file batches
  if (mFiles.size() < mNumThreads) mNumThreads = mFiles.size();
  mFilesPerThread = mFiles.size() / (mNumThreads);
  mRemainder = mFiles.size() % (mNumThreads);
  std::thread threads[mNumThreads];

  std::cout << "   Threads          : " << mNumThreads << "\n";
  std::cout << "   Files            : " << mFiles.size() << "\n";
  std::cout << "   Files per thread : " << mFilesPerThread << "\n";
  std::cout << "   Remainder        : " << mRemainder << "\n\n";

  std::cout << "   EOS table        : " << mOpacity->GetFileName() << "\n\n";

  // Run threaded analysis, remainder added on to thread 0
  threads[0] = std::thread(&Application::Analyse,
                           this,
                           0, 0, mRemainder + mFilesPerThread);
  for (int i = 1; i < mNumThreads; ++i) {
    threads[i] = std::thread(&Application::Analyse,
                             this,
                             i,
                             mRemainder + ((i + 0) * mFilesPerThread),
                             mRemainder + ((i + 1) * mFilesPerThread));
  }

  // Join the threads
  for (int i = 0; i < mNumThreads; ++i) {
    threads[i].join();
  }

  std::cout << "   Files analysed   : " << mFilesAnalysed << "\n\n";
}

void Application::Analyse(int task, int start, int end) {
  for (int i = start; i < end; ++i) {
    mFiles.at(i)->Read();
    if (mInFormat == "su") FindTemperatures((SnapshotFile *) mFiles.at(i));
    if (mConvert) ConvertFile((SnapshotFile *) mFiles.at(i));
    ++mFilesAnalysed;
    delete mFiles.at(i);
  }
}

void Application::ConvertFile(SnapshotFile *file) {
  NameData nd = file->GetNameData();
  if (mOutFormat == "df") {
    nd.format = "df";
    std::string outputName = nd.dir + "/" + nd.id + "." +
    nd.format + "." + nd.snap;

    DragonFile *df = new DragonFile(nd, true);
    df->SetParticles(file->GetParticles());
    df->SetSinks(file->GetSinks());
    df->SetNumGas(file->GetNumGas());
    df->SetNumSinks(file->GetNumSinks());
    df->SetNumTot(file->GetNumPart());
    df->SetTime(file->GetTime());
    df->Write(outputName, true);
  }
}

void Application::CenterDisc(File *file, int sinkID) {

}

void Application::FindTemperatures(SnapshotFile *file) {
  std::vector<Particle *> part = file->GetParticles();
  for (int i = 0; i < part.size(); ++i) {
    Particle *p = part.at(i);
    FLOAT density = p->GetD();
    FLOAT energy = p->GetU();
    FLOAT temp = mOpacity->GetTemp(density, energy);
    part.at(i)->SetT(temp);
  }
  file->SetParticles(part);
}
