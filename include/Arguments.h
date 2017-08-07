//===-- Arguments.h -------------------------------------------------------===//
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
/// Arguments.h defines functions to handle command line argument input.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Definitions.h"

class Arguments {
public:
  Arguments(int argc, char *argv[]);
  ~Arguments();

  int GetNumArgs() { return mNumArgs; }
  std::vector<std::string> GetArguments() { return mArguments; }
  std::string GetArgument(int i) { return mArguments.at(i); }

private:
  int mNumArgs = 0;
  std::vector<std::string> mArguments;
};
