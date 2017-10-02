//===-- ColumnFile.h ------------------------------------------------------===//
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
/// ColumnFile.h contains the functions to read and write simulation snapshots
/// in an ASCII data format.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "BinaryIO.h"
#include "Constants.h"
#include "File.h"
#include "Particle.h"
#include "Vec.h"

class ColumnFile : public SnapshotFile {
public:
  ColumnFile(NameData nd);
  ~ColumnFile();

  bool Read();
  bool Write(std::string fileName);

private:
  int mDimensions = 3;

  void AllocateMemory(void);

  bool ReadHeaderForm(void);
  void ReadParticleForm(void);
  void ReadSinkForm(void);

  void WriteHeaderForm(Formatter formatStream);
  void WriteParticleForm(Formatter formatStream);
  void WriteSinkForm(Formatter formatStream);
};
