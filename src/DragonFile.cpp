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

DragonFile::DragonFile(NameData nd, bool formatted, int extra_data) {
  mNameData = nd;
  mFormatted = formatted;
  mExtraData = extra_data;
}

DragonFile::~DragonFile() {
  for (int i = 0; i < mParticles.size(); ++i) {
    delete mParticles[i];
  }
  mParticles.clear();

  for (int i = 0; i < mSinks.size(); ++i) {
    delete mSinks[i];
  }
  mSinks.clear();
}

bool DragonFile::Read() {
  mInStream.open(mNameData.name);
  if (!mInStream.is_open()) {
    std::cout << "   Could not open DRAGON file " << mNameData.name
              << " for reading!\n\n";
    return false;
  }
  if (mFormatted) {
    ReadHeaderForm();
    AllocateMemory();
    ReadParticleForm();
    ReadSinkForm();
  } else {
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
    std::cout << "   Could not open DRAGON file " << fileName
              << " for writing!\n\n";
    return false;
  }

  if (formatted) {
    Formatter formatStream(mOutStream, 18, 2, 10);
    WriteHeaderForm(formatStream);
    WriteParticleForm(formatStream);
    WriteSinkForm(formatStream);
  } else {
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

void DragonFile::AllocateMemory() {
  mNumTot = mIntData[0];
  mNumGas = mIntData[2];
  mNumSink = mNumTot - mNumGas;

  mTime = mFloatData[0] * 1E6;

  for (int i = 0; i < mNumGas; ++i) {
    Particle *p = new Particle();
    mParticles.push_back(p);
  }

  for (int i = 0; i < mNumSink; ++i) {
    Sink *s = new Sink();
    mSinks.push_back(s);
  }
}

void DragonFile::CreateHeader() {
  mIntData[0] = mNumTot;
  mIntData[2] = mNumGas;
  mIntData[3] = mNumSink;
}

bool DragonFile::ReadHeaderForm() {
  for (int i = 0; i < 20; ++i)
    mInStream >> mIntData[i];
  for (int i = 0; i < 50; ++i)
    mInStream >> mFloatData[i];

  return true;
}

void DragonFile::ReadParticleForm() {
  FLOAT temp[3] = {0.0};
  
  // Positions
  for (int i = 0; i < mNumGas; ++i) {
    mInStream >> temp[0] >> temp[1] >> temp[2];
    mParticles[i]->SetX(
        Vec3(temp[0] * PC_TO_AU, temp[1] * PC_TO_AU, temp[2] * PC_TO_AU));
  }
  for (int i = 0; i < mNumSink; ++i) {
    mInStream >> temp[0] >> temp[1] >> temp[2];
    mSinks[i]->SetX(
        Vec3(temp[0] * PC_TO_AU, temp[1] * PC_TO_AU, temp[2] * PC_TO_AU));
  }

  // Velocities
  for (int i = 0; i < mNumGas; ++i) {
    mInStream >> temp[0] >> temp[1] >> temp[2];
    mParticles[i]->SetV(Vec3(temp[0], temp[1], temp[2]));
  }
  for (int i = 0; i < mNumSink; ++i) {
    mInStream >> temp[0] >> temp[1] >> temp[2];
    mSinks[i]->SetV(Vec3(temp[0], temp[1], temp[2]));
  }

  // temperature
  for (int i = 0; i < mNumGas; ++i) {
    mInStream >> temp[0];
    mParticles[i]->SetT(temp[0]);
  }
  for (int i = 0; i < mNumSink; ++i) {
    mInStream >> temp[0];
    mSinks[i]->SetT(temp[0]);
  }

  // Smoothing length
  for (int i = 0; i < mNumGas; ++i) {
    mInStream >> temp[0];
    mParticles[i]->SetH(temp[0] * PC_TO_AU);
  }
  for (int i = 0; i < mNumSink; ++i) {
    mInStream >> temp[0];
    mSinks[i]->SetH(temp[0] * PC_TO_AU);
  }

  // Density
  for (int i = 0; i < mNumGas; ++i) {
    mInStream >> temp[0];
    mParticles[i]->SetD(temp[0]);
  }
  for (int i = 0; i < mNumSink; ++i) {
    mInStream >> temp[0];
    mSinks[i]->SetD(temp[0]);
  }

  // Mass
  for (int i = 0; i < mNumGas; ++i) {
    mInStream >> temp[0];
    mParticles[i]->SetM(temp[0]);
  }
  for (int i = 0; i < mNumSink; ++i) {
    mInStream >> temp[0];
    mSinks[i]->SetM(temp[0]);
  }

  // Type
  for (int i = 0; i < mNumGas; ++i) {
    mInStream >> temp[0];
    mParticles[i]->SetType(temp[0]);
  }
  for (int i = 0; i < mNumSink; ++i) {
    mInStream >> temp[0];
    mSinks[i]->SetType(temp[0]);
  }

  // ID
  for (int i = 0; i < mNumGas; ++i) {
    mInStream >> temp[0];
    mParticles[i]->SetID(temp[0]);
  }
  for (int i = 0; i < mNumSink; ++i) {
    mInStream >> temp[0];
    mSinks[i]->SetID(temp[0]);
  }

  // Extra data
  for (int n = 0; n < mExtraData; ++n) {
    for (int i = 0; i < mNumGas; ++i) {
      mInStream >> temp[0];
      mParticles[i]->SetExtra(n, temp[0]);
    }
    for (int i = 0; i < mNumSink; ++i) {
      mInStream >> temp[0];
      mSinks[i]->SetExtra(n, temp[0]);
    }
  }
}

void DragonFile::ReadSinkForm() {
  // Requires implementation. Have Hydro particle which Sink and Particle
  // inherit from. Polymorph the sink particles into sinks.
}

bool DragonFile::ReadHeaderUnform() { return true; }

void DragonFile::ReadParticleUnform() {}

void DragonFile::ReadSinkUnform() {}

void DragonFile::WriteHeaderForm(Formatter formatStream) {
  mIntData[0] = mParticles.size() + mSinks.size();
  mIntData[2] = mParticles.size();
  mIntData[3] = mSinks.size();

  mFloatData[0] = mTime / 1E6;

  for (int i = 0; i < 20; ++i)
    formatStream << mIntData[i] << "\n";
  for (int i = 0; i < 50; ++i)
    formatStream << mFloatData[i] << "\n";
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

  for (int i = 0; i < mNumGas; ++i)
    formatStream << mParticles[i]->GetT() << "\n";
  for (int i = 0; i < mNumSink; ++i)
    formatStream << mSinks[i]->GetT() << "\n";
  for (int i = 0; i < mNumGas; ++i)
    formatStream << mParticles[i]->GetH() / PC_TO_AU << "\n";
  for (int i = 0; i < mNumSink; ++i)
    formatStream << mSinks[i]->GetH() / PC_TO_AU << "\n";
  for (int i = 0; i < mNumGas; ++i)
    formatStream << mParticles[i]->GetD() << "\n";
  for (int i = 0; i < mNumSink; ++i)
    formatStream << mSinks[i]->GetD() << "\n";
  for (int i = 0; i < mNumGas; ++i)
    formatStream << mParticles[i]->GetM() << "\n";
  for (int i = 0; i < mNumSink; ++i)
    formatStream << mSinks[i]->GetM() << "\n";
  for (int i = 0; i < mNumGas; ++i)
    formatStream << mParticles[i]->GetType() << "\n";
  for (int i = 0; i < mNumSink; ++i)
    formatStream << mSinks[i]->GetType() << "\n";
  for (int i = 0; i < mNumGas; ++i)
    formatStream << mParticles[i]->GetID() << "\n";
  for (int i = 0; i < mNumSink; ++i)
    formatStream << mSinks[i]->GetID() << "\n";
}

void DragonFile::WriteSinkForm(Formatter formatStream) {}

void DragonFile::WriteHeaderUnform() {}

void DragonFile::WriteParticleUnform() {}

void DragonFile::WriteSinkUnform() {}
