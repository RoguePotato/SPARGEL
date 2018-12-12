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

#include "ASCIIFile.h"
#include "Arguments.h"
#include "CloudAnalyser.h"
#include "ColumnFile.h"
#include "CoolingMap.h"
#include "Definitions.h"
#include "DiscAnalyser.h"
#include "DragonFile.h"
#include "File.h"
#include "FileNameExtractor.h"
#include "FragmentationAnalyser.h"
#include "Generator.h"
#include "MassAnalyser.h"
#include "OpacityTable.h"
#include "OpticalDepthOctree.h"
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
  int mOutputInfo = 0;

  Arguments *mArgs = NULL;
  Parameters *mParams = NULL;
  FileNameExtractor *mFNE = NULL;
  OpacityTable *mOpacity = NULL;
  CloudAnalyser *mCloudAnalyser = NULL;
  DiscAnalyser *mDiscAnalyser = NULL;
  FragmentationAnalyser *mFragAnalyser = NULL;
  SinkAnalyser *mSinkAnalyser = NULL;
  RadialAnalyser *mRadialAnalyser = NULL;
  MassAnalyser *mMassAnalyser = NULL;
  Generator *mGenerator = NULL;
  CoolingMap *mCoolingMap = NULL;

  std::vector<File *> mFiles;
  std::vector<SinkFile *> mSinkFiles;
  int mFilesAnalysed = 0;
  std::vector<std::string> mFileNames;

  std::string mInFormat = "";
  std::string mOutFormat = "";
  std::string mCoolingMethod = "";
  std::string mEosFilePath = "";
  float mGamma = 0.0;
  float mMuBar = 0.0;
  int mOutput = 0;
  int mReduceParticles = 0;
  int mExtraData = 0;
  int mConvert = 0;
  int mCloudAnalyse = 0;
  int mCloudCenter = 0;
  int mDiscAnalyse = 0;
  int mFragAnalyse = 0;
  int mSinkAnalyse = 0;
  int mNbodyOutput = 0;
  int mRadialAnalyse = 0;
  int mMassAnalyse = 0;
  int mCenter = 0;
  int mExtraQuantities = 0;
  int mResetTime = 0;
  Vec3 mPosCenter = {0.0, 0.0, 0.0};
  int mCenterDensest = 0;
  int mHillRadiusCut = 0;
  float mMidplaneCut = 0.0;
  int mInsertPlanet = 0.0;

  void Analyse(int task, int start, int end);
  void MidplaneCut(SnapshotFile *file);
  void HillRadiusCut(SnapshotFile *file);
  void OutputFile(SnapshotFile *file);
  void FindThermo(SnapshotFile *file);
  void FindOpticalDepth(SnapshotFile *file);
  void FindToomre(SnapshotFile *file);
  void FindBeta(SnapshotFile *file);
  void InsertPlanet(SnapshotFile *file);
  void ReduceParticles(SnapshotFile *file);

  void OutputInfo(SnapshotFile *file);
};
