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
/// creating a parameter context and starting the application with the given
/// parameters.
///
//===----------------------------------------------------------------------===//

#include "Application.h"
#include "Arguments.h"
#include "Definitions.h"

int main(int argc, char *argv[]) {
  Arguments *args = new Arguments(argc, argv);
  Application *app = new Application(args);

  app->StartSplash();
  if (app->Initialise()) {
    app->Run();
  }
  app->EndSplash();

  delete app;
  delete args;

  return 0;
}
