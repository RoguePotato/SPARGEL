//===-- Parameters.h ------------------------------------------------------===//
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
/// Parameters.h defines functions used to read in a parameters file.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Definitions.h"

class Parameters {
public:
  Parameters();
  ~Parameters();

  int GetInt(std::string key);
  float GetFloat(std::string key);
  std::string GetString(std::string key);

  bool Read(std::string FileName);

private:
  std::ifstream mInStream;
  std::map<std::string, int> mIntParams;
  std::map<std::string, double> mFloatParams;
  std::map<std::string, std::string> mStringParams;

  void ParseLine(std::string line);
  void SetParameter(std::string key, std::string value);
  std::string GetParameter(std::string key);

  std::string TrimWhiteSpace(std::string str);
  void SetDefaultParameters();
};
