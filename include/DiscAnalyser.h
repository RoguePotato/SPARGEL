//===-- DiscAnalyser.h ----------------------------------------------------===//
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
/// DiscAnalyser.h contains the functions to center around a given sink.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Definitions.h"
#include "File.h"
#include "Particle.h"
#include "Vec.h"

class DiscAnalyser {
public:
  DiscAnalyser();
  ~DiscAnalyser();

  void Center(SnapshotFile *file, int center);
  void Radial(SnapshotFile *file);

private:

};
