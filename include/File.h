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

// #include "EosTable.h"
#include "Definitions.h"
#include "Particle.h"

struct NameData {
  std::string name = "";
  std::string snap = "";
  std::string format = "";
  std::string id = "";
  std::string dir = "";
};

class File {
public:
  File() {};
  ~File() {};

  virtual bool Read(std::string fileName, bool formatted = true) = 0;
  virtual bool Write(std::string fileName, bool formatted) = 0;

protected:
  std::string mFileName = "";

  std::ifstream mInStream;
  std::ofstream mOutStream;

  std::string TrimWhiteSpace(std::string str);
};

class SnapshotFile : public File {
public:
  // virtual void CalculateTemperatures(EosTable *eos) {};
  SnapshotFile() {};
  ~SnapshotFile() {};

  virtual NameData GetNameData() { return mNameData; }
  virtual std::string GetFileName() { return mFileName; }
  virtual std::vector<Particle *> GetParticles() { return mParticles; }
  virtual real64 GetTime() { return mTime; }
  virtual uint32 GetNumGas() { return mNumGas; }
  virtual uint32 GetNumSinks() { return mNumSink; }
  virtual uint32 GetNumPart() { return mNumTot; }
  virtual bool GetFormatted() { return mFormatted; }

  virtual void SetParticles(std::vector<Particle *> particles) {
    mParticles = particles;
  }

  virtual void SetNumGas(uint32 i) { mNumGas = i; }
  virtual void SetNumSinks(uint32 i) { mNumSink = i; }
  virtual void SetNumPart(uint32 i) { mNumTot = i; }
  virtual void SetTime (real64 t) { mTime = t; }

protected:
  NameData mNameData = {};
  bool mFormatted = true;

  virtual bool Read(std::string fileName, bool formatted = true) {};
  virtual bool Write(std::string fileName, bool formatted) {};

  std::vector<Particle *> mParticles;
  std::vector<Sink *> mSinks;

  uint32 mNumGas = 0;
  uint32 mNumSink = 0;
  uint32 mNumTot = 0;

  real64 mTime = 0.0;
};
