//===-- DragonFile.cpp ----------------------------------------------------===//
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
/// DragonFile.cpp
///
//===----------------------------------------------------------------------===//

#include "DragonFile.h"

DragonFile::DragonFile() {

}

DragonFile::~DragonFile() {}

bool DragonFile::Read(NameData nameData, bool formatted) {
  mNameData = nameData;
  mFileName = mNameData.name;
  mFormatted = formatted;

  mInStream.open(mFileName);
  if (!mInStream.is_open()) {
    return false;
  }
  if (mFormatted) {
    std::cout << "Reading formatted DRAGON file: " << mFileName << "\n";
    ReadHeaderForm();
    AllocateMemory();
    ReadParticleForm();
    ReadSinkForm();
  } else {
    std::cout << "Reading unformatted DRAGON file: " << mFileName << "\n";
    std::cout << " NOT IMPLEMENTED YET \n";
    // ReadHeaderUnform();
    // AllocateMemory();
    // ReadParticleDataUnform();
    // ReadSinkDataUnform();
  }

  mInStream.close();

  return true;
}

bool DragonFile::Write(std::string fileName, bool formatted) {
  mOutStream.open(fileName, (formatted) ? std::ios::out : std::ios::binary);
  if (!mOutStream.is_open()) {
    std::cout << "Could not open: " << fileName << " for writing!\n";
    return false;
  }

  if (formatted) {
    std::cout << "Writing formatted DRAGON file: " << fileName << "\n";
    Formatter formatStream(mOutStream, 18, 2, 10);
    WriteHeaderForm(formatStream);
    WriteParticleForm(formatStream);
    WriteSinkForm(formatStream);
  } else {
    std::cout << "Writing unformatted DRAGON file: " << mFileName << "\n";
    std::cout << " NOT IMPLEMENTED YET \n";
    // mBW = new BinaryWriter(mOutStream);
    // WriteHeaderUnform();
    // WriteParticleUnform();
    // WriteSinkUnform();
    // delete mBW;
  }

  mOutStream.close();

  return true;
}

void DragonFile::AllocateMemory(void) {
  mNumTot = mIntData[0];
  mNumGas = mIntData[2];
  mNumSink = mNumTot - mNumGas;

  mTime = mFloatData[0] * 1E6;

  for (int i = 0; i < mNumTot; ++i) {
    Particle *p = new Particle();
    mParticles.push_back(p);
  }

  for (int i = 0; i < mNumSink; ++i) {
    Sink *s = new Sink();
    mSinks.push_back(s);
  }
}

void DragonFile::CreateHeader(void) {
  mIntData[0] = mNumTot;
  mIntData[2] = mNumGas;
  mIntData[3] = mNumSink;
}

void DragonFile::ReadHeaderForm(void) {
  for (int i = 0; i < 20; ++i)
    mInStream >> mIntData[i];
  for (int i = 0; i < 50; ++i)
    mInStream >> mFloatData[i];
}

void DragonFile::ReadParticleForm(void) {
  float Temp[3] = {0.0f};

  // Positions
  for (int i = 0; i < mNumTot; ++i) {
    mInStream >> Temp[0] >> Temp[1] >> Temp[2];
    mParticles.at(i)->SetX(Vec3(Temp[0] * PC_TO_AU,
                               Temp[1] * PC_TO_AU,
                               Temp[2] * PC_TO_AU));
  }

  // Velocities
  for (int i = 0; i < mNumTot; ++i) {
    mInStream >> Temp[0] >> Temp[1] >> Temp[2];
    mParticles.at(i)->SetV(Vec3(Temp[0], Temp[1], Temp[2]));
  }

  // Temperature
  for (int i = 0; i < mNumTot; ++i) {
    mInStream >> Temp[0];
    mParticles.at(i)->SetT(Temp[0]);
  }

  // Smoothing length
  for (int i = 0; i < mNumTot; ++i) {
    mInStream >> Temp[0];
    mParticles.at(i)->SetH(Temp[0] * PC_TO_AU);
  }

  // Density
  for (int i = 0; i < mNumTot; ++i) {
    mInStream >> Temp[0];
    mParticles.at(i)->SetD(Temp[0]);
  }

  // Mass
  for (int i = 0; i < mNumTot; ++i) {
    mInStream >> Temp[0];
    mParticles.at(i)->SetM(Temp[0]);
  }

  // Type
  for (int i = 0; i < mNumTot; ++i) {
    mInStream >> Temp[0];
    mParticles.at(i)->SetType(Temp[0]);
  }

  // ID
  for (int i = 0; i < mNumGas; ++i) {
    mInStream >> Temp[0];
    mParticles.at(i)->SetID(Temp[0]);
  }
}

void DragonFile::ReadSinkForm(void) {
  // Requires implementation. Have Hydro particle which Sink and Particle
  // inherit from. Polymorph the sink particles into sinks.
}

void DragonFile::ReadHeaderUnform(void) {}

void DragonFile::ReadParticleUnform() {}

void DragonFile::ReadSinkUnform() {}

void DragonFile::WriteHeaderForm(Formatter formatStream) {
  mIntData[0] = mParticles.size() + mSinks.size();
  mIntData[2] = mParticles.size();
  mIntData[3] = mSinks.size();

  mFloatData[0] = mTime / 1E6;

  for (int i = 0; i < 20; ++i) formatStream << mIntData[i] << "\n";
  for (int i = 0; i < 50; ++i) formatStream << mFloatData[i] << "\n";
}

void DragonFile::WriteParticleForm(Formatter formatStream) {
  for (int i = 0; i < mNumGas; ++i) {
    for (int j = 0; j < 3; ++j)
      formatStream << mParticles[i]->GetX()[j] / PC_TO_AU << "\t";
    formatStream << "\n";
  }
  for (int i = 0; i < mNumSink; ++i) {
    for (int j = 0; j < 3; ++j)
      formatStream << mSinks[i]->GetX()[j] / PC_TO_AU << "\t";
    formatStream << "\n";
  }

  for (int i = 0; i < mNumGas; ++i) {
    for (int j = 0; j < 3; ++j)
      formatStream << mParticles[i]->GetV()[j] << "\t";
    formatStream << "\n";
  }
  for (int i = 0; i < mNumSink; ++i) {
    for (int j = 0; j < 3; ++j)
      formatStream << mSinks[i]->GetV()[j] << "\t";
    formatStream << "\n";
  }

  for (int i = 0; i < mNumGas; ++i) formatStream << mParticles[i]->GetT() << "\n";
  for (int i = 0; i < mNumSink; ++i) formatStream << mSinks[i]->GetT() << "\n";
  for (int i = 0; i < mNumGas; ++i) formatStream << mParticles[i]->GetH() / PC_TO_AU << "\n";
  for (int i = 0; i < mNumSink; ++i) formatStream << mSinks[i]->GetH() / PC_TO_AU << "\n";
  for (int i = 0; i < mNumGas; ++i) formatStream << mParticles[i]->GetD() << "\n";
  for (int i = 0; i < mNumSink; ++i) formatStream << mSinks[i]->GetD() << "\n";
  for (int i = 0; i < mNumGas; ++i) formatStream << mParticles[i]->GetM() << "\n";
  for (int i = 0; i < mNumSink; ++i) formatStream << mSinks[i]->GetM() << "\n";
  for (int i = 0; i < mNumGas; ++i) formatStream << mParticles[i]->GetType() << "\n";
  for (int i = 0; i < mNumSink; ++i) formatStream << mSinks[i]->GetType() << "\n";
  for (int i = 0; i < mNumGas; ++i) formatStream << mParticles[i]->GetID() << "\n";
  for (int i = 0; i < mNumSink; ++i) formatStream << mSinks[i]->GetID() << "\n";
}

void DragonFile::WriteSinkForm(Formatter formatStream) {

}

void DragonFile::WriteHeaderUnform(void) {}

void DragonFile::WriteParticleUnform(void) {}

void DragonFile::WriteSinkUnform(void) {}
