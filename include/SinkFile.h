//===-- SinkFile.h ------------------------------------------------------------===//
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
/// SinkFile.h contains the functions to read in sink files produced by the
/// ExtraSinkOutput() function within GANDALF.
//===----------------------------------------------------------------------===//

#pragma once

#include "Constants.h"
#include "Definitions.h"
#include "File.h"

class SinkRecord {
public:
  SinkRecord() {};
  ~SinkRecord() {};

  FLOAT time = 0.0;
  int nsteps = 0;
  Vec3 pos = {0.0, 0.0, 0.0};
  Vec3 vel = {0.0, 0.0, 0.0};
  Vec3 acc = {0.0, 0.0, 0.0};
  Vec3 angmom = {0.0, 0.0, 0.0};
  FLOAT m = 0.0;
  FLOAT menc = 0.0;
  FLOAT mmax = 0.0;
  FLOAT macctot = 0.0;
  FLOAT dmdt = 0.0;
  FLOAT ketot = 0.0;
  FLOAT gpetot = 0.0;
  FLOAT rotketot = 0.0;
  FLOAT utot = 0.0;
  FLOAT taccrete = 0.0;
  FLOAT trad = 0.0;
  FLOAT trot = 0.0;
  FLOAT tvisc = 0.0;
};

class SinkFile : public File {
public:
  SinkFile() {};
  SinkFile(NameData nd);
  ~SinkFile();

  bool Read();
  bool Write(std::string fileName, bool formatted) {};

  NameData GetNameData() { return mNameData; }
  std::string GetFileName() { return mNameData.name; }
  std::vector<SinkRecord *> GetRecords() { return mRecords; }

  void SetNameDataFormat(std::string str) { mNameData.format = str; }
  void SetNameDataAppend(std::string str) { mNameData.append = str; }

private:
  NameData mNameData;
  std::vector<SinkRecord *> mRecords;
};
