//===-- BinaryIO.h --------------------------------------------------------===//
//
//                                  SPARGEL
//                   Smoothed Particle BinaryIO and Loader
//
// This file is distributed under the GNU General Public License. See LICENSE
// for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// BinaryIO.h allows the reading and writing of various data types in binary
/// format.
///
//===----------------------------------------------------------------------===//

#pragma once

#include <fstream>
#include <string>

class BinaryWriter {
public:
  BinaryWriter(std::ofstream &outStream) : mOutStream(outStream) {}

  void WriteValue(const std::string &value) { mOutStream << value; }

  void WriteValue(const bool &value) {
    const int intVal = value;
    WriteValue(intVal);
  }

  template <class TypeToWrite> void WriteValue(const TypeToWrite &value) {
    mOutStream.write((char *)&value, sizeof(value));
  }

private:
  std::ofstream &mOutStream;
};

class BinaryReader {
public:
  BinaryReader(std::ifstream &inStream) : mInStream(inStream) {}

  template <class TypeToRead> void ReadValue(TypeToRead &result) {
    mInStream.read((char *)&result, sizeof(TypeToRead));
  }

private:
  std::ifstream &mInStream;
};
