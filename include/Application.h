//===-- Application.h -----------------------------------------------------===//
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
/// Application.h contains the functions for the main analysis program. This is
/// where the program performs all functions.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Arguments.h"
#include "Definitions.h"
#include "DragonFile.h"
#include "File.h"
#include "Generator.h"
#include "OpacityTable.h"
#include "Parameters.h"
#include "SerenFile.h"

struct Maxima {
  FLOAT density;
  FLOAT temperature;
};

class Application {
public:
  Application(Arguments *args);
  ~Application();

  bool Initialise();
  void Run();

private:
  int mNumThreads = 0;
  unsigned int mMaxThreads = 0;
  int mFilesPerThread = 0;
  int mRemainder = 0;

  Arguments *mArgs;
  Parameters *mParams;
  Generator *mGenerator;
  OpacityTable *mOpacity;
  DragonFile *mDragonFile;
  SerenFile *mSerenFile;
  // RadialAnalyser *mRA;
  // FileNameExtractor *mFNE;
  //
  std::vector<File *> mFiles;
  std::vector<std::string> mFileNames;
  std::vector<Maxima> mMaxima;
	// std::vector<File *> mConvertedFiles;
  // std::vector<RadialAnalyser *> mRadialAnalysers;
  //
  // std::ofstream mOutStream;
  //
  std::string mInFormat = "";
  std::string mOutFormat = "";
  std::string mEosFilePath = "";
  int mConvert = 0;
  int mCenter = 0;
  int mRadial = 0;

  void Analyse(int task, int start, int end);
  void CenterDisc(File *file, int sinkID = 1);
  void ConvertFile(File *file, NameData nameData);
  void FindTemperatures(SnapshotFile *file);
  void CloudCollapse(SnapshotFile *file);
	// void WriteConvertedFiles(void);
};
