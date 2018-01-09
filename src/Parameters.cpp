//===-- Parameters.cpp ----------------------------------------------------===//
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
/// Parameters.cpp
///
//===----------------------------------------------------------------------===//

#include "Parameters.h"

Parameters::Parameters() { SetDefaultParameters(); }

Parameters::~Parameters() {}

bool Parameters::Read(std::string fileName) {
  mInStream.open(fileName);
  if (!mInStream.is_open()) {
    return false;
  }

  std::string line = "";

  SetDefaultParameters();

  while (mInStream.good()) {
    std::getline(mInStream, line);
    ParseLine(line);
  }

  mInStream.close();
  return true;
}

void Parameters::ParseLine(std::string line) {
  line = TrimWhiteSpace(line);

  int colonPos = line.find(":");
  int equalPos = line.find("=");
  int hashPos = line.find("#");
  int length = line.length();

  if (hashPos == 0 || length == 0)
    return;
  if (equalPos >= length || (colonPos < length && colonPos > equalPos))
    return;

  std::string varName = line.substr(colonPos + 1, equalPos - colonPos - 1);
  std::string varVal = line.substr(equalPos + 1, length - equalPos - 1);

  SetParameter(varName, varVal);
}

void Parameters::SetParameter(std::string key, std::string value) {
  if (mIntParams.count(key) == 1) {
    std::stringstream(value) >> mIntParams[key];
  } else if (mFloatParams.count(key) == 1) {
    std::stringstream(value) >> mFloatParams[key];
  } else if (mStringParams.count(key) == 1) {
    mStringParams[key] = value;
  } else {
    std::cout << "Parameter " << key << " was not recognised!\n";
  }
}

int Parameters::GetInt(std::string key) {
  if (mIntParams.count(key) == 1) {
    return mIntParams[key];
  }

  return -1;
}

float Parameters::GetFloat(std::string key) {
  if (mFloatParams.count(key) == 1) {
    return mFloatParams[key];
  }

  return -1;
}

std::string Parameters::GetString(std::string key) {
  if (mStringParams.count(key) == 1) {
    return mStringParams[key];
  }

  return "";
}

std::string Parameters::TrimWhiteSpace(std::string str) {
  std::string result;

  for (unsigned int i = 0; i < str.length(); ++i) {
    if (str[i] != ' ' && str[i] != '\t')
      result += str[i];
  }

  return result;
}

void Parameters::SetDefaultParameters() {
  mIntParams["THREADS"] = -1;

  mStringParams["INPUT_FILE"] = "COMMAND_LINE";
  mStringParams["IN_FORMAT"] = "sf";
  mStringParams["OUTPUT_ID"] = "PDA";
  mIntParams["CONVERT"] = 0;
  mStringParams["OUT_FORMAT"] = "sf";
  mIntParams["OUTPUT_FILES"] = 0;
  mStringParams["CLOUD_OUTPUT"] = "cloud.dat";
  mStringParams["EOS_TABLE"] = "eos.bell.cc.dat";
  mIntParams["OUTPUT_COOLING"] = 0;

  mIntParams["CLOUD_ANALYSIS"] = 0;
  mIntParams["CLOUD_CENTER"] = 0;

  mIntParams["SINK_ANALYSIS"] = 0;
  mIntParams["DISC_ANALYSIS"] = 0;
  mIntParams["RADIAL_ANALYSIS"] = 0;
  mIntParams["VERTICAL_ANALYSIS"] = 0;
  mIntParams["DISC_CENTER"] = 0;
  mFloatParams["CENTER_X"] = 0.0;
  mFloatParams["CENTER_Y"] = 0.0;
  mFloatParams["CENTER_Z"] = 0.0;
  mIntParams["MIDPLANE_CUT"] = 0;
  mFloatParams["RADIUS_IN"] = 0.0;
  mFloatParams["RADIUS_OUT"] = 100.0;
  mIntParams["RADIAL_BINS"] = 100;
  mIntParams["RADIAL_LOG"] = 0;
  mFloatParams["HEIGHT_LO"] = 0.0;
  mFloatParams["HEIGHT_HI"] = 10.0;
  mIntParams["VERTICAL_BINS"] = 100;

  mIntParams["GENERATE"] = 0;
  mStringParams["IC_TYPE"] = "disc";
  mIntParams["SEED"] = 0;
  mIntParams["N_HYDRO"] = 4096;
  mIntParams["DIMENSIONS"] = 3;

  // Protostellar disc parameters
  mFloatParams["M_STAR"] = 0.5;
  mFloatParams["M_DISC"] = 0.01;

  mFloatParams["R_IN"] = 5.0;
  mFloatParams["R_OUT"] = 100.0;
  mFloatParams["R_0"] = 0.01;
  mFloatParams["T_0"] = 250.0;
  mFloatParams["T_INF"] = 5.0;
  mIntParams["N_NEIGH"] = 50;
  mFloatParams["P"] = 1.0;
  mFloatParams["Q"] = 0.75;
}
