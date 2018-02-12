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
  // if (mArgs != NULL) delete mArgs;
  // if (mParams != NULL) delete mParams;
  // if (mOpacity != NULL) delete mOpacity;
  // if (mFNE != NULL) delete mFNE;
  // delete mRA;
}

void Application::StartSplash(void) {
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

void Application::EndSplash(void) {
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

  mConvert = mParams->GetInt("CONVERT");
  mInFormat = mParams->GetString("IN_FORMAT");
  mOutFormat = mParams->GetString("OUT_FORMAT");
  mOutput = mParams->GetInt("OUTPUT_FILES");
  mEosFilePath = mParams->GetString("EOS_TABLE");
  mCloudAnalyse = mParams->GetInt("CLOUD_ANALYSIS");
  mCloudCenter = mParams->GetInt("CLOUD_CENTER");
  mDiscAnalyse = mParams->GetInt("DISC_ANALYSIS");
  mSinkAnalyse = mParams->GetInt("SINK_ANALYSIS");
  mRadialAnalyse = mParams->GetInt("RADIAL_ANALYSIS");
  mCenter = mParams->GetInt("DISC_CENTER");
  mPosCenter =
      Vec3(mParams->GetFloat("CENTER_X"), mParams->GetFloat("CENTER_Y"),
           mParams->GetFloat("CENTER_Z"));
  mHillRadiusCut = mParams->GetInt("HILLRADIUS_CUT");
  mMidplaneCut = mParams->GetFloat("MIDPLANE_CUT");

  mOpacity = new OpacityTable(mEosFilePath, true);
  if (!mOpacity->Read())
    return false;

  if (mParams->GetInt("GENERATE")) {
    mGenerator = new Generator(mParams, mOpacity);
    mGenerator->Create();
    NameData nd;
    nd.dir = "./";
    nd.id = "SPA";
    nd.format = mOutFormat;
    nd.snap = "00000";

    SerenFile *gen = new SerenFile(nd, false);
    gen->SetParticles(mGenerator->GetParticles());
    gen->SetSinks(mGenerator->GetSinks());
    OutputFile(gen);
    mFiles.push_back(gen);
  }

  if (mCloudAnalyse) {
    mCloudAnalyser = new CloudAnalyser(mParams->GetString("CLOUD_OUTPUT"));
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
      mFiles.push_back(new SerenFile(nd, false));
    } else if (mInFormat == "sf") {
      mFiles.push_back(new SerenFile(nd, true));
    } else if (mInFormat == "du") {
      mFiles.push_back(new DragonFile(nd, false));
    } else if (mInFormat == "df") {
      mFiles.push_back(new DragonFile(nd, true));
    } else if (mInFormat == "column") {
      mFiles.push_back(new ColumnFile(nd));
    } else if (mInFormat == "sink") {
      mFiles.push_back(new SinkFile(nd));
    } else {
      std::cout << "Unrecognised input file format, exiting...\n";
      return false;
    }
  }

  if (mFiles.size() < 1) {
    std::cout << "No files selected, exiting...\n";
    return false;
  }
  return true;
}

void Application::Run() {
  // Set up file batches
  if (mFiles.size() < mNumThreads)
    mNumThreads = mFiles.size();
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

  if (mCloudAnalyse)
    mCloudAnalyser->Write();
  if (mSinkAnalyse) {
    mSinkAnalyser->WriteMassRadius();
    mSinkAnalyser->WriteNbody();
  }

  std::cout << "   Files analysed   : " << mFilesAnalysed << "\n\n";
}

void Application::Analyse(int task, int start, int end) {
  for (int i = start; i < end; ++i) {
    // File read
    if (mGenerator == NULL) {
      if (!mFiles.at(i)->Read())
        break;
    }

    // Extra quantity calculation
    FindThermo((SnapshotFile *)mFiles.at(i));

    // Cloud analysis
    if (mCloudAnalyse) {
      mCloudAnalyser->FindCentralQuantities((SnapshotFile *)mFiles.at(i));
      if (mCloudCenter) {
        mCloudAnalyser->CenterAroundDensest((SnapshotFile *)mFiles.at(i));
      }
    }
    // Disc analysis
    if (mDiscAnalyse) {
      if (mCenter) {
        mDiscAnalyser->Center((SnapshotFile *)mFiles.at(i), mCenter - 1,
                              mPosCenter);
      }
      // Find vertically integrated quantities
      if (mHillRadiusCut) {
        HillRadiusCut((SnapshotFile *)mFiles.at(i));
      }
      FindOpticalDepth((SnapshotFile *)mFiles.at(i));
      FindToomre((SnapshotFile *)mFiles.at(i));
      if (mMidplaneCut) {
        MidplaneCut((SnapshotFile *)mFiles.at(i));
      }
    }

    // Sink analysis
    if (mSinkAnalyse) {
      mSinkAnalyser->AddMassRadius((SinkFile *)mFiles.at(i));
      mSinkAnalyser->AddNbody((SinkFile *)mFiles.at(i));
    }
    // Radial analysis
    if (mRadialAnalyse) {
      RadialAnalyser *ra = new RadialAnalyser(mParams);
      ra->Run((SnapshotFile *)mFiles.at(i));
      delete ra;
    }
    // File conversion
    if (mConvert) {
      mFiles.at(i)->SetNameDataFormat(mOutFormat);
    }
    // Snapshot output
    if (mOutput) {
      OutputFile((SnapshotFile *)mFiles.at(i));
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
    FLOAT R = p->GetX().Norm();
    FLOAT z = abs(p->GetX().z);
    if (z <= mMidplaneCut) {
      trimmed.push_back(p);
    }
  }
  file->SetParticles(trimmed);
  file->SetNameDataAppend(".midplane");
}

void Application::HillRadiusCut(SnapshotFile *file) {
  std::vector<Sink *> sinks = file->GetSinks();
  if (sinks.size() < 2)
    return;

  std::vector<Particle *> part = file->GetParticles();
  std::vector<Particle *> trimmed;

  FLOAT planet_radius = sinks[1]->GetX().Norm();
  FLOAT hill_radius =
      planet_radius * pow(sinks[1]->GetM() / sinks[0]->GetM(), 0.333);

  std::cout << "Hill radius: " << hill_radius << " AU\n";

  // Center around the planet
  for (int i = 0; i < part.size(); ++i) {
    Vec3 new_pos = part[i]->GetX() - sinks[1]->GetX();
    part[i]->SetX(new_pos);
  }

  // Trim those particles within 2 Hill radii
  for (int i = 0; i < part.size(); ++i) {
    if (part[i]->GetX().Norm() > mHillRadiusCut * hill_radius) {
      trimmed.push_back(part[i]);
    }
  }
  std::cout << "Trimmed: " << part.size() - trimmed.size() << " particles\n";

  // Re-center around previous position
  for (int i = 0; i < trimmed.size(); ++i) {
    Vec3 new_pos = trimmed[i]->GetX() + sinks[1]->GetX();
    trimmed[i]->SetX(new_pos);
  }

  file->SetParticles(trimmed);
  file->SetNameDataAppend(".hillradius");
}

void Application::OutputFile(SnapshotFile *file) {
  file->SetNameDataAppend(".modified");
  NameData nd = file->GetNameData();
  std::string outputName;

  if (nd.dir == "")
    nd.dir = ".";

  outputName =
      nd.dir + "/" + nd.id + "." + nd.format + "." + nd.snap + nd.append;

  // TODO: reduce code duplication
  if (nd.format == "df") {
    DragonFile *df = new DragonFile(nd, true);
    df->SetParticles(file->GetParticles());
    df->SetSinks(file->GetSinks());
    df->SetNumGas(file->GetNumGas());
    df->SetNumSinks(file->GetNumSinks());
    df->SetNumTot(file->GetNumPart());
    df->SetTime(file->GetTime());
    df->Write(outputName, true);
  }

  if (nd.format == "su") {
    SerenFile *su = new SerenFile(nd, false);
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
    cf->SetTime(0.0);
    cf->Write(outputName);
  }
}

void Application::FindThermo(SnapshotFile *file) {
  std::vector<Particle *> part = file->GetParticles();
  for (int i = 0; i < part.size(); ++i) {
    Particle *p = part[i];
    FLOAT density = p->GetD();
    FLOAT energy = p->GetU();
    FLOAT sigma = p->GetSigma();
    FLOAT temp = p->GetT();
    if (mInFormat == "su" || mInFormat == "sf" || mInFormat == "column") {
      temp = mOpacity->GetTemp(density, energy);
    } else if (mInFormat == "df" || mInFormat == "du") {
      energy = mOpacity->GetEnergy(density, temp);
    }
    FLOAT gamma = mOpacity->GetGamma(density, temp);
    FLOAT kappa = mOpacity->GetKappa(density, temp);
    FLOAT kappar = mOpacity->GetKappar(density, temp);
    FLOAT mu_bar = mOpacity->GetMuBar(density, temp);
    FLOAT press = (gamma - 1.0) * density * energy;
    FLOAT cs = sqrtf((K * temp) / (M_P * mu_bar));
    FLOAT tau = kappa * sigma;
    FLOAT dudt = 1.0 / ((sigma * sigma * kappa) + (1 / kappar));

    part[i]->SetR(part[i]->GetX().Norm());
    part[i]->SetT(temp);
    part[i]->SetU(energy);
    part[i]->SetP(press);
    part[i]->SetCS(cs);
    part[i]->SetOpacity(kappa);
    part[i]->SetRealOpacity(kappar);
    part[i]->SetTau(tau);
    part[i]->SetDUDT(dudt);
  }
  file->SetParticles(part);
}

void Application::FindOpticalDepth(SnapshotFile *file) {
  std::vector<Particle *> part = file->GetParticles();

  OpticalDepthOctree *octree = new OpticalDepthOctree(
      Vec3(0.0, 0.0, 0.0), Vec3(2048.0, 2048.0, 2048.0), NULL, NULL);

  std::vector<OpticalDepthOctree *> list;

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
  octree->Construct(positive);
  octree->LinkTree(list);
  octree->Walk(positive, mOpacity);
  for (int i = 0; i < positive.size(); ++i)
    part[i] = positive[i];

  for (int i = 0; i < negative.size(); ++i) {
    Particle *p = negative[i];
    FLOAT x = p->GetX().x;
    FLOAT y = p->GetX().y;
    FLOAT z = -(p->GetX().z);
    negative[i]->SetX(Vec3(x, y, z));
  }
  octree->Construct(negative);
  octree->LinkTree(list);
  octree->Walk(negative, mOpacity);
  // The particles below the disc which have been flipped above now require
  // being flipped back.
  for (int i = 0; i < negative.size(); ++i) {
    FLOAT x = negative[i]->GetX().x;
    FLOAT y = negative[i]->GetX().y;
    FLOAT z = -(negative[i]->GetX().z);
    negative[i]->SetX(Vec3(x, y, z));
    part[i + positive.size()] = negative[i];
  }

  for (int i = 0; i < part.size(); ++i) {
    Particle *p = part[i];
    FLOAT temp = p->GetT();
    FLOAT sigma = p->GetRealSigma();
    FLOAT tau = p->GetRealTau();
    FLOAT dudt = 1.0 / (sigma * (tau + (1.0 / tau)));

    part[i]->SetRealDUDT(dudt);
  }

  // Replaces 3D velocity data with the ratios of estimated to real values for
  // optical depth, column density and cooling rate. This is useful for plotting
  // 2D maps for the above in SPLASH.
  // Do we output difference logarithmically or not?
  FLOAT min_tau = 1E30, max_tau = -1E30;
  FLOAT min_sig = 1E30, max_sig = -1E30;
  FLOAT min_cool = 1E30, max_cool = -1E30;
  if (mParams->GetInt("OUTPUT_COOLING")) {
    for (int i = 0; i < part.size(); ++i) {
      Particle *p = part[i];
      FLOAT tau_ratio = log10(p->GetTau() / p->GetRealTau());
      FLOAT sigma_ratio = log10(p->GetSigma() / p->GetRealSigma());
      FLOAT cooling_ratio = log10(p->GetDUDT() / p->GetRealDUDT());

      if (tau_ratio > max_tau)
        max_tau = tau_ratio;
      if (tau_ratio < min_tau)
        min_tau = tau_ratio;
      if (sigma_ratio > max_sig)
        max_sig = sigma_ratio;
      if (sigma_ratio < min_sig)
        min_sig = sigma_ratio;
      if (cooling_ratio > max_cool)
        max_cool = cooling_ratio;
      if (cooling_ratio < min_cool)
        min_cool = cooling_ratio;

      part[i]->SetV(Vec3(tau_ratio, sigma_ratio, cooling_ratio));
    }
  }
  file->SetParticles(part);

  delete octree;
}

void Application::FindToomre(SnapshotFile *file) {
  // Find Toomre parameter, first sort by radius
  // THERE IS AN ISSUE WHEN INCLUDING SINKS IN THE RADIAL SORT. IMAGINE A SINK
  // HAS FORMED SOMEWHERE IN THE DISC, THIS DOES NOT GET INCLUDED IN THE SORT.
  // IT MAY BE BETTER TO STORE SINKS AS PARTICLES AND DISTINGUISH BY TYPE.
  std::vector<Particle *> part = file->GetParticles();
  std::vector<Sink *> sink = file->GetSinks();
  std::sort(part.begin(), part.end(),
            [](Particle *a, Particle *b) { return b->GetR() < a->GetR(); });
  FLOAT inner_mass = 0.0;
  if (sink.size() == 1)
    inner_mass += sink[0]->GetM();
  for (int i = 0; i < part.size(); ++i) {
    Particle *p = part[i];
    FLOAT r3 = pow(p->GetR() * AU_TO_M, 3.0);
    FLOAT omega = sqrtf((G * inner_mass * MSUN_TO_KG) / (r3));
    FLOAT cs = p->GetCS();
    FLOAT sigma = p->GetRealSigma() * GPERCM2_TO_KGPERM2;
    FLOAT Q = (cs * omega) / (PI * G * sigma);

    part[i]->SetOmega(omega);
    part[i]->SetQ(Q);
    inner_mass += p->GetM();
  }
  file->SetParticles(part);
}
