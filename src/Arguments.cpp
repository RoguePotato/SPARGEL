//===-- Arguments.cpp -----------------------------------------------------===//
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
/// Arguments.cpp
///
//===----------------------------------------------------------------------===//

#include "Arguments.h"

Arguments::Arguments(int argc, char *argv[]) : mNumArgs(argc) {
  for (int i = 1; i < mNumArgs; ++i) {
    mArguments.push_back(argv[i]);
  }
}

Arguments::~Arguments() {

}
