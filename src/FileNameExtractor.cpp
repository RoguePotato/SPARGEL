//===-- FileNameExtractor.cpp ---------------------------------------------===//
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
/// FileNameExtractor.cpp
///
//===----------------------------------------------------------------------===///

#include "FileNameExtractor.h"

FileNameExtractor::FileNameExtractor(std::string fileName) {
  mND.name = fileName;
  Extract();
}

FileNameExtractor::~FileNameExtractor() {}

void FileNameExtractor::Extract() {
  int pos = 0;
  mND.snap = ExtractString(mND.name, pos, '.');
  pos += mND.snap.size() + 1;
  mND.format = ExtractString(mND.name, pos, '.');
  pos += mND.format.size() + 1;
  mND.id = ExtractString(mND.name, pos, '/');
  pos += mND.id.size() + 1;
  mND.dir = ExtractString(mND.name, pos, '=');
}

std::string FileNameExtractor::ExtractString(std::string str, int start,
                                             char delim) {
  std::string res = "";
  for (int i = str.size() - start - 1; i >= 0; --i) {
    if (str.at(i) == delim)
      break;
    res += str.at(i);
  }
  std::reverse(res.begin(), res.end());
  return res;
}
