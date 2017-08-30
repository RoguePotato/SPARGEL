//===-- DragonFile.h ------------------------------------------------------===//
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
/// DragonFile.h contains the functions to read and write simulation snapshots
/// in the DRAGON data format.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "BinaryIO.h"
#include "Constants.h"
#include "File.h"
#include "Particle.h"
#include "Vec.h"

class DragonFile : public SnapshotFile {
public:
  DragonFile(NameData nameData, bool formatted);
  ~DragonFile();

  bool Read();
  bool Write(std::string fileName, bool formatted);

private:
  BinaryReader *BR;
  BinaryWriter *BW;

  int32 mIntData[20] = {0};
  real64 mFloatData[50] = {0};

  int32 mTypeData[8][5] = {};

  void AllocateMemory();

  void ReadHeaderForm();
  void ReadParticleDataForm();
  void ReadSinkDataForm();

  void ReadHeaderUnform();
  void ReadParticleDataUnform();
  void ReadSinkDataUnform();
};
