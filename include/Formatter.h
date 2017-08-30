//===-- Formatter.h -------------------------------------------------------===//
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
/// Formatter.h makes it easy to set the format of output data, mainly for
/// files.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Definitions.h"

class Formatter {
public:
  Formatter(std::ostream &outStream, int widthFloat, int widthInteger,
            int precision)
      : mOutStream(outStream), mWidthFloat(widthFloat),
        mWidthInteger(widthInteger), mPrecision(precision) {}

  Formatter &operator<<(const int &output) {
    mOutStream << std::setw(mWidthInteger) << output;
    return *this;
  }

  Formatter &operator<<(const float &output) {
    mOutStream << std::setw(mWidthFloat) << std::setprecision(mPrecision)
               << output;
    return *this;
  }

  Formatter &operator<<(const double &output) {
    mOutStream << std::setw(mWidthFloat) << std::setprecision(mPrecision)
               << output;
    return *this;
  }

  Formatter &operator<<(const bool &output) {
    char boolVal;
    if (output) {
      boolVal = 'T';
    } else {
      boolVal = 'F';
    }
    mOutStream << boolVal;
    return *this;
  }

  Formatter &operator<<(std::ostream &(*Func)(std::ostream &)) {
    Func(mOutStream);
    return *this;
  }

  template <typename T> Formatter &operator<<(const T &output) {
    mOutStream << output;
    return *this;
  }

  void SetWidthInteger(int width) { mWidthInteger = width; }

private:
  std::ostream &mOutStream;
  int mWidthFloat;
  int mWidthInteger;
  int mPrecision;
};
