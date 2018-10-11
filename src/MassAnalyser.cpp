//===-- MassAnalyser.cpp --------------------------------------------------===//
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
/// MassAnalyser.cpp
///
//===----------------------------------------------------------------------===//

#include "MassAnalyser.h"

MassAnalyser::MassAnalyser(FLOAT encMassRad) : mEncMassRad(encMassRad) {}

MassAnalyser::~MassAnalyser() {}

void MassAnalyser::ExtractValues(SnapshotFile *file) {
  std::vector<Particle *> part = file->GetParticles();
  std::vector<Sink *> sinks = file->GetSinks();
  MassComponent mc = {};
  mc.time = file->GetTime();

  for (int i = 0; i < part.size(); ++i) {
    Particle *p = part[i];
    if (p->GetType() == GAS_TYPE) {
      mc.gas_mass += p->GetM();
      mc.gas_num++;
    } else if (p->GetType() == DUST_TYPE) {
      mc.dust_mass += p->GetM();
      mc.dust_num++;
    }
    mc.tot_mass += p->GetM();
  }

  // Sort by radius
  std::sort(part.begin(), part.end(),
            [](Particle *a, Particle *b) { return b->GetR() > a->GetR(); });

  // Find radius encompassing [90%, 95%, 99%] of the gas mass.
  for (int i = 0; i < 3; ++i) {
    FLOAT accum_mass = 0.0f;
    FLOAT threshold = mc.gas_mass * rout_percs[i];
    for (int j = 0; j < part.size(); ++j) {
      Particle *p = part[j];
      accum_mass += p->GetM();
      if (accum_mass >= threshold) {
        mc.rout[i] = p->GetR();
        break;
      }
    }
  }

  // Find maximum density position of that of a formed companion if it exists.
  if (sinks.size() == 1) {
    FLOAT max_dens = 0.0f;
    for (int i = 0; i < part.size(); ++i) {
      Particle *p = part[i];
      if (p->GetD() > max_dens) {
        max_dens = p->GetD();
        mc.rdens = p->GetR();
      }
    }
  } else {
    mc.rdens = sinks[1]->GetR();
  }

  for (int i = 0; i < sinks.size(); ++i) {
    mc.tot_mass += sinks[i]->GetM();
    mc.sink_mass += sinks[i]->GetM();
    mc.unique_sink_mass[i] = sinks[i]->GetM();
    mc.sink_num++;
  }

  mMasses.push_back(mc);

  for (int i = 0; i < 16; ++i)
    std::cout << "-----";
  std::cout << "\n";
  std::cout << "   File : " << file->GetFileName() << "\n";
  for (int i = 0; i < 16; ++i)
    std::cout << "-----";
  std::cout << "\n";

  std::cout << "   Time             : " << file->GetTime() << " yr\n";
  std::cout << "   Total mass       : " << mc.tot_mass << " msun\n";
  std::cout << "   Gas mas          : " << mc.gas_mass << " msun\n";
  std::cout << "   Stellar mass     : " << mc.unique_sink_mass[0] << " msun\n";
  std::cout << "   Companion mass   : " << mc.sink_mass - mc.unique_sink_mass[0]
            << " msun\n";
  std::cout << "   Disc/star mass   : " << mc.gas_mass / mc.unique_sink_mass[0]
            << "\n";
  std::cout << "   Outer radius     : " << mc.rout[0] << ", " << mc.rout[1]
            << ", " << mc.rout[2] << " AU\n";
  std::cout << "   Radius clump     : " << mc.rdens << " AU\n";
}

void MassAnalyser::CalculateAccretionRate() {
  std::sort(mMasses.begin(), mMasses.end(),
            [](MassComponent a, MassComponent b) { return b.time > a.time; });

  for (int i = 1; i < mMasses.size(); ++i) {
    FLOAT dt = mMasses[i - 1].time - mMasses[i].time;
    FLOAT dM = mMasses[i - 1].tot_mass - mMasses[i].tot_mass;
    if (dt != 0.0)
      mMasses[i].mdot = dM / dt;
  }
}

bool MassAnalyser::Write() {
  std::string outputName = "./SPARGEL_masses.dat";
  mOutStream.open(outputName, std::ios::out);
  if (!mOutStream.is_open()) {
    std::cout << "   Could not open file " << outputName << " for writing!\n";
    return false;
  }

  for (int i = 0; i < mMasses.size(); ++i) {
    MassComponent mc = mMasses[i];
    mOutStream << mc.time << "\t" << mc.tot_mass << "\t" << mc.gas_mass << "\t"
               << mc.mdot << "\t" << mc.rout[0] << "\t" << mc.rout[1] << "\t"
               << mc.rout[2] << "\t" << mc.rdens << "\t" << mc.dust_mass << "\t"
               << mc.sink_mass << "\t" << mc.gas_num << "\t" << mc.dust_num
               << "\t" << mc.sink_num << "\t";
    for (int j = 0; j < 16; ++j) {
      mOutStream << mc.unique_sink_mass[j] << "\t";
    }
    mOutStream << "\n";
  }
  mOutStream.close();

  std::cout << "   File output      : " << outputName << "\n";
  return true;
}