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
  mOutput = mParams->GetInt("OUTPUT_FILES");
  mEosFilePath = mParams->GetString("EOS_TABLE");
  mCloudAnalyse = mParams->GetInt("CLOUD_ANALYSIS");
  mCloudCenter = mParams->GetInt("CLOUD_CENTER");
  mDiscAnalyse = mParams->GetInt("DISC_ANALYSIS");
  mSinkAnalyse = mParams->GetInt("SINK_ANALYSIS");
  mCenter = mParams->GetInt("DISC_CENTER");
  mRadialAnalyse = mParams->GetInt("RADIAL_ANALYSIS");

  mOpacity = new OpacityTable(mEosFilePath, true);
  if (!mOpacity->Read()) return false;

  if (mCloudAnalyse) {
    mCloudAnalyser = new CloudAnalyser(mParams->GetString("CLOUD_OUTPUT"));
  }

  if (mDiscAnalyse) {
    mDiscAnalyser = new DiscAnalyser();
  }

  if (mSinkAnalyse) {
    mSinkAnalyser = new SinkAnalyser();
  }

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
    else if (mInFormat == "column"){
      mFiles.push_back(new ColumnFile(nd));
    }
    else if (mInFormat == "sink") {
      mFiles.push_back(new SinkFile(nd));
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

  int pos = 0;
  for (int i = 0; i < mNumThreads; ++i) {
    int start = pos;
    int end = pos + mFilesPerThread + ((mRemainder > 0) ? 1 : 0);
    pos = end;
    --mRemainder;

    threads[i] = std::thread(&Application::Analyse, this, i, start, end);
  }

  // Join the threads
  for (int i = 0; i < mNumThreads; ++i) {
    threads[i].join();
  }

  if (mCloudAnalyse) mCloudAnalyser->Write();
  if (mSinkAnalyse) {
    mSinkAnalyser->WriteMassRadius();
    mSinkAnalyser->WriteNbody();
  }

  std::cout << "   Files analysed   : " << mFilesAnalysed << "\n\n";
}

void Application::Analyse(int task, int start, int end) {
  for (int i = start; i < end; ++i) {
    // File read
    if (!mFiles.at(i)->Read()) break;

    // U to T conversion for SEREN format
    if (mInFormat == "su" || mInFormat == "sf") {
      FindTemperatures((SnapshotFile *) mFiles.at(i));
    }
    // Cloud analysis
    if (mCloudAnalyse) {
      mCloudAnalyser->FindCentralQuantities((SnapshotFile *) mFiles.at(i));
      if (mCloudCenter) {
        mCloudAnalyser->CenterAroundDensest((SnapshotFile *) mFiles.at(i));
      }
    }
    // Disc analysis
    if (mDiscAnalyse) {
      if (mCenter) {
        mDiscAnalyser->Center((SnapshotFile *) mFiles.at(i), mCenter - 1);
      }
    }
    // Sink analysis
    if (mSinkAnalyse) {
      mSinkAnalyser->AddMassRadius((SinkFile *) mFiles.at(i));
      mSinkAnalyser->AddNbody((SinkFile *) mFiles.at(i));
    }
    // Radial analysis
    if (mRadialAnalyse) {
      RadialAnalyser *ra = new RadialAnalyser(mParams->GetInt("RADIUS_IN"),
                                              mParams->GetInt("RADIUS_OUT"),
                                              mParams->GetInt("RADIAL_BINS"),
                                              mParams->GetInt("RADIAL_LOG"));
      ra->Run((SnapshotFile *) mFiles.at(i));
      delete ra;
    }
    // File conversion
    if (mConvert) {
      mFiles.at(i)->SetNameDataFormat(mOutFormat);
    }
    // Snapshot output
    if (mOutput) {
      OutputFile((SnapshotFile *) mFiles.at(i));
    }
    ++mFilesAnalysed;
    delete mFiles.at(i);
  }
}

void Application::OutputFile(SnapshotFile *file) {
  NameData nd = file->GetNameData();
  std::string outputName = nd.dir + "/" + nd.id + "." +
  nd.format + "." + nd.snap + nd.append;

  // TODO: reduce code duplication
  if (nd.format == "df") {
    DragonFile *df = new DragonFile(nd, true);
    df->SetParticles(file->GetParticles());
    df->SetSinks(file->GetSinks());
    df->SetNumGas(file->GetNumGas());
    df->SetNumSinks(file->GetNumSinks());
    df->SetNumTot(file->GetNumPart());
    df->SetTime(file->GetTime());
    df->Write(outputName, true);
  }

  if (nd.format == "su") {
    SerenFile *su = new SerenFile(nd, false);
    su->SetParticles(file->GetParticles());
    su->SetSinks(file->GetSinks());
    su->SetNumGas(file->GetNumGas());
    su->SetNumSinks(file->GetNumSinks());
    su->SetNumTot(file->GetNumPart());
    su->SetTime(file->GetTime());
    su->Write(outputName, false);
  }
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
