//===-- FileNameExtractor.h -----------------------------------------------===//
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
/// FileNameExtractor.h takes a file name and extracts the directory, ID
/// and format.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Definitions.h"
#include "File.h"

class FileNameExtractor {
public:
  FileNameExtractor(std::string fileName);
  ~FileNameExtractor();

  NameData GetNameData() { return mND; }

private:
  NameData mND;

  void Extract();
  std::string ExtractString(std::string str, int start, char delim);
};
