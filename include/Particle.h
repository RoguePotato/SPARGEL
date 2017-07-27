//===-- Particle.h --------------------------------------------------------===//
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
/// Particle.h defines all particle classes and corresponding data.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Definitions.h"

class Particle {
public:
  Particle();
  ~Particle();

private:

};

class Gas : public Particle {
public:
  Gas();
  ~Gas();

private:

};

class Sink : public Particle {
public:
  Sink();
  ~Sink();

private:

};
