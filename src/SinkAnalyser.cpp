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

void SinkAnalyser::CalculateMassRadius(SnapshotFile *file, int sink_id) {
  std::vector<Particle *> part = file->GetParticles();
  std::vector<Sink *> sink = file->GetSinks();

  if (sink_id < 0 || sink_id > sink.size()) {
    return;
  }

  // Center particles around the sink.
  for (int i = 0; i < part.size(); ++i) {
    Vec3 newX = part.at(i)->GetX() - sink.at(sink_id)->GetX();
    part.at(i)->SetX(newX);
  }

  // Sort particles by radius.
  std::sort(part.begin(), part.end(), [](Particle *a, Particle *b) {
    return b->GetX().Norm() > a->GetX().Norm();
  });

  // Find the radius of the sink at the point the density drops below 1e-13
  // g/cm^-3.
  FLOAT total_mass = sink.at(sink_id)->GetM();
  for (int i = 0; i < part.size(); ++i) {
    if (part.at(i)->GetD() < 1e-13) {
      sink.at(sink_id)->SetClumpR(part.at(i)->GetX().Norm()); 
      break;
    }
    total_mass += part.at(i)->GetM();
  }
  sink.at(sink_id)->SetClumpM(total_mass * MSUN_TO_MJUP);

  file->SetSinks(sink);
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

  AccretionRecord ar;
  ar.id = sf->GetNameData().id;
  ar.avg_begin = avg_dmdt[0];
  ar.avg_end = avg_dmdt[1];
  ar.avg_tot = avg_dmdt[2];
  avg_dmdt_records.push_back(ar);

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

  // Zero the arrays.
  for (int i = 0; i < 3; ++i) {
    avg_dmdt[i] = 0.0;
    avg_num[i] = 0;
  }
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

bool SinkAnalyser::WriteMassAccretion() {
  std::ofstream outStream;
  outStream.open("./SPARGEL_sink_accretion.dat", std::ios::out);
  if (!outStream.is_open()) {
    return false;
  }

  for (int i = 0; i < avg_dmdt_records.size(); ++i) {
    outStream << avg_dmdt_records[i].id << "\t" << avg_dmdt_records[i].avg_begin
              << "\t" << avg_dmdt_records[i].avg_end << "\t"
              << avg_dmdt_records[i].avg_tot << "\n";
  }

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
