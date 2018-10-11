//===-- SinkAnalyser.cpp --------------------------------------------------===//
//
//                                  SPARGEL
//                   Smoothed SinkAnalyser Generator and Loader
//
// This file is distributed under the GNU General Public License. See LICENSE
// for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// SinkAnalyser.cpp
///
//===----------------------------------------------------------------------===//

#include "SinkAnalyser.h"

SinkAnalyser::SinkAnalyser() {}

SinkAnalyser::~SinkAnalyser() {
  // for (int i = 0; i < mNbodyRecords.size(); ++i) {
  //   delete mNbodyRecords[0];
  // }
  // mNbodyRecords.clear();
}

void SinkAnalyser::AddMassRadius(SinkFile *sf) {
  SinkRecord *f = sf->GetRecords().front();
  SinkRecord *b = sf->GetRecords().back();
  mRecords.push_back(*f);
  mRecords.push_back(*b);
}

void SinkAnalyser::CalculateAccRate(SinkFile *sf) {
  std::vector<SinkRecord *> records = sf->GetRecords();
  FLOAT dt = records.back()->time - records.front()->time;

  // Average dmdt for first 10 percent of the simulation.
  for (int i = 0; i < records.size(); ++i) {
    SinkRecord *r = records[i];
    FLOAT t = r->time;

    if (t > dt * 0.1) {
      break;
    }
    avg_num[0]++;
    avg_dmdt[0] += r->dmdt;
  }

  // Average dmdt for last 10 percent of the simulation.
  for (int i = 0; i < records.size(); ++i) {
    SinkRecord *r = records[i];
    FLOAT t = r->time;

    if (t > dt * 0.9) {
      avg_num[1]++;
      avg_dmdt[1] += r->dmdt;
    }
  }

  // Average dmdt across the whole simulation.
  for (int i = 0; i < records.size(); ++i) {
    SinkRecord *r = records[i];
    avg_num[2]++;
    avg_dmdt[2] += r->dmdt;
  }

  // Calculate the averages.
  for (int i = 0; i < 3; ++i) {
    avg_dmdt[i] /= avg_num[i];
  }

  for (int i = 0; i < 16; ++i)
    std::cout << "-----";
  std::cout << "\n";
  std::cout << "   File : " << sf->GetFileName() << "\n";
  for (int i = 0; i < 16; ++i)
    std::cout << "-----";
  std::cout << "\n";

  // Output the values.
  std::cout << "   Avg.mdot (< 0.1) : " << avg_dmdt[0] << " Msun/yr\n";
  std::cout << "   Avg.mdot (> 0.9) : " << avg_dmdt[1] << " Msun/yr\n";
  std::cout << "   Avg.mdot         : " << avg_dmdt[2] << " Msun/yr\n";
}

void SinkAnalyser::AddNbody(SinkFile *sf) {
  SinkRecord *r = sf->GetRecords().back();
  mNbodyRecords.push_back(*r);
}

bool SinkAnalyser::WriteMassRadius() {
  std::ofstream outStream;
  outStream.open("mass_radius.dat", std::ios::out);
  if (!outStream.is_open()) {
    std::cout << "   Could not open MASS-RADIUS file "
              << "NAME"
              << " for writing!\n\n";
    return false;
  }

  // TODO: Better way of organising data!
  // Combine mRecords and mNbodyRecords?
  for (int i = 0; i < mRecords.size(); i += 2) {
    outStream << i << mRecords[i].m << "\t" << mRecords[i].pos.Norm() << "\t"
              << mRecords[i + 1].m << "\t" << mRecords[i + 1].pos.Norm()
              << "\n";
  }
  outStream.close();

  return true;
}

bool SinkAnalyser::WriteNbody() {
  std::ofstream outStream;
  outStream.open("nbody.dat", std::ios::out);
  if (!outStream.is_open()) {
    std::cout << "   Could not open NBODY file "
              << "NAME"
              << " for writing!\n\n";
    return false;
  }

  outStream << mNbodyRecords.size() << " sinks\n";
  outStream << "id/pid/step/time(yr)/mass(Msun)/x/y/z/r(AU)/vx/vy/vz(km/s)/v\n";
  for (int i = 0; i < mNbodyRecords.size(); ++i) {
    outStream << i << "\t" << i << "\t" << mNbodyRecords[i].nsteps << "\t"
              << mNbodyRecords[i].time << "\t" << mNbodyRecords[i].m << "\t"
              << mNbodyRecords[i].pos.x << "\t" << mNbodyRecords[i].pos.y
              << "\t" << mNbodyRecords[i].pos.z << "\t"
              << mNbodyRecords[i].pos.Norm() << "\t" << mNbodyRecords[i].vel.x
              << "\t" << mNbodyRecords[i].vel.y << "\t"
              << mNbodyRecords[i].vel.z << "\t" << mNbodyRecords[i].vel.Norm()
              << "\n";
  }
  outStream.close();

  return true;
}
