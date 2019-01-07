//===-- Application.cpp ---------------------------------------------------===//
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
/// Application.cpp
///
//===----------------------------------------------------------------------===//

#include "Application.h"

Application::Application(Arguments *args) : mArgs(args) {}

Application::~Application() {
  mFiles.clear();
  mSinkFiles.clear();
  delete mParams;
  delete mOpacity;
  delete mFNE;
  if (mGenerator != NULL)
    delete mGenerator;
  if (mDiscAnalyser != NULL)
    delete mDiscAnalyser;
  if (mEvolAnalyser != NULL)
    delete mEvolAnalyser;
  if (mCloudAnalyser != NULL)
    delete mCloudAnalyser;
  if (mSinkAnalyser != NULL)
    delete mSinkAnalyser;
  if (mMassAnalyser != NULL)
    delete mMassAnalyser;
}

void Application::StartSplash() {
  std::cout << "\n";
  for (int i = 0; i < 16; ++i)
    std::cout << "=====";
  std::cout << "\n\n";
  std::cout << "   SParGeL\n\n";
  std::cout << "   Smoothed Particle Generator and Loader\n\n";
  for (int i = 0; i < 16; ++i)
    std::cout << "=====";
  std::cout << "\n\n";
}

void Application::EndSplash() {
  for (int i = 0; i < 16; ++i)
    std::cout << "=====";
  std::cout << "\n";
}

bool Application::Initialise() {
  if (mArgs->GetNumArgs() < 2) {
    std::cout << "A parameter file must be specified, exiting...\n";
    return false;
  }

  mParams = new Parameters();
  mParams->Read(mArgs->GetArgument(0));

  mNumThreads = mParams->GetInt("THREADS");
  mMaxThreads = std::thread::hardware_concurrency();
  if (mMaxThreads <= 0) {
    std::cout << "Number of threads not detected, exiting...\n";
    return false;
  }
  if (mNumThreads < 0 || mNumThreads > mMaxThreads)
    mNumThreads = mMaxThreads;

  mOutputInfo = mParams->GetInt("OUTPUT_INFO");

  mConvert = mParams->GetInt("CONVERT");
  mInFormat = mParams->GetString("IN_FORMAT");
  mOutFormat = mParams->GetString("OUT_FORMAT");
  mOutput = mParams->GetInt("OUTPUT_FILES");
  mReduceParticles = mParams->GetInt("REDUCE_PARTICLES");
  mExtraData = std::min(EXTRA_DATA, mParams->GetInt("EXTRA_DATA"));
  mCoolingMethod = mParams->GetString("COOLING_METHOD");
  mGamma = mParams->GetFloat("GAMMA");
  mMuBar = mParams->GetFloat("MU_BAR");
  mEosFilePath = mParams->GetString("EOS_TABLE");
  mMassAnalyse = mParams->GetInt("MASS_ANALYSIS");
  mCloudAnalyse = mParams->GetInt("CLOUD_ANALYSIS");
  mCloudCenter = mParams->GetInt("CLOUD_CENTER");
  mDiscAnalyse = mParams->GetInt("DISC_ANALYSIS");
  mEvolAnalyse = mParams->GetInt("EVOLUTION_ANALYSIS");
  mSinkAnalyse = mParams->GetInt("SINK_ANALYSIS");
  mNbodyOutput = mParams->GetInt("NBODY_OUTPUT");
  mRadialAnalyse = mParams->GetInt("RADIAL_ANALYSIS");
  mCenter = mParams->GetInt("DISC_CENTER");
  mPosCenter =
      Vec3(mParams->GetFloat("CENTER_X"), mParams->GetFloat("CENTER_Y"),
           mParams->GetFloat("CENTER_Z"));
  mCenterDensest = mParams->GetInt("CENTER_DENSEST");
  mHillRadiusCut = mParams->GetInt("HILLRADIUS_CUT");
  mMidplaneCut = mParams->GetFloat("MIDPLANE_CUT");
  mExtraQuantities = mParams->GetInt("EXTRA_QUANTITIES");
  mResetTime = mParams->GetInt("RESET_TIME");
  mInsertPlanet = mParams->GetInt("INSERT_PLANET");

  mOpacity =
      new OpacityTable(mEosFilePath, true, mParams->GetFloat("OPACITY_MOD"));
  if (!mOpacity->Read())
    return false;

  // Generation of initial conditions
  if (mParams->GetInt("GENERATE")) {
    mGenerator = new Generator(mParams, mOpacity);
    mGenerator->Create();
    NameData nd;
    nd.dir = "./";
    nd.id = mParams->GetString("OUTPUT_ID");
    nd.format = mOutFormat;
    nd.snap = "00000";

    SerenFile *gen = new SerenFile(nd, false, mExtraData);
    gen->SetParticles(mGenerator->GetParticles());
    gen->SetSinks(mGenerator->GetSinks());
    OutputFile(gen);
    mFiles.push_back(gen);
  }

  // Cooling map creator
  if (mParams->GetInt("COOLING_MAP")) {
    mCoolingMap = new CoolingMap(mOpacity, mParams);
    mCoolingMap->Output();
  }

  if (mDiscAnalyse) {
    mDiscAnalyser = new DiscAnalyser(mParams);
  }

  if (mSinkAnalyse) {
    mSinkAnalyser = new SinkAnalyser();
  }

  // Create files
  for (int i = 1; i < mArgs->GetNumArgs() - 1; ++i) {
    std::string curArg = mArgs->GetArgument(i);
    mFNE = new FileNameExtractor(curArg);
    NameData nd = mFNE->GetNameData();

    if (mInFormat == "su") {
      mFiles.push_back(new SerenFile(nd, false, mExtraData));
    } else if (mInFormat == "sf") {
      mFiles.push_back(new SerenFile(nd, true, mExtraData));
    } else if (mInFormat == "du") {
      mFiles.push_back(new DragonFile(nd, false, mExtraData));
    } else if (mInFormat == "df") {
      mFiles.push_back(new DragonFile(nd, true, mExtraData));
    } else if (mInFormat == "column") {
      mFiles.push_back(new ColumnFile(nd));
    } else if (mInFormat == "ascii") {
      mFiles.push_back(new ASCIIFile(nd));
    } else if (mInFormat == "sink") {
      mSinkFiles.push_back(new SinkFile(nd));
    } else {
      std::cout << "Unrecognised input file format, exiting...\n";
      return false;
    }
  }

  if (mCloudAnalyse && mFiles.size() > 0) {
    mCloudAnalyser = new CloudAnalyser(mFiles[0]->GetNameData(),
                                       mParams->GetInt("CLOUD_AVERAGE"));
  }

  if (mEvolAnalyse && mFiles.size() > 0) {
    mEvolAnalyser = new EvolutionAnalyser(mFiles[0]->GetNameData());
  }

  if (mMassAnalyse && mFiles.size() > 0) {
    mMassAnalyser = new MassAnalyser();
  }

  return true;
}

void Application::Run() {
  // Set up file batches
  if (mFiles.size() > 0) {
    if (mFiles.size() < mNumThreads) {
      mNumThreads = mFiles.size();
    }
    mFilesPerThread = mFiles.size() / (mNumThreads);
    mRemainder = mFiles.size() % (mNumThreads);
    std::thread threads[mNumThreads];

    std::cout << "   Threads          : " << mNumThreads << "\n";
    std::cout << "   Files            : " << mFiles.size() << "\n";
    std::cout << "   Files per thread : " << mFilesPerThread << "\n";
    std::cout << "   Remainder        : " << mRemainder << "\n\n";

    std::cout << "   EOS table        : " << mOpacity->GetFileName() << "\n\n";

    int pos = 0;
    for (int i = 0; i < mNumThreads; ++i) {
      int start = pos;
      int end = pos + mFilesPerThread + ((mRemainder > 0) ? 1 : 0);
      pos = end;
      --mRemainder;

      threads[i] = std::thread(&Application::Analyse, this, i, start, end);
    }
    // Join the threads
    for (int i = 0; i < mNumThreads; ++i) {
      threads[i].join();
    }
  }

  // Sink file analysis.
  if (mSinkAnalyse) {
    for (int i = 0; i < mSinkFiles.size(); ++i) {
      mSinkFiles[i]->Read();
      mSinkAnalyser->AddMassRadius(mSinkFiles[i]);
      mSinkAnalyser->CalculateAccRate(mSinkFiles[i]);
      mSinkAnalyser->WriteMassAccretion();
      if (mNbodyOutput) {
        mSinkAnalyser->AddNbody(mSinkFiles[i]);
      }
      ++mFilesAnalysed;
    }
  }

  if (mMassAnalyse) {
    mMassAnalyser->CalculateAccretionRate();
    mMassAnalyser->Write();
  }
  if (mCloudAnalyse) {
    mCloudAnalyser->Write();
  }
  if (mSinkAnalyse) {
    mSinkAnalyser->WriteMassRadius();
    if (mNbodyOutput) {
      mSinkAnalyser->WriteNbody();
    }
  }
  if (mEvolAnalyse) {
    mEvolAnalyser->Write();
  }

  std::cout << "   Files analysed   : " << mFilesAnalysed << "\n\n";
}

void Application::Analyse(int task, int start, int end) {
  for (int i = start; i < end; ++i) {
    // File read
    if (mGenerator == NULL) {
      if (!mFiles[i]->Read())
        break;
    }

    // Thermal property calculation. Independant between particles/sinks. Can be
    // done before all other analysis.
    FindThermo((SnapshotFile *)mFiles[i]);

    // Cloud analysis
    if (mCloudAnalyse) {
      mCloudAnalyser->FindCentralQuantities((SnapshotFile *)mFiles[i]);
      if (mCloudCenter) {
        mCloudAnalyser->CenterAroundDensest((SnapshotFile *)mFiles[i]);
      }
    }
    // Disc analysis
    if (mDiscAnalyse) {
      if (mCenter) {
        mDiscAnalyser->Center((SnapshotFile *)mFiles[i], mCenter - 1,
                              mPosCenter, mCenterDensest);
        mDiscAnalyser->FindOuterRadius((SnapshotFile *) mFiles[i]);
      }
      // Find vertically integrated quantities
      if (mHillRadiusCut) {
        HillRadiusCut((SnapshotFile *)mFiles[i]);
      }
      if (mExtraQuantities) {
        FindOpticalDepth((SnapshotFile *)mFiles[i]);
        FindBeta((SnapshotFile *)mFiles[i]);
      }
      if (mMidplaneCut) {
        MidplaneCut((SnapshotFile *)mFiles[i]);
      }
      if (mEvolAnalyse) {
        mEvolAnalyser->Append((SnapshotFile *)mFiles[i]);
      }
      if (mSinkAnalyse) {
        mSinkAnalyser->CalculateMassRadius((SnapshotFile *)mFiles[i], 1);
      }
    }

    // Planet insertion
    if (mInsertPlanet) {
      InsertPlanet((SnapshotFile *)mFiles[i]);
    }

    // Necessary Toomre quantity calculations. Needs to be done after moving the
    // disc (e.g. centering).
    FindToomre((SnapshotFile *)mFiles[i]);

    // Particle reduction.
    // TODO: Double-free when deleting snapshot file.
    if (mReduceParticles) {
      ReduceParticles((SnapshotFile *)mFiles[i]);
    }

    // Mass analysis
    if (mMassAnalyse) {
      mMassAnalyser->ExtractValues((SnapshotFile *)mFiles[i]);
    }

    // Radial analysis
    if (mRadialAnalyse) {
      RadialAnalyser *ra = new RadialAnalyser(mParams);
      ra->Run((SnapshotFile *)mFiles[i]);
      delete ra;
    }
    // File conversion
    if (mConvert) {
      mFiles[i]->SetNameDataFormat(mOutFormat);
    }
    // Snapshot output
    if (mOutput) {
      OutputFile((SnapshotFile *)mFiles[i]);
    }
    // Screen output
    if (mOutputInfo) {
      OutputInfo((SnapshotFile *)mFiles[i]);
    }
    ++mFilesAnalysed;
    delete mFiles[i];
  }
}

void Application::MidplaneCut(SnapshotFile *file) {
  std::vector<Particle *> part = file->GetParticles();
  std::vector<Particle *> trimmed;
  for (int i = 0; i < part.size(); ++i) {
    Particle *p = part[i];
    float R = p->GetX().Norm();
    float z = abs(p->GetX().z);
    if (z <= mMidplaneCut) {
      trimmed.push_back(p);
    } else {
      delete part[i];
    }
  }
  file->SetParticles(trimmed);
  file->SetNumGas(trimmed.size());
  file->SetNameDataAppend(".midplane");
  trimmed.clear();
}

void Application::HillRadiusCut(SnapshotFile *file) {
  std::vector<Sink *> sinks = file->GetSinks();
  if (sinks.size() < 2)
    return;

  std::vector<Particle *> part = file->GetParticles();
  std::vector<Particle *> trimmed;

  float planet_radius = sinks[1]->GetX().Norm();
  float hill_radius =
      planet_radius * pow(sinks[1]->GetM() / sinks[0]->GetM(), 0.333);

  std::cout << "Hill radius: " << hill_radius << " AU\n";

  // Center around the planet
  for (int i = 0; i < part.size(); ++i) {
    Vec3 new_pos = part[i]->GetX() - sinks[1]->GetX();
    part[i]->SetX(new_pos);
  }

  // Trim those particles within a Hill radius
  for (int i = 0; i < part.size(); ++i) {
    if (part[i]->GetX().Norm() > mHillRadiusCut * hill_radius) {
      trimmed.push_back(part[i]);
    } else {
      delete part[i];
    }
  }
  std::cout << "Trimmed: " << part.size() - trimmed.size() << " particles\n";

  // Re-center around previous position
  for (int i = 0; i < trimmed.size(); ++i) {
    Vec3 new_pos = trimmed[i]->GetX() + sinks[1]->GetX();
    trimmed[i]->SetX(new_pos);
  }

  file->SetParticles(trimmed);
  file->SetNumGas(trimmed.size());
  file->SetNameDataAppend(".hillradius");
  trimmed.clear();
}

void Application::OutputFile(SnapshotFile *file) {
  if (!mGenerator)
    file->SetNameDataAppend(".modified");
  NameData nd = file->GetNameData();
  std::string outputName;

  if (nd.dir == "")
    nd.dir = ".";

  outputName =
      nd.dir + "/" + nd.id + "." + nd.format + "." + nd.snap + nd.append;

  if (mResetTime) {
    file->SetTime(0.0f);
  }

  // TODO: reduce code duplication and clean up new created files.
  if (nd.format == "df") {
    DragonFile *df = new DragonFile(nd, true, mExtraData);
    df->SetParticles(file->GetParticles());
    df->SetSinks(file->GetSinks());
    df->SetNumGas(file->GetNumGas());
    df->SetNumSinks(file->GetNumSinks());
    df->SetNumTot(file->GetNumPart());
    df->SetTime(file->GetTime());
    df->Write(outputName, true);
  }

  if (nd.format == "su") {
    SerenFile *su = new SerenFile(nd, false, mExtraData);
    su->SetParticles(file->GetParticles());
    su->SetSinks(file->GetSinks());
    su->SetNumGas(file->GetNumGas());
    su->SetNumSinks(file->GetNumSinks());
    su->SetNumTot(file->GetNumPart());
    su->SetTime(file->GetTime());
    su->Write(outputName, false);
  }

  if (nd.format == "column") {
    ColumnFile *cf = new ColumnFile(nd);
    cf->SetParticles(file->GetParticles());
    cf->SetSinks(file->GetSinks());
    cf->SetNumGas(file->GetParticles().size());
    cf->SetNumSinks(file->GetSinks().size());
    cf->SetNumTot(file->GetParticles().size() + file->GetSinks().size());
    cf->SetTime(file->GetTime());
    cf->Write(outputName);
  }
}

void Application::FindThermo(SnapshotFile *file) {
  std::vector<Particle *> part = file->GetParticles();
  for (int i = 0; i < part.size(); ++i) {
    Particle *p = part[i];
    float density = p->GetD();
    float energy = p->GetU();
    float sigma = p->GetSigma();
    float temp = p->GetT();
    if (mCoolingMethod == "stamatellos" || mCoolingMethod == "lombardi") {
      if (mInFormat == "su" || mInFormat == "sf" || mInFormat == "column" ||
          mInFormat == "ascii") {
        temp = mOpacity->GetTemp(density, energy);
      } else if (mInFormat == "df" || mInFormat == "du") {
        energy = mOpacity->GetEnergy(density, temp);
      }
    } else if (mCoolingMethod == "beta_cooling") {
      if (mInFormat == "su" || mInFormat == "sf" || mInFormat == "column") {
        temp = (energy * mMuBar * M_P * (mGamma - 1.0)) / K;
      } else if (mInFormat == "df" || mInFormat == "du") {
        energy = (K * temp) / (mMuBar * M_P * (mGamma - 1.0));
      }
    }
    float gamma = mOpacity->GetGamma(density, temp);
    float kappa = mOpacity->GetKappa(density, temp);
    float kappar = mOpacity->GetKappar(density, temp);
    float mu_bar = mOpacity->GetMuBar(density, temp);
    float press = (gamma - 1.0) * density * energy;
    float cs = sqrt((K * temp) / (M_P * mu_bar));
    float tau = kappa * sigma;
    float dudt = 1.0 / ((sigma * sigma * kappa) + (1 / kappar));

    part[i]->SetT(temp);
    part[i]->SetU(energy);
    part[i]->SetP(press);
    part[i]->SetCS(cs);
    part[i]->SetKappa(kappar);
    part[i]->SetTau(tau);
    part[i]->SetDUDT(dudt);
  }
  file->SetParticles(part);
}

void Application::FindOpticalDepth(SnapshotFile *file) {
  std::vector<Particle *> part = file->GetParticles();
  std::vector<Particle *> trimmed;

  OpticalDepthOctree *octree =
      new OpticalDepthOctree(Vec3(0.0, 0.0, 0.0), Vec3(2048.0, 2048.0, 2048.0));

  // Trim particles outside of octree
  // TODO: Move the disc to positive space?
  std::sort(part.begin(), part.end(),
            [](Particle *a, Particle *b) { return b->GetR() > a->GetR(); });

  for (int i = 0; i < part.size(); ++i) {
    if (part[i]->GetR() > 1024.0) {
      continue;
    }

    Particle *p = part[i];
    trimmed.push_back(p);
  }
  part = trimmed;

  // Sort by z descending
  std::sort(part.begin(), part.end(),
            [](Particle *a, Particle *b) { return b->GetX().x < a->GetX().x; });

  std::vector<Particle *> positive, negative;
  for (int i = 0; i < part.size(); ++i) {
    if (part[i]->GetX().z >= 0.0) {
      positive.push_back(part[i]);
    } else {
      negative.push_back(part[i]);
    }
  }

  // Construct, link and walk twice. First for particles with z > 0. Then take
  // particles with z < 0 and reflect about the z-axis using absolute z values.
  OpticalDepthPoint *positive_points = new OpticalDepthPoint[positive.size()];
  octree->Construct(positive, positive_points);
  octree->Walk(positive, mOpacity);
  for (int i = 0; i < positive.size(); ++i)
    part[i] = positive[i];

  for (int i = 0; i < negative.size(); ++i) {
    Particle *p = negative[i];
    float x = p->GetX().x;
    float y = p->GetX().y;
    float z = -(p->GetX().z);
    negative[i]->SetX(Vec3(x, y, z));
  }
  OpticalDepthPoint *negative_points = new OpticalDepthPoint[negative.size()];
  octree->Construct(negative, negative_points);
  octree->Walk(negative, mOpacity);
  // The particles below the disc which have been flipped above now require
  // being flipped back.
  for (int i = 0; i < negative.size(); ++i) {
    float x = negative[i]->GetX().x;
    float y = negative[i]->GetX().y;
    float z = -(negative[i]->GetX().z);
    negative[i]->SetX(Vec3(x, y, z));
    part[i + positive.size()] = negative[i];
  }
  positive.clear();
  negative.clear();

  for (int i = 0; i < part.size(); ++i) {
    Particle *p = part[i];
    float sigma = p->GetSigma();
    float tau = p->GetTau();
    float dudt = 1.0 / (sigma * (tau + (1.0 / tau)));

    part[i]->SetDUDT(dudt);
  }

  delete octree;
  delete[] positive_points;
  delete[] negative_points;
}

void Application::FindToomre(SnapshotFile *file) {
  std::vector<Particle *> part = file->GetParticles();
  std::vector<Sink *> sink = file->GetSinks();
  std::sort(part.begin(), part.end(), [](Particle *a, Particle *b) {
    return b->GetX().Norm() > a->GetX().Norm();
  });
  float inner_mass = 0.0;
  int sink_index = 0;

  // Add mass contribution from central star if it exists
  if (sink.size() > 0) {
    inner_mass += sink[0]->GetM();
    sink_index = 1;
  }

  for (int i = 0; i < part.size(); ++i) {
    Particle *p = part[i];
    float r = p->GetX().Norm();
    float r3 = pow(r * AU_TO_M, 3.0);
    float omega = sqrt((G * inner_mass * MSUN_TO_KG) / (r3));

    float cs = p->GetCS();
    float sigma = p->GetSigma() * GPERCM2_TO_KGPERM2;
    float Q = (cs * omega) / (PI * G * sigma);

    part[i]->SetOmega(omega);
    part[i]->SetQ(Q);

    // Add contribution from other sinks but only once when
    // we have exceeded it's radius
    for (int i = sink_index; i < sink.size(); ++i) {
      if (r > sink[i]->GetX().Norm()) {
        inner_mass += sink[i]->GetM();
        sink_index++;
      }
    }
    inner_mass += p->GetM();
  }
  file->SetParticles(part);
}

void Application::FindBeta(SnapshotFile *file) {
  std::vector<Particle *> part = file->GetParticles();
  for (int i = 0; i < part.size(); ++i) {
    float r = part[i]->GetX().Norm();
    float omega = part[i]->GetOmega();
    float u = part[i]->GetU() / ERGPERG_TO_JPERKG;
    float dens = part[i]->GetD();
    float temp = part[i]->GetT();
    float u_bgr = mOpacity->GetEnergy(dens, 10.0) / ERGPERG_TO_JPERKG;

    float dudt_norm = part[i]->GetDUDT();
    float dudt = dudt_norm * 4.0 * SB * pow(temp, 4.0);
    float beta = u * (omega / dudt);

    part[i]->SetBeta(beta);
  }
  file->SetParticles(part);
}

void Application::InsertPlanet(SnapshotFile *file) {
  std::vector<Particle *> part = file->GetParticles();
  std::vector<Sink *> sink = file->GetSinks();

  float mass = mParams->GetFloat("PLANET_MASS") / MSUN_TO_MJUP;
  float radius = mParams->GetFloat("PLANET_RADIUS");
  float smoothing = mParams->GetFloat("PLANET_SMOOTHING");
  float ecc = mParams->GetFloat("PLANET_ECC");
  float inc = mParams->GetFloat("PLANET_INC");

  Vec3 vel = Vec3(0.0, 0.0, 0.0);

  float hill_radius = radius * pow(mass / (3.0 * sink[0]->GetM()), 1.0 / 3.0);
  float interior_mass = 0.0;
  for (int i = 0; i < part.size(); ++i) {
    if (part[i]->GetX().Norm() < radius) {
      interior_mass += part[i]->GetM();
    }
  }
  vel[1] = sqrt((G * (sink[0]->GetM() + interior_mass) * MSUN_TO_KG) /
                (radius * AU_TO_M)) /
           KMPERS_TO_MPERS;

  // TODO: Set velocity based on ecc and inc.

  Sink *planet = new Sink();
  planet->SetX(Vec3(radius, 0.0, 0.0));
  planet->SetV(vel);
  planet->SetM(mass);
  planet->SetH(smoothing);
  planet->SetType(-1);
  sink.push_back(planet);

  file->SetNameDataAppend(".planet");
  file->SetSinks(sink);
}

void Application::ReduceParticles(SnapshotFile *file) {
  std::vector<Particle *> part = file->GetParticles();
  int curr_num = part.size();
  int final_num = std::min(mReduceParticles, curr_num);
  if (final_num >= curr_num) {
    return;
  }

  float new_mass = part[0]->GetM() * (curr_num / final_num);

  std::vector<Particle *> new_part;
  for (int i = 0; i < final_num; ++i) {
    int r = rand() % curr_num;
    new_part.push_back(part[r]);
  }

  for (int i = 0; i < final_num; ++i) {
    Particle *p = new_part[i];
    float h = pow((3 * 50 * new_mass) / (32 * PI * p->GetD()), (1.0f / 3.0f));
    new_part[i]->SetM(new_mass);
    new_part[i]->SetH(h);
  }
  file->SetNumGas(new_part.size());
  file->SetNameDataAppend(".reduced");
  file->SetParticles(new_part);
}

void Application::OutputInfo(SnapshotFile *file) {
  std::vector<Particle *> part = file->GetParticles();
  std::vector<Sink *> sink = file->GetSinks();

  for (int i = 0; i < 16; ++i)
    std::cout << "=====";
  std::cout << "\n   " << file->GetFileName() << "\n";
  for (int i = 0; i < 16; ++i)
    std::cout << "=====";
  std::cout << "\n";

  float gas_mass = 0.0, total_mass = 0.0;
  for (int i = 0; i < sink.size(); ++i) {
    std::cout << "   Sink " << i + 1 << "\n";
    std::cout << "   Mass   = " << sink[i]->GetM() << "\n";
    std::cout << "   Radius = " << sink[i]->GetX().Norm() << "\n";
    std::cout << "   Accretion radius = " << sink[i]->GetH() << "\n";
    for (int i = 0; i < 16; ++i)
      std::cout << "-----";
    std::cout << "\n";

    total_mass += sink[i]->GetM();
  }
  for (int i = 0; i < part.size(); ++i) {
    gas_mass += part[i]->GetM();
  }
  std::cout << "   Gas mass   : " << gas_mass << "\n";
  std::cout << "   Total mass : " << gas_mass + total_mass << "\n";
}