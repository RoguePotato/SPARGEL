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
    std::cout << " NOT IMPLEMENTED YET "
              << "\n\n";
    // ReadHeaderUnform();
    // AllocateMemory();
    // ReadParticleDataUnform();
    // ReadSinkDataUnform();
  }

  mInStream.close();

  return true;
}

void DragonFile::ReadHeaderForm(void) {
  for (int i = 0; i < 20; ++i)
    mInStream >> mIntData[i];
  for (int i = 0; i < 50; ++i)
    mInStream >> mFloatData[i];
}

void DragonFile::ReadHeaderUnform(void) {
  // BR = new BinaryReader(mInStream);
  //
  // for (int i = 0; i < 20; ++i)
  //   BR->ReadValue(mIntData[i]);
  // for (int i = 0; i < 50; ++i)
  //   BR->ReadValue(mFloatData[i]);
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

bool DragonFile::Write(std::string fileName, bool formatted) {
  std::cout << "Writing file: " << fileName << "\n";
  mOutStream.open(fileName);
  if (!mOutStream.is_open()) {
    return false;
  }

  for (int i = 0; i < mParticles.size(); ++i) {
    int type = mParticles.at(i)->GetType();
    if (type == 1) ++mNumGas;
    if (type == -1) ++mNumSink;
    ++mNumTot;
  }

  mIntData[0] = mNumTot;
  mIntData[2] = mNumGas;
  mIntData[3] = mNumSink;

  mFloatData[0] = mTime / 1E6;

  for (int i = 0; i < 20; ++i) mOutStream << mIntData[i] << "\n";
  for (int i = 0; i < 50; ++i) mOutStream << mFloatData[i] << "\n";

  for (int i = 0; i < mParticles.size(); ++i) {
    mOutStream << mParticles.at(i)->GetX().x / PC_TO_AU << "\t"
              << mParticles.at(i)->GetX().y / PC_TO_AU << "\t"
              << mParticles.at(i)->GetX().z / PC_TO_AU << "\n";
  }
  // for (int i = 0; i < mNumSink; ++i) {
  //   mOutStream << mSinks.at(i).SerenData[1] / PC_TO_AU << "\t"
  //             << mSinks.at(i).SerenData[2] / PC_TO_AU << "\t"
  //             << mSinks.at(i).SerenData[3] / PC_TO_AU << "\n";
  // }

  for (int i = 0; i < mParticles.size(); ++i) {
    mOutStream << mParticles.at(i)->GetV().x << "\t"
              << mParticles.at(i)->GetV().y << "\t"
              << mParticles.at(i)->GetV().z << "\n";
  }
  // for (int i = 0; i < mNumSink; ++i) {
  //   mOutStream << mSinks.at(i).SerenData[4] << "\t"
  //             << mSinks.at(i).SerenData[5] << "\t"
  //             << mSinks.at(i).SerenData[6] << "\n";
  // }

  for (int i = 0; i < mParticles.size(); ++i) mOutStream << mParticles.at(i)->GetT() << "\n";
  // for (int i = 0; i < mNumSink; ++i) mOutStream << mSinks.at(i)->GetT() << "\n";
  for (int i = 0; i < mParticles.size(); ++i) mOutStream << mParticles.at(i)->GetH() / PC_TO_AU << "\n";
  // for (int i = 0; i < mNumSink; ++i) mOutStream << mSinks.at(i)->GetH() / PC_TO_AU << "\n";
  for (int i = 0; i < mParticles.size(); ++i) mOutStream << mParticles.at(i)->GetD() << "\n";
  // for (int i = 0; i < mNumSink; ++i) mOutStream << mSinks.at(i)->GetD() << "\n";
  for (int i = 0; i < mParticles.size(); ++i) mOutStream << mParticles.at(i)->GetM() << "\n";
  // for (int i = 0; i < mNumSink; ++i) mOutStream << mSinks.at(i)->GetSerenData[8] << "\n";
  for (int i = 0; i < mParticles.size(); ++i) mOutStream << mParticles.at(i)->GetType() << "\n";
  // for (int i = 0; i < mNumSink; ++i) mOutStream << -1 << "\n";
  for (int i = 0; i < mParticles.size(); ++i) mOutStream << mParticles.at(i)->GetID() << "\n";
  // for (int i = 0; i < mNumSink; ++i) mOutStream << mSinks.at(i)->GetID() << "\n";

  mOutStream.close();
  return true;
}

void DragonFile::ReadParticleUnform() {}

void DragonFile::ReadSinkUnform() {}

void DragonFile::WriteHeaderForm(Formatter formatStream) {}

void DragonFile::WriteParticleForm(Formatter formatStream) {}

void DragonFile::WriteSinkForm(Formatter formatStream) {}

void DragonFile::WriteHeaderUnform(void) {}

void DragonFile::WriteParticleUnform(void) {}

void DragonFile::WriteSinkUnform(void) {}
