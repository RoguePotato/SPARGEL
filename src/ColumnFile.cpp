//===-- ColumnFile.cpp ----------------------------------------------------===//
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
/// ColumnFile.cpp
///
//===----------------------------------------------------------------------===//

#include "ColumnFile.h"

ColumnFile::ColumnFile(NameData nd) { mNameData = nd; }

ColumnFile::~ColumnFile() {
  for (int i = 0; i < mParticles.size(); ++i) {
    delete mParticles[i];
  }
  mParticles.clear();

  for (int i = 0; i < mSinks.size(); ++i) {
    delete mSinks[i];
  }
  mSinks.clear();
}

bool ColumnFile::Read() {
  mInStream.open(mNameData.name);
  if (!mInStream.is_open()) {
    std::cout << "   Could not open COLUMN file " << mNameData.name
              << "   for reading!\n\n";
    return false;
  }

  ReadHeaderForm();
  AllocateMemory();
  ReadParticleForm();
  ReadSinkForm();

  mInStream.close();

  return true;
}

bool ColumnFile::Write(std::string fileName) {
  mOutStream.open(fileName, std::ios::out);
  if (!mOutStream.is_open()) {
    std::cout << "   Could not open COLUMN file " << fileName
              << " for writing!\n\n";
    return false;
  }

  Formatter formatStream(mOutStream, 18, 2, 10);
  WriteHeaderForm(formatStream);
  WriteParticleForm(formatStream);
  WriteSinkForm(formatStream);

  mOutStream.close();

  return true;
}

void ColumnFile::AllocateMemory() {
  for (int i = 0; i < mNumGas; ++i) {
    Particle *p = new Particle();
    mParticles.push_back(p);
  }

  for (int i = 0; i < mNumSink; ++i) {
    Sink *s = new Sink();
    mSinks.push_back(s);
  }
}

bool ColumnFile::ReadHeaderForm() {
  mInStream >> mNumGas >> mNumSink >> mDimensions >> mTime;
  mNumTot = mNumGas + mNumSink;
}

void ColumnFile::ReadParticleForm() {
  FLOAT temp[10] = {};
  for (int i = 0; i < mNumGas; ++i) {
    mInStream >> temp[0] >> temp[1] >> temp[2] >> temp[3] >> temp[4] >>
        temp[5] >> temp[6] >> temp[7] >> temp[8] >> temp[9];

    mParticles[i]->SetX(Vec3(temp[0], temp[1], temp[2]));
    mParticles[i]->SetV(Vec3(temp[3], temp[4], temp[5]));
    mParticles[i]->SetM(temp[6]);
    mParticles[i]->SetH(temp[7]);
    mParticles[i]->SetD(temp[8]);
    mParticles[i]->SetU(temp[9]);
  }
}

void ColumnFile::ReadSinkForm() {
  FLOAT temp[10] = {};
  for (int i = 0; i < mNumSink; ++i) {
    mInStream >> temp[0] >> temp[1] >> temp[2] >> temp[3] >> temp[4] >>
        temp[5] >> temp[6] >> temp[7] >> temp[8] >> temp[9];

    mSinks[i]->SetX(Vec3(temp[0], temp[1], temp[2]));
    mSinks[i]->SetV(Vec3(temp[3], temp[4], temp[5]));
    mSinks[i]->SetM(temp[6]);
    mSinks[i]->SetH(temp[7]);
    mSinks[i]->SetD(temp[8]);
    mSinks[i]->SetU(temp[9]);
  }
}

void ColumnFile::WriteHeaderForm(Formatter formatStream) {
  formatStream << mNumGas << "\n";
  formatStream << mNumSink << "\n";
  formatStream << mDimensions << "\n";
  formatStream << mTime << "\n";
}

void ColumnFile::WriteParticleForm(Formatter formatStream) {
  for (int i = 0; i < mNumGas; ++i) {
    for (int j = 0; j < mDimensions; ++j) {
      formatStream << mParticles[i]->GetX()[j] << "\t";
    }
    for (int j = 0; j < mDimensions; ++j) {
      formatStream << mParticles[i]->GetV()[j] << "\t";
    }
    formatStream << mParticles[i]->GetM() << "\t";
    formatStream << mParticles[i]->GetH() << "\t";
    formatStream << mParticles[i]->GetD() << "\t";
    formatStream << mParticles[i]->GetU() << "\n";
  }
}

void ColumnFile::WriteSinkForm(Formatter formatStream) {
  for (int i = 0; i < mNumSink; ++i) {
    for (int j = 0; j < mDimensions; ++j) {
      formatStream << mSinks[i]->GetX()[j] << "\t";
    }
    for (int j = 0; j < mDimensions; ++j) {
      formatStream << mSinks[i]->GetV()[j] << "\t";
    }
    formatStream << mSinks[i]->GetM() << "\t";
    formatStream << mSinks[i]->GetH() << "\t";
    formatStream << mSinks[i]->GetD() << "\t";
    formatStream << mSinks[i]->GetU() << "\n";
  }
}
