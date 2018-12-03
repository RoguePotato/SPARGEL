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

MassAnalyser::MassAnalyser() {}

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

  // Mass and N within 1 AU of companion. Hill radius calculation.
  FLOAT sink_mass[3] = {0.0, 0.0, 0.0};
  FLOAT sink_n[3] = {0.0, 0.0, 0.0};
  FLOAT hill_radius = 0.0;
  if (sinks.size() > 0) {
    Vec3 sink_pos = sinks[1]->GetX();
    for (int i = 0; i < part.size(); ++i) {
      FLOAT dx = (part[i]->GetX() - sink_pos).Norm();

      if (dx < 0.25) {
        sink_mass[0] += part[i]->GetM() * MSUN_TO_MJUP;
        sink_n[0]++;
      }
      if (dx < 0.5) {
        sink_mass[1] += part[i]->GetM() * MSUN_TO_MJUP;
        sink_n[1]++;
      }
      if (dx < 1.0) {
        sink_mass[2] += part[i]->GetM() * MSUN_TO_MJUP;
        sink_n[2]++;
      }
    }
    for (int i = 0; i < 3; ++i) {
      sink_n[i] /= part.size();
    }
    // Hill radius
    hill_radius =
        sinks[1]->GetR() *
        powf(sink_mass[2] / (3 * sinks[0]->GetM() * MSUN_TO_MJUP), 0.33f);
  }

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
  if (sinks.size() > 1) {
    std::cout << "   Planet mass      : "
              << (mc.sink_mass - mc.unique_sink_mass[0]) * MSUN_TO_MJUP
              << " mjup\n";
    std::cout << "   Planet mass enc. : " << sink_mass[0] << " " << sink_mass[1]
              << " " << sink_mass[2] << " mjup\n";
    std::cout << "   Planet N % enc   : " << sink_n[0] << " " << sink_n[1]
              << " " << sink_n[2] << "\n";
    std::cout << "   Planet R_hill    : " << hill_radius << " AU\n";
  }
  std::cout << "   Disc/star mass   : " << mc.gas_mass / mc.unique_sink_mass[0]
            << "\n";
  std::cout << "   Outer radius     : " << mc.rout[0] << " " << mc.rout[1]
            << " " << mc.rout[2] << " AU\n";
  std::cout << "   a clump          : " << mc.rdens << " AU\n";

  if (sinks.size() < 1) { 
    return;
  }
  
  std::cout << "   R clump          : " << sinks.at(1)->GetClumpR() << " AU\n";
  std::cout << "   M clump          : " << sinks.at(1)->GetClumpM()
            << " Mjup\n";
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