//===-- ASCIIFile.h -------------------------------------------------------===//
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
/// ASCIIFile.h
///
//===----------------------------------------------------------------------===//

#pragma once

#include "BinaryIO.h"
#include "Constants.h"
#include "File.h"
#include "Particle.h"
#include "Vec.h"

class ASCIIFile : public SnapshotFile {
public:
  ASCIIFile(NameData nd);
  ~ASCIIFile();

  bool Read();

private:
  int mDimensions = 3;

  void ReadParticleForm();
  void SetSmoothingLength();
};
