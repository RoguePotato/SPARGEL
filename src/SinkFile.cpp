//===-- SinkFile.cpp ------------------------------------------------------===//
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
/// SinkFile.cpp
///
//===----------------------------------------------------------------------===//

#include "SinkFile.h"

SinkFile::SinkFile(NameData nd) : mNameData(nd) {

}

SinkFile::~SinkFile() {
  for (int i = 0; i < mRecords.size(); ++i) {
    delete mRecords[i];
  }
  mRecords.clear();
}

bool SinkFile::Read() {
  mInStream.open(mNameData.name, std::ios::in);
  if (!mInStream.is_open()) {
    std::cout << "   Could not open SINK file " << mNameData.name
              << " for reading!\n\n";
    return false;
  }

  FLOAT temp[27] = {};
  std::string line;
  while (getline(mInStream, line)) {
    std::istringstream istr(line);
    for (int i = 0; i < 27; ++i) istr >> temp[i];
    SinkRecord *r = new SinkRecord();
    r->time = temp[0] * TIME_UNIT;
    r->nsteps = temp[1];
    r->pos = Vec3(temp[2], temp[3], temp[4]);
    r->vel = Vec3(temp[5], temp[6], temp[7]);
    r->m = temp[14];
    mRecords.push_back(r);
  }

  return true;
}
