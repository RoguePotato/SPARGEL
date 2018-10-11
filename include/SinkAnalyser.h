//===-- SinkAnalyser.h ----------------------------------------------------===//
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
/// SinkAnalyser.h contains the functions to analyse sink files.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Definitions.h"
#include "SinkFile.h"

class SinkAnalyser {
public:
  SinkAnalyser();
  ~SinkAnalyser();

  void AddMassRadius(SinkFile *sf);
  void CalculateAccRate(SinkFile *sf);
  void AddNbody(SinkFile *sf);
  bool WriteMassRadius();
  bool WriteNbody();

private:
  std::vector<SinkRecord> mRecords;
  std::vector<SinkRecord> mNbodyRecords;
  int avg_num[3] = {0, 0, 0};
  FLOAT avg_dmdt[3] = {0.0, 0.0, 0.0};
};
