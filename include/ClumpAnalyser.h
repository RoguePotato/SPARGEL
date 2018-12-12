//===-- ClumpAnalyser.h ---------------------------------------------------===//
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
/// ClumpAnalyser.h handles data analysis for clumps, typically using spherical
/// bins in logarithmic space.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Definitions.h"
#include "Parameters.h"

class ClumpAnalyser {
public:
  ClumpAnalyser(const Parameters *params);
  ~ClumpAnalyser();

private:
  const Parameters *mParams;
};