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
#include "Particle.h"
#include "Vec.h"

class SerenFile : public SnapshotFile {
public:
  SerenFile(NameData nameData, bool formatted);
  ~SerenFile();

  bool Read();
  bool Write(std::string fileName, bool formatted);

private:
  const uint32 STRING_LENGTH = 20;
  const std::string ASCII_FORMAT = "SERENASCIIDUMPV2";
  const std::string BINARY_FORMAT = "SERENBINARYDUMPV3";

  std::string mFormatID = "";
  uint32 mHeader[4] = {0}; // precision (4 or 8), ndim, vdim, bdim
  int32 mIntData[50] = {0};
  int64 mLongData[50] = {0};
  real32 mFloatData[50] = {0};
  real64 mDoubleData[50] = {0};

  uint32 mPrecision = 8;
  uint32 mPosDim = 1;
  uint32 mVelDim = 1;
  uint32 mMagDim = 1;

  uint32 mSinkDataLength = 0;

  std::vector<std::string> mUnitData;
  std::vector<std::string> mDataID;
  uint32 mNumUnit = 0;
  uint32 mNumData = 0;

  int32 mTypeData[8][5] = {};
  int32 mUnknownValues[50] = {};

  void AllocateMemory();

  void ReadHeaderForm();
  void ReadParticleDataForm();
  void ReadSinkDataForm();

  void ReadHeaderUnform();
  void ReadParticleDataUnform();
  void ReadSinkDataUnform();

  bool WriteForm(void);
  bool WriteUnform(void);
};
