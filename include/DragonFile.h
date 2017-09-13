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
  int mIntData[20] = {0};
  FLOAT mFloatData[50] = {0};

  int mTypeData[8][5] = {};

  void AllocateMemory();

  void ReadHeaderForm();
  void ReadParticleDataForm();
  void ReadSinkDataForm();

  void ReadHeaderUnform();
  void ReadParticleDataUnform();
  void ReadSinkDataUnform();
};
