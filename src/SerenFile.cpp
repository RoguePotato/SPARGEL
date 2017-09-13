//===-- SerenFile.cpp -----------------------------------------------------===//
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
/// SerenFile.cpp
///
//===----------------------------------------------------------------------===//

#include "SerenFile.h"

SerenFile::SerenFile(NameData nameData, bool formatted) {
  mNameData = nameData;
  mFileName = mNameData.name;
  mFormatted = formatted;
}

SerenFile::~SerenFile() {

}

bool SerenFile::Read(void) {
  mInStream.open(mFileName, (mFormatted) ? std::ios::in : std::ios::binary);
  if (!mInStream.is_open()) {
    std::cout << "Could not open: " << mFileName << " for reading!\n";
    return false;
  }
  std::cout << "Reading SEREN file: " << mFileName << "\n";

  if (mFormatted) {
    ReadHeaderForm();
    AllocateMemory();
    ReadParticleForm();
    ReadSinkForm();
  }
  else {
    ReadHeaderUnform();
    AllocateMemory();
    ReadParticleUnform();
    ReadSinkUnform();
  }

  mInStream.close();

  return true;
}

bool SerenFile::Write(std::string fileName, bool formatted) {
  mOutStream.open(fileName, (formatted) ? std::ios::out : std::ios::binary);
  if (!mOutStream.is_open()) {
    std::cout << "Could not open: " << fileName << " for writing!\n";
    return false;
  }
  std::cout << "Writing file: " << fileName << "\n";

  if (formatted) {
    Formatter formatStream(mOutStream, 18, 2, 10);
    WriteHeaderForm(formatStream);
    WriteParticleForm(formatStream);
    WriteSinkForm(formatStream);
  } else {
    mBW = new BinaryWriter(mOutStream);
    WriteHeaderUnform();
    WriteParticleUnform();
    WriteSinkUnform();
    delete mBW;
  }

  mOutStream.close();

  return true;
}

void SerenFile::AllocateMemory(void) {
  mPrecision = mHeader[0];
  mPosDim = mHeader[1];
  mVelDim = mHeader[2];
  mMagDim = mHeader[3];

  mNumGas = mIntData[0];
  mNumSink = mIntData[1];
  mNumTot = mNumGas + mNumSink;

  mTime = mDoubleData[0];

  mSinkDataLength = 12 + 2 * mPosDim;

  for (int i = 0; i < mNumGas; ++i) {
    Particle *p = new Particle();
    mParticles.push_back(p);
  }

  for (int i = 0; i < mNumSink; ++i) {
    Sink *s = new Sink();
    mSinks.push_back(s);
  }
}

void SerenFile::ReadHeaderForm(void) {
  std::string temp = "";

  mInStream >> mFormatID;
  for (int i = 0; i < 4; ++i)
    mInStream >> mHeader[i];
  for (int i = 0; i < 50; ++i)
    mInStream >> mIntData[i];
  for (int i = 0; i < 50; ++i)
    mInStream >> mLongData[i];
  for (int i = 0; i < 50; ++i)
    mInStream >> mFloatData[i];
  for (int i = 0; i < 50; ++i)
    mInStream >> mDoubleData[i];

  mNumUnit = mIntData[19];
  mNumData = mIntData[20];

  for (int i = 0; i < mNumUnit; ++i) {
    mInStream >> temp;
    mUnitData.push_back(temp);
  }

  for (int i = 0; i < mNumData; ++i) {
    mInStream >> temp;
    mDataID.push_back(temp);
  }

  for (int i = 0; i < mNumData; ++i) {
    for (int j = 0; j < 5; ++j) {
      mInStream >> mTypeData[i][j];
    }
  }
}

void SerenFile::ReadParticleForm(void) {
  double temp[3] = {0.0};

  for (int i = 0; i < mNumGas; ++i) {
    mInStream >> temp[0];
    mParticles.at(i)->SetID(temp[0]);
  }

  for (int i = 0; i < mNumGas; ++i) {
    if (mPosDim == 1)
      mInStream >> temp[0];
    if (mPosDim == 2)
      mInStream >> temp[0] >> temp[1];
    if (mPosDim == 3)
      mInStream >> temp[0] >> temp[1] >> temp[2];

    mParticles.at(i)->SetX(Vec3(temp[0], temp[1], temp[2]));
  }

  for (int i = 0; i < mNumGas; ++i) {
    mInStream >> temp[0];
    mParticles.at(i)->SetM(temp[0]);
  }

  for (int i = 0; i < mNumGas; ++i) {
    mInStream >> temp[0];
    mParticles.at(i)->SetH(temp[0]);
  }

  for (int i = 0; i < mNumGas; ++i) {
    if (mVelDim == 1)
      mInStream >> temp[0];
    if (mVelDim == 2)
      mInStream >> temp[0] >> temp[1];
    if (mVelDim == 3)
      mInStream >> temp[0] >> temp[1] >> temp[2];

    mParticles.at(i)->SetV(Vec3(temp[0], temp[1], temp[2]));
  }

  for (int i = 0; i < mNumGas; ++i) {
    mInStream >> temp[0];
    mParticles.at(i)->SetD(temp[0]);
  }

  for (int i = 0; i < mNumGas; ++i) {
    mInStream >> temp[0];
    mParticles.at(i)->SetU(temp[0]);
  }
}

void SerenFile::ReadSinkForm(void) {
  double temp[3] = {0.0};
  std::string dummyStr = "";

  for (int i = 0; i < 6; ++i)
    mInStream >> temp[0];

  for (int i = 0; i < mNumSink; ++i) {
    mInStream >> dummyStr;
    mInStream >> temp[0] >> temp[1];
    mSinks.at(i)->SetID(temp[0]);

    for (int j = 0; j < mSinkDataLength; ++j) {
      mInStream >> temp[0];
      mSinks.at(i)->SetData(j, temp[0]);
    }
  }
}

void SerenFile::ReadHeaderUnform(void) {
  mBR = new BinaryReader(mInStream);

  std::vector<char> fileTag(STRING_LENGTH);
  mInStream.read(&fileTag[0], STRING_LENGTH);
  std::string concatString(fileTag.begin(), fileTag.end());
  mFormatID = TrimWhiteSpace(concatString);

  for (int i = 0; i < 4; ++i)
    mBR->ReadValue(mHeader[i]);
  for (int i = 0; i < 50; ++i)
    mBR->ReadValue(mIntData[i]);
  for (int i = 0; i < 50; ++i)
    mBR->ReadValue(mLongData[i]);
  for (int i = 0; i < 50; ++i)
    mBR->ReadValue(mFloatData[i]);
  for (int i = 0; i < 50; ++i)
    mBR->ReadValue(mDoubleData[i]);

  mNumUnit = mIntData[19];
  mNumData = mIntData[20];

  for (int i = 0; i < mNumUnit; ++i) {
    char buffer[STRING_LENGTH];
    mInStream.read(buffer, STRING_LENGTH);
    mUnitData.push_back(TrimWhiteSpace(std::string(buffer, STRING_LENGTH)));
  }

  for (int i = 0; i < mNumData; ++i) {
    char buffer[STRING_LENGTH];
    mInStream.read(buffer, STRING_LENGTH);
    mDataID.push_back(std::string(buffer, STRING_LENGTH));
  }

  for (int i = 0; i < mNumData; ++i) {
    for (int j = 0; j < 5; j++) {
      mBR->ReadValue(mTypeData[i][j]);
    }
  }
}

void SerenFile::ReadParticleUnform(void) {
  int intTemp = 0;
  double temp[3] = {0.0, 0.0, 0.0};

  for (int i = 0; i < mNumGas; ++i) {
    mBR->ReadValue(intTemp);
    mParticles.at(i)->SetID(0);
  }

  for (int i = 0; i < mNumGas; ++i) {
    for (int i = 0; i < mPosDim; ++i)
      mBR->ReadValue(temp[i]);
    mParticles.at(i)->SetX(Vec3(temp[0], temp[1], temp[2]));
  }

  for (int i = 0; i < mNumGas; ++i) {
    mBR->ReadValue(temp[0]);
    mParticles.at(i)->SetM(temp[0]);
  }

  for (int i = 0; i < mNumGas; ++i) {
    mBR->ReadValue(temp[0]);
    mParticles.at(i)->SetH(temp[0]);
  }

  for (int i = 0; i < mNumGas; ++i) {
    for (int i = 0; i < mVelDim; ++i)
      mBR->ReadValue(temp[i]);
    mParticles.at(i)->SetV(Vec3(temp[0], temp[1], temp[2]));
  }

  for (int i = 0; i < mNumGas; ++i) {
    mBR->ReadValue(temp[0]);
    mParticles.at(i)->SetD(temp[0]);
  }

  for (int i = 0; i < mNumGas; ++i) {
    mBR->ReadValue(temp[0]);
    mParticles.at(i)->SetU(temp[0]);
  }
}

void SerenFile::ReadSinkUnform(void) {
  double temp[3] = {0.0};
  int tempInt = 0;
  int dummyBool = 0;

  for (int i = 0; i < 6; ++i)
    mBR->ReadValue(tempInt);

  for (int i = 0; i < mSinks.size(); ++i) {
    for (int j = 0; j < 2; ++j)
      mBR->ReadValue(dummyBool);
    for (int j = 0; j < 2; ++j)
      mBR->ReadValue(tempInt);

    for (int j = 0; j < mSinkDataLength; ++j) {
      mBR->ReadValue(temp[0]);
      mSinks.at(i)->SetData(j, temp[0]);
    }
  }
}

void SerenFile::WriteHeaderForm(Formatter formatStream) {
  mOutStream << ASCII_FORMAT << "\n";
  for (int i = 0; i < 4; ++i)
    formatStream << mHeader[i] << "\n";
  formatStream.SetWidthInteger(10);
  for (int i = 0; i < 50; ++i)
    formatStream << mIntData[i] << "\n";
  for (int i = 0; i < 50; ++i)
    formatStream << mLongData[i] << "\n";
  for (int i = 0; i < 50; ++i)
    formatStream << mFloatData[i] << "\n";
  for (int i = 0; i < 50; ++i)
    formatStream << mDoubleData[i] << "\n";

  for (int i = 0; i < mNumUnit; ++i) {
    std::ostringstream SStream;
    SStream << std::left << std::setw(STRING_LENGTH) << std::setfill(' ')
            << mUnitData.at(i);
    formatStream << SStream.str() << "\n";
  }

  for (int i = 0; i < mNumData; ++i) {
    std::ostringstream SStream;
    SStream << std::left << std::setw(STRING_LENGTH) << std::setfill(' ')
            << mDataID.at(i);
    formatStream << SStream.str() << "\n";
  }

  for (int i = 0; i < mNumData; ++i) {
    for (int j = 0; j < 5; ++j) {
      formatStream << mTypeData[i][j] << "\t";
    }
    formatStream << "\n";
  }
}

void SerenFile::WriteParticleForm(Formatter formatStream) {
  for (int i = 0; i < mNumGas; ++i)
    formatStream << mParticles[i]->GetID() << "\n";

  for (int i = 0; i < mNumGas; ++i) {
    for (int j = 0; j < mPosDim; ++j)
      formatStream << mParticles[i]->GetX()[j] << "\t";
    formatStream << "\n";
  }

  for (int i = 0; i < mNumGas; ++i)
    formatStream << mParticles[i]->GetM() << "\n";

  for (int i = 0; i < mNumGas; ++i)
    formatStream << mParticles[i]->GetH() << "\n";

  for (int i = 0; i < mNumGas; ++i) {
    for (int j = 0; j < mVelDim; ++j)
      formatStream << mParticles[i]->GetV()[j] << "\t";
    formatStream << "\n";
  }

  for (int i = 0; i < mNumGas; ++i)
    formatStream << mParticles[i]->GetD() << "\n";

  for (int i = 0; i < mNumGas; ++i)
    formatStream << mParticles[i]->GetU() << "\n";
}

void SerenFile::WriteSinkForm(Formatter formatStream) {
  int sinkValues[6] = {2, 2, 0, mSinkDataLength, 0, 0};
  for (int i = 0; i < 6; ++i)
    formatStream << sinkValues[i];
  formatStream << "\n";
  formatStream.SetWidthInteger(8);
  for (int i = 0; i < mNumSink; ++i) {
    formatStream << "TT"
                 << "\n";
    formatStream << mSinks[i]->GetID() << 0 << "\n";

    for (int j = 0; j < 8; ++j)
      formatStream << mSinks[i]->GetData(j);

    formatStream << "\n";
  }
}

void SerenFile::WriteHeaderUnform(void) {
  std::ostringstream stream;
  stream << std::left << std::setw(STRING_LENGTH)
         << std::setfill(' ') << BINARY_FORMAT;
  mOutStream << stream.str();

  for (int i = 0; i < 4; ++i)
    mBW->WriteValue(mHeader[i]);
  for (int i = 0; i < 50; ++i)
    mBW->WriteValue(mIntData[i]);
  for (int i = 0; i < 50; ++i)
    mBW->WriteValue(mLongData[i]);
  for (int i = 0; i < 50; ++i)
    mBW->WriteValue(mFloatData[i]);
  for (int i = 0; i < 50; ++i)
    mBW->WriteValue(mDoubleData[i]);

  for (int i = 0; i < mNumUnit; ++i) {
    std::ostringstream stream;
    stream << std::left << std::setw(STRING_LENGTH)
           << std::setfill(' ') << mUnitData[i];
    mOutStream << stream.str();
  }
  for (int i = 0; i < mNumData; ++i) {
    std::ostringstream stream;
    stream << std::left << std::setw(STRING_LENGTH)
           << std::setfill(' ') << mDataID[i];
    mOutStream << stream.str();
  }

  for (int i = 0; i < mNumData; ++i) {
    for (int j = 0; j < 5; ++j)
      mBW->WriteValue(mTypeData[i][j]);
  }
}

void SerenFile::WriteParticleUnform(void) {
  for (int i = 0; i < mNumGas; ++i)
    mBW->WriteValue(mParticles[i]->GetID());

  for (int i = 0; i < mNumGas; ++i) {
    for (int j = 0; j < mPosDim; ++j) {
      mBW->WriteValue(mParticles[i]->GetX()[j]);
    }
  }

  for (int i = 0; i < mNumGas; ++i)
    mBW->WriteValue(mParticles[i]->GetM());

  for (int i = 0; i < mNumGas; ++i)
    mBW->WriteValue(mParticles[i]->GetH());

  for (int i = 0; i < mNumGas; ++i) {
    for (int j = 0; j < mVelDim; ++j) {
      mBW->WriteValue(mParticles[i]->GetV()[j]);
    }
  }

  for (int i = 0; i < mNumGas; ++i)
    mBW->WriteValue(mParticles[i]->GetD());

  for (int i = 0; i < mNumGas; ++i)
    mBW->WriteValue(mParticles[i]->GetU());
}

void SerenFile::WriteSinkUnform(void) {
  int sinkValues[6] = {2, 2, 0, mSinkDataLength, 0, 0};
  for (int i = 0; i < 6; ++i)
    mBW->WriteValue(sinkValues[i]);

  for (int i = 0; i < mNumSink; ++i) {
    mBW->WriteValue(true);
    mBW->WriteValue(true);
    mBW->WriteValue(i + 1);
    mBW->WriteValue(0);

    for (int j = 0; j < mSinkDataLength; ++j)
      mBW->WriteValue(mSinks[i]->GetData(j));
  }
}
