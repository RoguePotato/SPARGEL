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
#include "File.h"
#include "Parameters.h"

class Analyser {
public:
  Analyser(Arguments *args);
  ~Analyser();

  bool Initialise();
  void Run();

private:
  Arguments *mArgs;
  Parameters *mParams;
  // RadialAnalyser *mRA;
  // FileNameExtractor *mFNE;
  // EosTable *mEOS;
  //
  // std::vector<std::string> mFileNames;
  // std::vector<File *> mFiles;
	// std::vector<File *> mConvertedFiles;
  // std::vector<RadialAnalyser *> mRadialAnalysers;
  //
  // std::ofstream mOutStream;
  //
  std::string mInFormat = "";
  std::string mOutFormat = "";
  std::string mEosFilePath = "";
  int8 mConvert = 0;
  int8 mCenter = 0;
  int8 mRadial = 0;
  //
  void CenterDisc(File *file, uint32 sinkID = 1);
  void ConvertFile(File *file, NameData nameData);
	// void WriteConvertedFiles(void);
};
