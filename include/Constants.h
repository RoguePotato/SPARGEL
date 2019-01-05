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

static const float PI = 3.14159265359;
static const float G = 6.6740831E-11;
static const float G_AU = 4.01e-14;
static const float K = 1.38064852E-23;
static const float SB = 5.670367E-5; // E-8 for SI
static const float MU_BAR = 2.35;
static const float M_P = 1.6726219E-27;
static const float GAMMA = 5.0 / 3.0;
static const float MU = 2.35;
static const float THETA = 0.57735026919;

static const float MSUN_TO_KG = 1.9891E30;
static const float MSUN_TO_G = 1.9891E33;
static const float MSUN_TO_MJUP = 1047.56;
static const float PC_TO_M = 3.08567758E16;
static const float PC_TO_AU = 206265.0;
static const float AU_TO_CM = 1.495978707E13;
static const float AU_TO_M = 1.495978707E11;
static const float AU_TO_KM = 1.495978707E8;
static const float MSOLAU_TO_KGM = 8.88803576E7;
static const float MSOLPERAU2_TO_GPERCM2 = 8.888035760594663E6;
static const float MSOLPERAU3_TO_GPERCM3 = 5.94128494E-7;
static const float GPERCM2_TO_KGPERM2 = 10.0;
static const float KMPERS_TO_MPERS = 1000.0;
static const float KMPERS_TO_CMPERS = 1E5;
static const float ERGPERG_TO_JPERKG = 1E-4;
static const float YR_TO_SEC = 3.154E7;

static const float TIME_UNIT = 0.159146;

static const int RADIAL_QUAN = 19;
static const int EXTRA_DATA = 4;
static const int TOT_RAD_QUAN = RADIAL_QUAN + EXTRA_DATA;
static const float ROUT_PERCS[3] = {0.90f, 0.95f, 0.99f};

static const int DENSITY = 0;
static const int TEMPERATURE = 1;