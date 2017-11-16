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
  for (int i = 0; i < 16; ++i) std::cout << "=====";
  std::cout << "\n\n";
  std::cout << "   SParGeL\n\n";
  std::cout << "   Smoothed Particle Generator and Loader\n\n";
  for (int i = 0; i < 16; ++i) std::cout << "=====";
  std::cout << "\n\n";
}

void Application::EndSplash(void) {
  for (int i = 0; i < 16; ++i) std::cout << "=====";
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
  if (mNumThreads < 0 || mNumThreads > mMaxThreads) mNumThreads = mMaxThreads;

  mConvert = mParams->GetInt("CONVERT");
  mInFormat = mParams->GetString("IN_FORMAT");
  mOutFormat = mParams->GetString("OUT_FORMAT");
  mOutput = mParams->GetInt("OUTPUT_FILES");
  mEosFilePath = mParams->GetString("EOS_TABLE");
  mCloudAnalyse = mParams->GetInt("CLOUD_ANALYSIS");
  mCloudCenter = mParams->GetInt("CLOUD_CENTER");
  mDiscAnalyse = mParams->GetInt("DISC_ANALYSIS");
  mSinkAnalyse = mParams->GetInt("SINK_ANALYSIS");
  mCenter = mParams->GetInt("DISC_CENTER");
  mRadialAnalyse = mParams->GetInt("RADIAL_ANALYSIS");

  mOpacity = new OpacityTable(mEosFilePath, true);
  if (!mOpacity->Read()) return false;

  if (mParams->GetInt("GENERATE")) {
    mGenerator = new Generator(mParams, mOpacity);
    mGenerator->Create();
    NameData nd;

    ColumnFile *cf = new ColumnFile(nd);
    cf->SetParticles(mGenerator->GetParticles());
    cf->SetSinks(mGenerator->GetSinks());
    OutputFile((SnapshotFile *) cf, "./disc_column.dat");
    return false;
  }

  if (mCloudAnalyse) {
    mCloudAnalyser = new CloudAnalyser(mParams->GetString("CLOUD_OUTPUT"));
  }

  if (mDiscAnalyse) {
    mDiscAnalyser = new DiscAnalyser();
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
    }
    else if (mInFormat == "sf"){
      mFiles.push_back(new SerenFile(nd, true));
    }
    else if (mInFormat == "du"){
      mFiles.push_back(new DragonFile(nd, false));
    }
    else if (mInFormat == "df"){
      mFiles.push_back(new DragonFile(nd, true));
    }
    else if (mInFormat == "column"){
      mFiles.push_back(new ColumnFile(nd));
    }
    else if (mInFormat == "sink") {
      mFiles.push_back(new SinkFile(nd));
    }
    else {
      std::cout << "Unrecognised input file format, exiting...\n";
      return false;
    }
  }

  // TODO: Handle case where user does not want to read files just generate
  // one. Possible run generator above here, and create the file, then
  // perform analysis. If no analysis is selected, it will just write.
  if (mFiles.size() < 1) {
    std::cout << "No files selected, exiting...\n";
    return false;
  }
  return true;
}

void Application::Run() {
  // Set up file batches
  if (mFiles.size() < mNumThreads) mNumThreads = mFiles.size();
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

  if (mCloudAnalyse) mCloudAnalyser->Write();
  if (mSinkAnalyse) {
    mSinkAnalyser->WriteMassRadius();
    mSinkAnalyser->WriteNbody();
  }

  std::cout << "   Files analysed   : " << mFilesAnalysed << "\n\n";
}

void Application::Analyse(int task, int start, int end) {
  for (int i = start; i < end; ++i) {
    // File read
    if (!mFiles.at(i)->Read()) break;

    // Extra quantity calculation
    if (mInFormat == "su" || mInFormat == "sf") {
      FindThermo((SnapshotFile *) mFiles.at(i));
    }
    // Cloud analysis
    if (mCloudAnalyse) {
      mCloudAnalyser->FindCentralQuantities((SnapshotFile *) mFiles.at(i));
      if (mCloudCenter) {
        mCloudAnalyser->CenterAroundDensest((SnapshotFile *) mFiles.at(i));
      }
    }
    // Disc analysis
    if (mDiscAnalyse) {
      if (mCenter) {
        mDiscAnalyser->Center((SnapshotFile *) mFiles.at(i), mCenter - 1);
      }
      FindRealSigma((SnapshotFile *) mFiles.at(i));
    }
    // Sink analysis
    if (mSinkAnalyse) {
      mSinkAnalyser->AddMassRadius((SinkFile *) mFiles.at(i));
      mSinkAnalyser->AddNbody((SinkFile *) mFiles.at(i));
    }
    // Radial analysis
    if (mRadialAnalyse) {
      RadialAnalyser *ra = new RadialAnalyser(
        mParams->GetInt("RADIUS_IN"),
        mParams->GetInt("RADIUS_OUT"),
        mParams->GetInt("RADIAL_BINS"),
        mParams->GetInt("RADIAL_LOG"),
        mParams->GetInt("VERTICAL_ANALYSIS"));
      ra->Run((SnapshotFile *) mFiles.at(i));
      delete ra;
    }
    // File conversion
    if (mConvert) {
      mFiles.at(i)->SetNameDataFormat(mOutFormat);
    }
    // Snapshot output
    if (mOutput) {
      OutputFile((SnapshotFile *) mFiles.at(i));
    }
    ++mFilesAnalysed;
    delete mFiles.at(i);
  }
}

void Application::OutputFile(SnapshotFile *file, std::string fileName) {
  NameData nd = file->GetNameData();
  std::string outputName;

  if (fileName == "") {
    outputName = nd.dir + "/" + nd.id + "." +
    nd.format + "." + nd.snap + nd.append;
  }
  else {
    outputName = fileName;
    nd.format = "column";
  }

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
    FLOAT temp = mOpacity->GetTemp(density, energy);
    FLOAT gamma = mOpacity->GetGamma(density, temp);
    FLOAT kappa = mOpacity->GetKappa(density, temp);
    FLOAT kappar = mOpacity->GetKappar(density, temp);
    FLOAT press = (gamma - 1.0) * density * energy;
    FLOAT tau = kappa * sigma;
    FLOAT ahydro = (1.0 / sigma) * 1.06 * press;

    part[i]->SetT(temp);
    part[i]->SetP(press);
    part[i]->SetOpacity(kappa);
    part[i]->SetRealOpacity(kappar);
    part[i]->SetTau(tau);
    part[i]->SetHydroAcc(ahydro);
  }
  file->SetParticles(part);
}

/*void Application::FindRealSigma(SnapshotFile *file) {
  std::vector<Particle *> part = file->GetParticles();
  std::sort(part.begin(), part.end(),
            [](Particle *a, Particle *b) { return b->GetX().z < a->GetX().z; });

  FLOAT h_fac = 2.0;

  // POSITIVE Z
  for (int i = 0; i < part.size(); ++i) {
    Particle *a = part[i];
    if (a->GetX().z < 0.0) break;

    FLOAT search = h_fac * a->GetH();
    FLOAT m = a->GetM(), cd = 0.0, dz = 0.0;
    FLOAT tau = a->GetD() * a->GetRealOpacity() * a->GetX().z;
    FLOAT d = a->GetD(), T = a->GetT();
    FLOAT maxZ = -1E30;
    int Npart = 1;

    for (int j = 0; j < i; ++j) {
      Particle *b = part[j];
      FLOAT dist = sqrt(pow(a->GetX().x - b->GetX().x, 2.0) +
                        pow(a->GetX().y - b->GetX().y, 2.0));
      if (dist > search) continue;

      m += b->GetM();
      dz = fabs(b->GetX().z - a->GetX().z);
      d += b->GetD();
      T += b->GetT();
      tau += b->GetD() * b->GetRealOpacity() * dz * AU_TO_CM;

      if (b->GetX().z > maxZ) maxZ = b->GetX().z;

      ++Npart;
    }
    // Sometimes there is no particle above in the neighbour list
    // so the particle is the highest around i.e. it is a surface particle.
    // As such, the distance to the surface is it's own z.
    if (Npart == 1) maxZ = 0.0;

    part.at(i)->SetRealSigma(
      (m / (PI * search * search)) * MSOLPERAU2_TO_GPERCM2);

    d /= Npart;
    T /= Npart;
    // tau /= Npart;
    tau = d * mOpacity->GetKappar(d, T) * fabs(a->GetX().z - maxZ) * AU_TO_CM;

    part[i]->SetRealTau(tau);
  }

  // NEGATIVE Z
  for (int i = part.size() - 1; i >= 0; --i) {
    Particle *a = part[i];
    if (a->GetX().z > 0.0) break;

    FLOAT search = h_fac * a->GetH();
    FLOAT m = a->GetM(), cd = 0.0, dz = 0.0;
    FLOAT tau = a->GetD() * a->GetRealOpacity() * fabs(a->GetX().z);
    FLOAT d = a->GetD(), T = a->GetT();
    FLOAT minZ = 1E30;
    int Npart = 1;

    for (int j = part.size() - 1; j > i; --j) {
      Particle *b = part[j];
      FLOAT dist = sqrt(pow(a->GetX().x - b->GetX().x, 2.0) +
                        pow(a->GetX().y - b->GetX().y, 2.0));
      if (dist > search) continue;

      m += b->GetM();
      dz = fabs(b->GetX().z - a->GetX().z);
      d += b->GetD();
      T += b->GetT();
      tau += b->GetD() * b->GetRealOpacity() * dz * AU_TO_CM;
      if (b->GetX().z < minZ) minZ = b->GetX().z;

      ++Npart;
    }
    if (Npart == 1) minZ = 0.0;

    part[i]->SetRealSigma(
      (m / (PI * search * search)) * MSOLPERAU2_TO_GPERCM2);

    d /= Npart;
    T /= Npart;
    // Opacity average or calculate opacity from average rho and T
    // tau /= Npart;
    tau = d * mOpacity->GetKappar(d, T) * fabs(a->GetX().z - minZ) * AU_TO_CM;

    part[i]->SetRealTau(tau);
  }

  file->SetParticles(part);

  std::ofstream out;
  out.open("sigma.dat");
  for (int i = 0; i < part.size(); ++i) {
    Particle *a = part[i];
    out << a->GetR() << "\t"
        << a->GetRealSigma() << "\t"
        << a->GetSigma() << "\t"
        << a->GetX().z << "\t"
        << a->GetTau() << "\t"
        << a->GetRealTau() << "\t"
        << a->GetD() << "\n";
  }
  out.close();
}*/

void Application::FindRealSigma(SnapshotFile *file) {
  std::vector<Particle *> part = file->GetParticles();

  // Sort full particle array by x for quicker neighbour find
  std::sort(part.begin(), part.end(),
  [](Particle *a, Particle *b) { return b->GetX().x > a->GetX().x; });

  // Number of intermediate points to test in the z-direction
  const int Z_TEST_POINTS = 1;

  // Assume a disc height and calculate the bin height
  const FLOAT DISC_HEIGHT = 10.0;
  FLOAT bin_height = DISC_HEIGHT / Z_TEST_POINTS;
  FLOAT bin_height_scaled = bin_height * AU_TO_CM;

  for (int i = 0; i < part.size(); ++i) {
    if (i % 100 == 0) std::cout << ".";
    Particle *a = part[i];
    FLOAT tau = 0.0;
    FLOAT x = a->GetX().x;
    FLOAT y = a->GetX().y;
    FLOAT z = a->GetX().z;
    int start_bin = fabs(z) / bin_height;

    for (int j = start_bin; j < Z_TEST_POINTS; ++j) {
      FLOAT curZ = j * bin_height;
      if (z < 0.0) curZ *= -1;
      Vec3 testPoint = Vec3(x, y, curZ);

      // Get nearest neighbours around the current interpolation point
      std::vector<Particle *> neighbours =
        FindNeighbours(part, testPoint, i);

      // Use smoothing length from the closest neighbour as we don't know
      // the smoothing length of the interpolating point
      FLOAT h = a->GetH();
      FLOAT invh = 1.0 / h;
      FLOAT dim_fac =  PI * pow(h * AU_TO_CM, 3.0);
      for (int n = 0; n < neighbours.size(); ++n) {
        Particle *neib = neighbours[n];
        FLOAT q = sqrt(pow(testPoint.x - neib->GetX().x, 2.0) +
                       pow(testPoint.y - neib->GetX().y, 2.0) +
                       pow(testPoint.z - neib->GetX().z, 2.0)) * invh;
        if (q > 2.0) continue;

        FLOAT w = 0.0;
        if (q > 0.0 && q <= 1.0) {
          w = 0.25 * pow(2.0 - q, 3.0) - pow(1.0 - q, 3.0);
        }
        else if (q > 1.0 && q <= 2.0) {
          w = 0.25 * pow(2.0 - q, 3.0);
        }

        FLOAT W = w / dim_fac;
        FLOAT M = neib->GetM() * MSUN_TO_G;
        FLOAT rho = W * M;
        FLOAT T = neib->GetT();
        FLOAT kappa = mOpacity->GetKappar(rho, T);
        tau += rho * kappa * bin_height_scaled;
      }
    }

    part[i]->SetRealTau(tau);
  }

  std::ofstream out;
  out.open("new_sigma.dat");
  for (int i = 0; i < part.size(); ++i) {
    out << part[i]->GetR() << "\t"
        << part[i]->GetX().x << "\t"
        << part[i]->GetX().y << "\t"
        << part[i]->GetX().z << "\t"
        << part[i]->GetTau() << "\t"
        << part[i]->GetRealTau() << "\t"
        << part[i]->GetD() << "\t"
        << part[i]->GetT() << "\n";
  }
}

std::vector<Particle *> Application::FindNeighbours(
 std::vector<Particle *> part,
 Vec3 pos,
 int part_index)
{
  std::vector<Particle *> result;
  FLOAT h = part[part_index]->GetH() * 2.0;
  int n_neigh = 0;
  for (int i = part_index + 1; i < part.size(); ++i) {
    if (fabs(part[i]->GetX().x - pos.x > h)) break;
    result.push_back(part[i]);
  }

  for (int i = part_index - 1; i > 0; --i) {
    if (fabs(pos.x - part[i]->GetX().x) > h) break;
    result.push_back(part[i]);
  }

  return result;
}
