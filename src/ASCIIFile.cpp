//===-- ASCIIFile.cpp -----------------------------------------------------===//
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
/// ASCIIFile.cpp
///
//===----------------------------------------------------------------------===//

#include "ASCIIFile.h"

ASCIIFile::ASCIIFile(NameData nd) { mNameData = nd; }

ASCIIFile::~ASCIIFile() {}

bool ASCIIFile::Read() {
  mInStream.open(mNameData.name);
  if (!mInStream.is_open()) {
    std::cout << "   Could not open ASCII file " << mNameData.name
              << "   for reading!\n\n";
    return false;
  }

  ReadParticleForm();
  SetSmoothingLength();

  mInStream.close();
}

void ASCIIFile::ReadParticleForm() {
  FLOAT temp[9] = {};

  std::string line;
  while (getline(mInStream, line)) {
    std::istringstream istr(line);

    if (istr >> temp[0] >> temp[1] >> temp[2] >> temp[3] >> temp[4] >>
        temp[5] >> temp[6] >> temp[7] >> temp[8]) {
      Particle *p = new Particle();
      Vec3 pos = {temp[0], temp[1], temp[2]};
      p->SetX(pos / AU_TO_CM);
      Vec3 vel = {temp[3], temp[4], temp[5]};
      p->SetV(vel / KMPERS_TO_MPERS);
      p->SetM(temp[6] / MSUN_TO_G);
      p->SetD(temp[7]);
      p->SetU(temp[8] * ERGPERG_TO_JPERKG);
      mParticles.push_back(p);
    }
  }
  mNumTot = mNumGas = mParticles.size();
}

void ASCIIFile::SetSmoothingLength() {
  for (int i = 0; i < mNumGas; ++i) {
    FLOAT m = mParticles[i]->GetM();
    FLOAT rho = mParticles[i]->GetD() * MSOLPERAU2_TO_GPERCM2;
    FLOAT h = pow((3 * 50 * m) / (32.0 * PI * rho), (1.0 / 3.0));
    mParticles[i]->SetH(h);
  }
}