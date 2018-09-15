//===-- File.h ------------------------------------------------------------===//
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
/// File.h contains the functions to handle file I/O. The File class is the
/// parent for all other file types.
//===----------------------------------------------------------------------===//

#pragma once

#include "BinaryIO.h"
#include "Definitions.h"
#include "Formatter.h"
#include "Particle.h"

struct NameData {
  std::string name = "";
  std::string snap = "";
  std::string format = "";
  std::string id = "";
  std::string dir = "";
  std::string append = "";
};

class File {
public:
  File(){};
  File(NameData nd, bool formatted, int extra_data){};
  virtual ~File(){};

  virtual bool Read() = 0;
  virtual bool Write(std::string fileName, bool formatted) = 0;

  virtual NameData GetNameData() { return mNameData; }
  virtual std::string GetFileName() { return mNameData.name; }

  virtual void SetNameDataFormat(std::string str) { mNameData.format = str; }
  virtual void SetNameDataAppend(std::string str) { mNameData.append += str; }

protected:
  NameData mNameData = {};

  std::ifstream mInStream;
  std::ofstream mOutStream;

  int mExtraData = 0;

  inline std::string TrimWhiteSpace(std::string str) {
    std::string result;
    for (int i = 0; i < str.length(); ++i) {
      if (str[i] != ' ' && str[i] != '\t')
        result += str[i];
    }
    return result;
  }
};

class SnapshotFile : public File {
public:
  SnapshotFile(){};
  SnapshotFile(NameData nd, bool formatted){};
  virtual ~SnapshotFile(){};

  virtual std::vector<Particle *> GetParticles() { return mParticles; }
  virtual std::vector<Sink *> GetSinks() { return mSinks; }
  virtual double GetTime() { return mTime; }
  virtual int GetNumGas() { return mNumGas; }
  virtual int GetNumDust() { return mNumDust; }
  virtual int GetNumSinks() { return mNumSink; }
  virtual int GetNumPart() { return mNumTot; }
  virtual bool GetFormatted() { return mFormatted; }

  virtual void SetParticles(std::vector<Particle *> particles) {
    mParticles = particles;
  }
  virtual void SetSinks(std::vector<Sink *> sinks) { mSinks = sinks; }

  virtual void CreateHeader(){};

  virtual void SetNumGas(int i) { mNumGas = i; }
  virtual void SetNumSinks(int i) { mNumSink = i; }
  virtual void SetNumTot(int i) { mNumTot = i; }
  virtual void SetTime(double t) { mTime = t; }

protected:
  virtual bool Read(){};
  virtual bool Write(std::string fileName, bool formatted){};

  virtual void AllocateMemory(){};

  virtual bool ReadHeaderForm(){};
  virtual void ReadParticleForm(){};
  virtual void ReadSinkForm(){};

  virtual bool ReadHeaderUnform(){};
  virtual void ReadParticleUnform(){};
  virtual void ReadSinkUnform(){};

  virtual void WriteHeaderForm(Formatter formatStream){};
  virtual void WriteParticleForm(Formatter formatStream){};
  virtual void WriteSinkForm(Formatter formatStream){};

  virtual void WriteHeaderUnform(){};
  virtual void WriteParticleUnform(){};
  virtual void WriteSinkUnform(){};

  BinaryReader *mBR;
  BinaryWriter *mBW;

  bool mFormatted = true;

  std::vector<Particle *> mParticles;
  std::vector<Sink *> mSinks;

  int mNumGas = 0;
  int mNumDust = 0;
  int mNumSink = 0;
  int mNumTot = 0;

  double mTime = 0.0;
};
