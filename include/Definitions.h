//===-- Definitions.h -----------------------------------------------------===//
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
/// Definitions.h lists all includes and type definitions for use within
/// SPARGEL.
///
//===----------------------------------------------------------------------===//

#pragma once

#include <algorithm>
#include <math.h>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <map>
#include <sstream>
#include <stdint.h>
#include <thread>
#include <time.h>
#include <vector>

#define PRECISION DOUBLE_PRECISION

#define SINGLE_PRECISION 4
#define DOUBLE_PRECISION 8

#if PRECISION == SINGLE_PRECISION
typedef float FLOAT;
typedef double DOUBLE;
#elif PRECISION == DOUBLE_PRECISION
typedef double FLOAT;
typedef double DOUBLE;
#endif

#define GAS_TYPE 1
#define SINK_TYPE -1
#define DUST_TYPE 2