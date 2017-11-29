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
#include "CloudAnalyser.h"
#include "ColumnFile.h"
#include "DiscAnalyser.h"
#include "Definitions.h"
#include "DragonFile.h"
#include "File.h"
#include "FileNameExtractor.h"
#include "Generator.h"
#include "OpacityTable.h"
#include "Parameters.h"
#include "RadialAnalyser.h"
#include "SerenFile.h"
#include "SinkAnalyser.h"
#include "SinkFile.h"

class Application {
public:
  Application(Arguments *args);
  ~Application();

  void StartSplash();
  void EndSplash();
  bool Initialise();
  void Run();

private:
  int mNumThreads = 0;
  unsigned int mMaxThreads = 0;
  int mFilesPerThread = 0;
  int mRemainder = 0;

  Arguments *mArgs = NULL;
  Parameters *mParams = NULL;
  FileNameExtractor *mFNE = NULL;
  OpacityTable *mOpacity = NULL;
  CloudAnalyser *mCloudAnalyser = NULL;
  DiscAnalyser *mDiscAnalyser = NULL;
  SinkAnalyser *mSinkAnalyser = NULL;
  RadialAnalyser *mRadialAnalyser = NULL;
  Generator *mGenerator = NULL;

  std::vector<File *> mFiles;
  int mFilesAnalysed = 0;
  std::vector<std::string> mFileNames;

  std::string mInFormat = "";
  std::string mOutFormat = "";
  std::string mEosFilePath = "";
  int mOutput = 0;
  int mConvert = 0;
  int mCloudAnalyse = 0;
  int mCloudCenter = 0;
  int mDiscAnalyse = 0;
  int mSinkAnalyse = 0;
  int mCenter = 0;
  int mRadialAnalyse = 0;

  void Analyse(int task, int start, int end);
  void OutputFile(SnapshotFile *file, std::string fileName = "");
  void FindThermo(SnapshotFile *file);
  void FindColumnDensity(SnapshotFile *file);
  void FindOpticalDepth(SnapshotFile *file);
  std::vector<Particle *> FindNeighbours(std::vector<Particle *> part,
                                         Vec3 pos,
                                         int part_index);
};
