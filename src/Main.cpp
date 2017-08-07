//===-- Main.cpp ----------------------------------------------------------===//
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
/// Main.cpp contains the declaration to the main entry point of the program,
/// creating a parameter context and starting the analyser with the given
/// parameters.
///
//===----------------------------------------------------------------------===//

#include "Arguments.h"
#include "Definitions.h"

int main(int argc, char *argv[]) {
  Arguments *_Args = new Arguments(argc, argv);
  // Analyser *_Analyser = new Analyser(_Args);
  //
  // if (_Analyser->Initialise()) {
  //   _Analyser->Run();
  // }
  //
  // delete _Analyser;
  delete _Args;

  return 0;
}
