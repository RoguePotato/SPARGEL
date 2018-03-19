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

SerenFile::SerenFile(NameData nd, bool formatted) {
  mNameData = nd;
  mFormatted = formatted;
}

SerenFile::~SerenFile() {
  for (int i = 0; i < mParticles.size(); ++i) {
    delete mParticles[i];
  }
  mParticles.clear();

  for (int i = 0; i < mSinks.size(); ++i) {
    delete mSinks[i];
  }
  mSinks.clear();
}

bool SerenFile::Read() {
  mInStream.open(mNameData.name,
                 (mFormatted) ? std::ios::in : std::ios::binary);
  if (!mInStream.is_open()) {
    std::cout << "   Could not open SEREN file " << mNameData.name
              << " for reading!\n\n";
    return false;
  }

  if (mFormatted) {
    if (!ReadHeaderForm()) {
      std::cout << "   Error reading SEREN header format!\n\n";
      return false;
    }
    AllocateMemory();
    ReadParticleForm();
    ReadSinkForm();
  } else {
    mBR = new BinaryReader(mInStream);
    if (!ReadHeaderUnform()) {
      std::cout << "   Error reading SEREN header format!\n\n";
      return false;
    }
    AllocateMemory();
    ReadParticleUnform();
    ReadSinkUnform();
    delete mBR;
  }

  mInStream.close();

  // Set sink data
  for (int i = 0; i < mSinks.size(); ++i) {
    double *curData = mSinks.at(i)->GetAllData();
    mSinks.at(i)->SetX(Vec3(curData[1], curData[2], curData[3]));
    mSinks.at(i)->SetV(Vec3(curData[4], curData[5], curData[6]));
    mSinks.at(i)->SetM(curData[7]);
    mSinks.at(i)->SetH(curData[8]);
    mSinks.at(i)->SetType(-1);
  }

  return true;
}

bool SerenFile::Write(std::string fileName, bool formatted) {
  mOutStream.open(fileName, (formatted) ? std::ios::out : std::ios::binary);
  if (!mOutStream.is_open()) {
    std::cout << "   Could not open SEREN file " << fileName
              << " for writing!\n";
    return false;
  }
  std::cout << "   Writing " << fileName << "\n";

  CreateHeader();

  // Get sink data, pertinent information starts at index 1
  for (int i = 0; i < mNumSink; ++i) {
    mSinks[i]->SetData(0 + 1, mSinks[i]->GetX().x);
    mSinks[i]->SetData(1 + 1, mSinks[i]->GetX().y);
    mSinks[i]->SetData(2 + 1, mSinks[i]->GetX().z);
    mSinks[i]->SetData(3 + 1, mSinks[i]->GetV().x);
    mSinks[i]->SetData(4 + 1, mSinks[i]->GetV().y);
    mSinks[i]->SetData(5 + 1, mSinks[i]->GetV().z);
    mSinks[i]->SetData(6 + 1, mSinks[i]->GetM());
    mSinks[i]->SetData(7 + 1, mSinks[i]->GetH());
  }

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
  mNumDust = mIntData[6];
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

void SerenFile::CreateHeader(void) {
  mNumGas = mParticles.size();
  mNumSink = mSinks.size();

  mHeader[0] = mPrecision;
  mHeader[1] = mPosDim;
  mHeader[2] = mVelDim;
  mHeader[3] = mMagDim;

  // TODO: Replace string vectors
  // with array size 50
  for (int i = 0; i < 50; ++i) {
    mUnitData.push_back("");
    mDataID.push_back("");
  }

  mUnitData[0] = "au";
  mUnitData[1] = "m_sun";
  mUnitData[2] = "yr";
  mUnitData[3] = "km_s";
  mUnitData[4] = "km_s2";
  mUnitData[5] = "g_cm3";
  mUnitData[6] = "g_cm2";
  mUnitData[7] = "Pa";
  mUnitData[8] = "N";
  mUnitData[9] = "J";
  mUnitData[10] = "m_sunkm_s";
  mUnitData[11] = "m_sunkm2_s";
  mUnitData[12] = "rad_s";
  mUnitData[13] = "m_sun_yr";
  mUnitData[14] = "L_sun";
  mUnitData[15] = "cm2_g";
  mUnitData[16] = "cm2_g";
  mUnitData[17] = "cm2_g";
  mUnitData[18] = "cm2_g";
  mUnitData[19] = "J_kg";
  mUnitData[20] = "K";
  mNumUnit = 21;

  mNumData = 0;
  mDataID[mNumData] = "porig";
  mTypeData[mNumData][0] = 1;
  mTypeData[mNumData][1] = 1;
  mTypeData[mNumData][2] = mNumGas;
  mTypeData[mNumData][3] = 2;
  mTypeData[mNumData][4] = 0;
  mNumData++;

  mDataID[mNumData] = "r";
  mTypeData[mNumData][0] = mPosDim;
  mTypeData[mNumData][1] = 1;
  mTypeData[mNumData][2] = mNumGas;
  mTypeData[mNumData][3] = 4;
  mTypeData[mNumData][4] = 1;
  mNumData++;

  mDataID[mNumData] = "m";
  mTypeData[mNumData][0] = 1;
  mTypeData[mNumData][1] = 1;
  mTypeData[mNumData][2] = mNumGas;
  mTypeData[mNumData][3] = 4;
  mTypeData[mNumData][4] = 2;
  mNumData++;

  mDataID[mNumData] = "h";
  mTypeData[mNumData][0] = 1;
  mTypeData[mNumData][1] = 1;
  mTypeData[mNumData][2] = mNumGas;
  mTypeData[mNumData][3] = 4;
  mTypeData[mNumData][4] = 1;
  mNumData++;

  mDataID[mNumData] = "v";
  mTypeData[mNumData][0] = mVelDim;
  mTypeData[mNumData][1] = 1;
  mTypeData[mNumData][2] = mNumGas;
  mTypeData[mNumData][3] = 4;
  mTypeData[mNumData][4] = 4;
  mNumData++;

  mDataID[mNumData] = "rho";
  mTypeData[mNumData][0] = 1;
  mTypeData[mNumData][1] = 1;
  mTypeData[mNumData][2] = mNumGas;
  mTypeData[mNumData][3] = 4;
  mTypeData[mNumData][4] = 6;
  mNumData++;

  mDataID[mNumData] = "u";
  mTypeData[mNumData][0] = 1;
  mTypeData[mNumData][1] = 1;
  mTypeData[mNumData][2] = mNumGas;
  mTypeData[mNumData][3] = 4;
  mTypeData[mNumData][4] = 20;
  mNumData++;

  mDataID[mNumData] = "sink_v1";
  mTypeData[mNumData][0] = 1;
  mTypeData[mNumData][1] = 1;
  mTypeData[mNumData][2] = mNumSink;
  mTypeData[mNumData][3] = 7;
  mTypeData[mNumData][4] = 0;
  mNumData++;

  mIntData[0] = mNumGas;
  mIntData[1] = mNumSink;
  mIntData[3] = 0;        // icm_type
  mIntData[4] = mNumGas;  // gas_type
  mIntData[5] = 0;        // cdm_type (cold dark matter?)
  mIntData[6] = mNumDust; // dust_type
  mIntData[19] = mNumUnit;
  mIntData[20] = mNumData;
  mLongData[0] = 0;    // Noutsnap
  mLongData[1] = 0;    // Nsteps
  mLongData[10] = 0;   // Noutlitesnap
  mFloatData[0] = 1.2; // h_fac
  mFloatData[1] = 0.0;
  mDoubleData[0] = 0.0;                   // time
  mDoubleData[1] = 0.0;                   // time lastsnap
  mDoubleData[2] = mParticles[0]->GetM(); // avg. hydro mass
  mDoubleData[10] = 0.0;                  // tlite lastsnap

  mSinkDataLength = 12 + 2 * mPosDim;
}

bool SerenFile::ReadHeaderForm(void) {
  std::string temp = "";

  mInStream >> mFormatID;
  if (mFormatID.compare("SERENASCIIDUMPV2"))
    return false;

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

  return true;
}

void SerenFile::ReadParticleForm(void) {
  double temp[3] = {0.0};

  for (int i = 0; i < mNumGas + mNumDust; ++i) {
    mInStream >> temp[0];
    mParticles.at(i)->SetID(temp[0]);
  }

  for (int i = 0; i < mNumGas + mNumDust; ++i) {
    if (mPosDim == 1)
      mInStream >> temp[0];
    if (mPosDim == 2)
      mInStream >> temp[0] >> temp[1];
    if (mPosDim == 3)
      mInStream >> temp[0] >> temp[1] >> temp[2];

    mParticles.at(i)->SetX(Vec3(temp[0], temp[1], temp[2]));
  }

  for (int i = 0; i < mNumGas + mNumDust; ++i) {
    mInStream >> temp[0];
    mParticles.at(i)->SetM(temp[0]);
  }

  for (int i = 0; i < mNumGas + mNumDust; ++i) {
    mInStream >> temp[0];
    mParticles.at(i)->SetH(temp[0]);
  }

  for (int i = 0; i < mNumGas + mNumDust; ++i) {
    if (mVelDim == 1)
      mInStream >> temp[0];
    if (mVelDim == 2)
      mInStream >> temp[0] >> temp[1];
    if (mVelDim == 3)
      mInStream >> temp[0] >> temp[1] >> temp[2];

    mParticles.at(i)->SetV(Vec3(temp[0], temp[1], temp[2]));
  }

  for (int i = 0; i < mNumGas + mNumDust; ++i) {
    mInStream >> temp[0];
    mParticles.at(i)->SetD(temp[0]);
  }

  for (int i = 0; i < mNumGas + mNumDust; ++i) {
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

bool SerenFile::ReadHeaderUnform(void) {
  std::vector<char> fileTag(STRING_LENGTH);
  mInStream.read(&fileTag[0], STRING_LENGTH);
  std::string concatString(fileTag.begin(), fileTag.end());
  mFormatID = TrimWhiteSpace(concatString);
  if (mFormatID.compare("SERENBINARYDUMPV3"))
    return false;

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

  return true;
}

void SerenFile::ReadParticleUnform(void) {
  int intTemp = 0;
  double temp[3] = {0.0, 0.0, 0.0};

  for (int i = 0; i < mNumGas + mNumDust; ++i) {
    mBR->ReadValue(intTemp);
    mParticles.at(i)->SetID(intTemp);
  }

  for (int i = 0; i < mNumGas + mNumDust; ++i) {
    for (int i = 0; i < mPosDim; ++i)
      mBR->ReadValue(temp[i]);
    mParticles.at(i)->SetX(Vec3(temp[0], temp[1], temp[2]));
  }

  for (int i = 0; i < mNumGas + mNumDust; ++i) {
    mBR->ReadValue(temp[0]);
    mParticles.at(i)->SetM(temp[0]);
  }

  for (int i = 0; i < mNumGas + mNumDust; ++i) {
    mBR->ReadValue(temp[0]);
    mParticles.at(i)->SetH(temp[0]);
  }

  for (int i = 0; i < mNumGas + mNumDust; ++i) {
    for (int i = 0; i < mVelDim; ++i)
      mBR->ReadValue(temp[i]);
    mParticles.at(i)->SetV(Vec3(temp[0], temp[1], temp[2]));
  }

  for (int i = 0; i < mNumGas + mNumDust; ++i) {
    mBR->ReadValue(temp[0]);
    mParticles.at(i)->SetD(temp[0]);
  }

  for (int i = 0; i < mNumGas + mNumDust; ++i) {
    mBR->ReadValue(temp[0]);
    mParticles.at(i)->SetU(temp[0]);
  }

  // std::cout << "    Reading pseudo-column density!\n";
  // for (int i = 0; i < mNumGas + mNumDust; ++i) {
  //   mBR->ReadValue(temp[0]);
  //   mParticles.at(i)->SetSigma(temp[0]);
  // }
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
  for (int i = 0; i < mNumDust; ++i)
    formatStream << mParticles[i]->GetID() << "\n";

  for (int i = 0; i < mNumDust; ++i) {
    for (int j = 0; j < mPosDim; ++j)
      formatStream << mParticles[i]->GetX()[j] << "\t";
    formatStream << "\n";
  }

  for (int i = 0; i < mNumDust; ++i)
    formatStream << mParticles[i]->GetM() << "\n";

  for (int i = 0; i < mNumDust; ++i)
    formatStream << mParticles[i]->GetH() << "\n";

  for (int i = 0; i < mNumDust; ++i) {
    for (int j = 0; j < mVelDim; ++j)
      formatStream << mParticles[i]->GetV()[j] << "\t";
    formatStream << "\n";
  }

  for (int i = 0; i < mNumDust; ++i)
    formatStream << mParticles[i]->GetD() << "\n";

  for (int i = 0; i < mNumDust; ++i)
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
  stream << std::left << std::setw(STRING_LENGTH) << std::setfill(' ')
         << BINARY_FORMAT;
  mBW->WriteValue(stream.str());

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
    stream << std::left << std::setw(STRING_LENGTH) << std::setfill(' ')
           << mUnitData[i];
    mOutStream << stream.str();
  }
  for (int i = 0; i < mNumData; ++i) {
    std::ostringstream stream;
    stream << std::left << std::setw(STRING_LENGTH) << std::setfill(' ')
           << mDataID[i];
    mOutStream << stream.str();
  }

  for (int i = 0; i < mNumData; ++i) {
    for (int j = 0; j < 5; ++j)
      mBW->WriteValue(mTypeData[i][j]);
  }
}

void SerenFile::WriteParticleUnform(void) {
  for (int i = 0; i < mNumGas + mNumDust; ++i)
    mBW->WriteValue(mParticles[i]->GetID());

  for (int i = 0; i < mNumGas + mNumDust; ++i) {
    for (int j = 0; j < mPosDim; ++j) {
      mBW->WriteValue(mParticles[i]->GetX()[j]);
    }
  }

  for (int i = 0; i < mNumGas + mNumDust; ++i)
    mBW->WriteValue(mParticles[i]->GetM());

  for (int i = 0; i < mNumGas + mNumDust; ++i)
    mBW->WriteValue(mParticles[i]->GetH());

  for (int i = 0; i < mNumGas + mNumDust; ++i) {
    for (int j = 0; j < mVelDim; ++j) {
      mBW->WriteValue(mParticles[i]->GetV()[j]);
    }
  }

  for (int i = 0; i < mNumGas + mNumDust; ++i)
    mBW->WriteValue(mParticles[i]->GetD());

  for (int i = 0; i < mNumGas + mNumDust; ++i)
    mBW->WriteValue(mParticles[i]->GetU());
}

void SerenFile::WriteSinkUnform(void) {
  int sinkValues[6] = {2, 2, 0, mSinkDataLength, 0, 0};
  for (int i = 0; i < 6; ++i) {
    mBW->WriteValue(sinkValues[i]);
  }

  for (int i = 0; i < mNumSink; ++i) {
    mBW->WriteValue(true);
    mBW->WriteValue(true);
    mBW->WriteValue(i + 1);
    mBW->WriteValue(0);

    for (int j = 0; j < mSinkDataLength; ++j) {
      mBW->WriteValue(mSinks[i]->GetData(j));
    }
  }
}
