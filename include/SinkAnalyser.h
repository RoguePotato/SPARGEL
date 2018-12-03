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

struct AccretionRecord {
  std::string id = "";
  FLOAT avg_begin = 0.0;
  FLOAT avg_end = 0.0;
  FLOAT avg_tot = 0.0;
};

class SinkAnalyser {
public:
  SinkAnalyser();
  ~SinkAnalyser();

  void AddMassRadius(SinkFile *sf);
  void CalculateMassRadius(SnapshotFile *file, int sink_id);
  void CalculateAccRate(SinkFile *sf);
  void AddNbody(SinkFile *sf);
  bool WriteMassRadius();
  bool WriteMassAccretion();
  bool WriteNbody();

private:
  std::vector<SinkRecord> mRecords;
  std::vector<SinkRecord> mNbodyRecords;
  int avg_num[3] = {0, 0, 0};
  FLOAT avg_dmdt[3] = {0.0, 0.0, 0.0};
  std::vector<AccretionRecord> avg_dmdt_records;
};
