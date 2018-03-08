//===-- Constants.h -------------------------------------------------------===//
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
/// Constants.h defines all constant values to be used within SPARGEL.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Definitions.h"

static const FLOAT PI = 3.14159265359;
static const FLOAT G = 6.6740831E-11;
static const FLOAT G_AU = 4.01e-14;
static const FLOAT K = 1.38064852E-23;
static const FLOAT SB = 5.670367E-5; // E-8 for SI
static const FLOAT MU_BAR = 2.35;
static const FLOAT M_P = 1.6726219E-27;
static const FLOAT GAMMA = 5.0 / 3.0;
static const FLOAT MU = 2.35;
static const FLOAT THETA = 0.57735026919;

static const FLOAT MSUN_TO_KG = 1.9891E30;
static const FLOAT MSUN_TO_G = 1.9891E33;
static const FLOAT PC_TO_M = 3.08567758E16;
static const FLOAT PC_TO_AU = 206265.0;
static const FLOAT AU_TO_CM = 1.495978707E13;
static const FLOAT AU_TO_M = 1.495978707E11;
static const FLOAT AU_TO_KM = 1.495978707E8;
static const FLOAT MSOLAU_TO_KGM = 8.88803576E7;
static const FLOAT MSOLPERAU2_TO_GPERCM2 = 8.888035760594663E6;
static const FLOAT MSOLPERAU3_TO_GPERCM3 = 6.2647E-7;
static const FLOAT GPERCM2_TO_KGPERM2 = 10.0;
static const FLOAT KMPERS_TO_MPERS = 1000.0;
static const FLOAT ERGPERG_TO_JPERKG = 1E-4;
static const FLOAT YR_TO_SEC = 3.154E7;

static const FLOAT TIME_UNIT = 0.159146;

static const int RADIAL_QUANTITIES = 32;