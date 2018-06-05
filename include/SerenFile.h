//===-- SerenFile.h ------------------------------------------------------===//
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
/// SerenFile.h contains the functions to read and write simulation snapshots
/// in the SEREN data format.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "BinaryIO.h"
#include "Constants.h"
#include "File.h"
#include "Formatter.h"
#include "OpacityTable.h"
#include "Particle.h"
#include "Vec.h"

class SerenFile : public SnapshotFile {
public:
  SerenFile(NameData nd, bool formatted, int extra_data);
  ~SerenFile();

  bool Read();
  bool Write(std::string fileName, bool formatted);
  void FindTemperatures(OpacityTable *op);

  void CreateHeader(void);

private:
  const int STRING_LENGTH = 20;
  const std::string ASCII_FORMAT = "SERENASCIIDUMPV2";
  const std::string BINARY_FORMAT = "SERENBINARYDUMPV3";

  std::string mFormatID = "";
  int mHeader[4] = {0}; // precision (4 or 8), ndim, vdim, bdim
  int mIntData[50] = {0};
  long mLongData[50] = {0};
  FLOAT mFloatData[50] = {0};
  DOUBLE mDoubleData[50] = {0};

  int mPrecision = 8;
  int mPosDim = 3;
  int mVelDim = 3;
  int mMagDim = 3;

  int mSinkDataLength = 0;

  std::vector<std::string> mUnitData;
  std::vector<std::string> mDataID;
  int mNumUnit = 0;
  int mNumData = 0;

  int mTypeData[8][5] = {};
  int mUnknownValues[50] = {};

  void AllocateMemory(void);

  bool ReadHeaderForm(void);
  void ReadParticleForm(void);
  void ReadSinkForm(void);

  bool ReadHeaderUnform(void);
  void ReadParticleUnform(void);
  void ReadSinkUnform(void);

  void WriteHeaderForm(Formatter formatStream);
  void WriteParticleForm(Formatter formatStream);
  void WriteSinkForm(Formatter formatStream);

  void WriteHeaderUnform(void);
  void WriteParticleUnform(void);
  void WriteSinkUnform(void);
};
