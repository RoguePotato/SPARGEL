//===-- Generator.cpp -----------------------------------------------------===//
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
/// Generator.cpp
///
//===----------------------------------------------------------------------===//

#include "Generator.h"

Generator::Generator(Parameters *params, OpacityTable *opacity)
    : mParams(params), mOpacity(opacity) {}

Generator::~Generator() {
  delete mOctree;
  delete[] mOctreePoints;
}

void Generator::Create() {
  SetupParams();

  if (mParams->GetString("IC_TYPE") == "disc" ||
      mParams->GetString("IC_TYPE") == "binary") {
    CreateDisc();
    CreateStars();
    CalculateVelocity();
    if (mPlanet) {
      CreatePlanet();
    }
  } else if (mParams->GetString("IC_TYPE") == "cloud") {
    CreateCloud();
  }
}

void Generator::SetupParams() {
  mSeed = mParams->GetInt("SEED");
  mNumHydro = mParams->GetInt("N_HYDRO");
  mDim = mParams->GetInt("DIMENSIONS");
  mMStar = mParams->GetFloat("M_STAR");
  mMBinary = mParams->GetFloat("BINARY_M");
  mMTotal = mMStar + mMBinary;
  mBinarySep = mParams->GetFloat("BINARY_A");
  mBinaryEcc = mParams->GetFloat("BINARY_ECC");
  mBinaryInc = mParams->GetFloat("BINARY_INC");
  mMDisc = mParams->GetFloat("M_DISC");
  mRin = mParams->GetFloat("R_IN");
  mRout = mParams->GetFloat("R_OUT");
  mR0 = mParams->GetFloat("R_0");
  mT0 = mParams->GetFloat("T_0");
  mTinf = mParams->GetFloat("T_INF");
  mNumNeigh = mParams->GetInt("N_NEIGH");
  mP = mParams->GetFloat("P");
  mQ = mParams->GetFloat("Q");
  mSinkRadius = mParams->GetFloat("SINK_RADIUS");
  mPlanet = mParams->GetInt("PLANET");
  mPlanetMass = mParams->GetFloat("PLANET_MASS");
  mPlanetRadius = mParams->GetFloat("PLANET_RADIUS");
  mPlanetEcc = mParams->GetFloat("PLANET_ECC");
  mPlanetInc = mParams->GetFloat("PLANET_INC");
  mCloudRadius = mParams->GetFloat("CLOUD_RADIUS");
  mCloudMass = mParams->GetFloat("CLOUD_MASS");

  mOmegaIn = (mRin * mRin) / (mR0 * mR0);
  mOmegaOut = (mRout * mRout) / (mR0 * mR0);
  mSigma0 =
      ((mMDisc * (2.0 - mP)) / (2 * PI * mR0 * mR0)) *
      pow(pow((mR0 * mR0 + mRout * mRout) / (mR0 * mR0), 1.0 - (mP / 2.0)) -
              pow((mR0 * mR0 + mRin * mRin) / (mR0 * mR0), 1.0 - (mP / 2.0)),
          -1.0f);

  mCloudVol = (4.0f / 3.0f) * PI * powf(mCloudRadius, 3.0f);

  if (mSeed > 0) {
    srand(mSeed);
  } else {
    srand(time(0));
  }
}

void Generator::GenerateRandoms() {
  // TODO: Add Mersenne twister
  for (int i = 0; i < 3; ++i) {
    mRands[i] = rand() / (FLOAT)RAND_MAX;
  }
}

void Generator::CreateDisc() {
  // Allocate memory
  for (int i = 0; i < mNumHydro; ++i) {
    mParticles.push_back(new Particle());
  }

  for (int i = 0; i < mNumHydro; ++i) {
    GenerateRandoms();

    FLOAT index[2];
    index[0] = 1.0f - (mP / 2.0f);
    index[1] = 2.0f / (2.0f - mP);

    FLOAT omegaIn = pow(1.0 + mOmegaIn, index[0]);
    FLOAT omegaOut = pow(1.0 + mOmegaOut, index[0]);
    FLOAT inner = (omegaIn + mRands[0] * (omegaOut - omegaIn));

    FLOAT omega = pow(inner, index[1]) - 1.0;
    FLOAT R = mR0 * pow(omega, 0.5f);
    FLOAT phi = 2 * PI * mRands[1];

    FLOAT x = R * cos(phi);
    FLOAT y = R * sin(phi);

    FLOAT sigma = mSigma0 * pow((mR0 * mR0) / (mR0 * mR0 + R * R), mP / 2.0);

    FLOAT T =
        pow(pow(mTinf, 4.0) +
                pow(mT0, 4.0) * pow(pow(R, 2.0) + pow(mR0, 2.0), -2.0 * mQ),
            0.25);
    FLOAT cS2 = ((K * T) / (MU * M_P)) / (AU_TO_M * AU_TO_M);

    FLOAT z_0 = -((PI * sigma * R * R * R) / (2.0 * mMStar)) +
                pow(pow((PI * sigma * R * R * R) / (2.0 * mMStar), 2.0) +
                        ((cS2 * R * R * R) / (G_AU * mMStar)),
                    0.5);

    FLOAT z = (2.0 / PI) * z_0 * asin(2.0 * mRands[2] - 1.0);

    FLOAT rho_0 = ((PI * mSigma0) / (4.0 * z_0)) *
                  pow((mR0 * mR0) / (mR0 * mR0 + R * R), mP / 2.0);

    FLOAT rho = (rho_0 * cos((PI * z) / (2 * z_0)));

    FLOAT m = mMDisc / mNumHydro;

    FLOAT h = pow((3 * mNumNeigh * m) / (32.0 * PI * (rho)), (1.0 / 3.0));

    FLOAT U = mOpacity->GetEnergy(rho, T);

    mParticles[i]->SetID(i);
    mParticles[i]->SetX(Vec3(x, y, z));
    mParticles[i]->SetR(Vec3(x, y, z).Norm());
    mParticles[i]->SetT(T);
    mParticles[i]->SetH(h);
    mParticles[i]->SetD(rho * MSOLPERAU3_TO_GPERCM3);
    mParticles[i]->SetM(m);
    mParticles[i]->SetU(U);
    mParticles[i]->SetSigma(sigma);
    mParticles[i]->SetType(1);
  }
}

void Generator::CreateCloud() {
  // Allocate memory
  for (int i = 0; i < mNumHydro; ++i) {
    mParticles.push_back(new Particle());
  }

  for (int i = 0; i < mNumHydro; ++i) {
    GenerateRandoms();

    FLOAT r = powf(mRands[0], (1.0f / 3.0f)) * mCloudRadius;
    FLOAT theta = acos(1.0f - 2.0f * mRands[1]);
    FLOAT phi = 2.0f * PI * mRands[2];

    FLOAT x = r * sin(theta) * cos(phi);
    FLOAT y = r * sin(theta) * sin(phi);
    FLOAT z = r * cos(theta);

    FLOAT m = mCloudMass / mNumHydro;
    FLOAT rho = mCloudMass / mCloudVol;

    FLOAT h = powf((3 * mNumNeigh * m) / (32 * PI * (rho)), (1.0f / 3.0f));
    FLOAT T = 5.0f;
    FLOAT U = (K * T) / (2.35f * M_P * 0.66666f);

    mParticles[i]->SetID(i);
    mParticles[i]->SetX(Vec3(x, y, z));
    mParticles[i]->SetR(Vec3(x, y, z).Norm());
    mParticles[i]->SetT(T);
    mParticles[i]->SetH(h);
    mParticles[i]->SetD(rho * MSOLPERAU3_TO_GPERCM3);
    mParticles[i]->SetM(m);
    mParticles[i]->SetU(U);
    mParticles[i]->SetType(1);
  }
}

void Generator::CreateStars() {
  Sink *s1 = new Sink();
  s1->SetID(mParticles.size() + 1);
  s1->SetH(mSinkRadius);
  s1->SetM(mMStar);
  s1->SetType(-1);

  if (mParams->GetString("IC_TYPE") == "binary") {
    Sink *s2 = new Sink();
    s2->SetID(mParticles.size() + 2);
    s2->SetH(mSinkRadius);
    s2->SetM(mMBinary);

    FLOAT x1 = -mBinarySep * (1.0 - mBinaryEcc) * (mMBinary / mMTotal) +
               ((mBinarySep / 2.0) * (1.0 - cos(mBinaryInc)));
    FLOAT x2 = mBinarySep * (1.0 - mBinaryEcc) * (mMStar / mMTotal) -
               ((mBinarySep / 2.0) * (1.0 - cos(mBinaryInc)));
    FLOAT z1 = (mBinarySep / 2.0) * sin(mBinaryInc);
    FLOAT z2 = -(mBinarySep / 2.0) * sin(mBinaryInc);

    s1->SetX(Vec3(x1, 0.0, z1));
    s2->SetX(Vec3(x2, 0.0, z2));

    mSinks.push_back(s2);
  }
  mSinks.push_back(s1);
}

void Generator::CreatePlanet() {
  Sink *s = new Sink();
  s->SetID(mParticles.size() + mSinks.size() + 1);
  Vec3 star_pos = mSinks[0]->GetX();
  Vec3 star_vel = mSinks[0]->GetV();
  Vec3 planet_pos = Vec3(mPlanetRadius * (1.0 - mPlanetEcc), 0.0, 0.0);
  Vec3 planet_vel = Vec3(0.0, 0.0, 0.0);

  FLOAT hill_radius =
      mPlanetRadius * pow(mPlanetMass / (3.0 * mMStar), 1.0 / 3.0);
  FLOAT h = 0.1;
  FLOAT interior_mass = 0.0;
  for (int i = 0; i < mParticles.size(); ++i) {
    if (mParticles[i]->GetX().Norm() < mPlanetRadius) {
      interior_mass += mParticles[i]->GetM();
    }
  }

  planet_vel[1] = sqrt((G * (mMStar + interior_mass) * MSUN_TO_KG) /
                       (mPlanetRadius * AU_TO_M)) /
                  KMPERS_TO_MPERS;
  s->SetH(h);
  s->SetM(mPlanetMass);
  s->SetType(-1);
  s->SetX(planet_pos);
  s->SetV(planet_vel);
  mSinks.push_back(s);
}

void Generator::CalculateVelocity() {
  mOctree = new Octree(Vec3(0.0, 0.0, 0.0), Vec3(512.0, 512.0, 512.0));

  // Insert particles
  mOctreePoints = new OctreePoint[mParticles.size() + mSinks.size()];
  for (int i = 0; i < mParticles.size(); ++i) {
    Vec3 pos = mParticles.at(i)->GetX();
    FLOAT M = mParticles.at(i)->GetM();

    mOctreePoints[i].SetPosition(pos);
    mOctreePoints[i].SetMass(M);
    mOctree->Insert(mOctreePoints + i);
  }

  // Insert sinks
  for (int i = 0; i < mSinks.size(); ++i) {
    Vec3 pos = mSinks.at(i)->GetX();
    FLOAT M = mSinks.at(i)->GetM();

    mOctreePoints[i].SetPosition(pos);
    mOctreePoints[i].SetMass(M);
    mOctree->Insert(mOctreePoints + i);
  }

  for (int i = 0; i < mParticles.size(); ++i) {
    Vec3 acc = Vec3(0.0, 0.0, 0.0);
    Vec3 pos = mParticles.at(i)->GetX();
    FLOAT R = mParticles.at(i)->GetR();
    FLOAT x = mParticles.at(i)->GetX()[0];
    FLOAT y = mParticles.at(i)->GetX()[1];
    FLOAT h = mParticles.at(i)->GetH();

    mOctree->TraverseTree(pos, acc, h);

    FLOAT v = sqrt(acc.Norm() * R) * AU_TO_KM;
    FLOAT vX = (-v * y) / (R + 0.000001);
    FLOAT vY = (v * x) / (R + 0.000001);

    mParticles.at(i)->SetV(Vec3(vX, vY, 0.0));
  }

  if (mParams->GetString("IC_TYPE") == "binary") {
    FLOAT v_y1 = -sqrt((G * mMTotal * MSUN_TO_KG) / (mBinarySep * AU_TO_M)) *
                 sqrt((1.0 + mBinaryEcc) / (1.0 - mBinaryEcc)) *
                 (mMBinary / mMTotal);
    FLOAT v_y2 = sqrt((G * mMTotal * MSUN_TO_KG) / (mBinarySep * AU_TO_M)) *
                 sqrt((1 + mBinaryEcc) / (1.0 - mBinaryEcc)) *
                 (mMStar / mMTotal);

    v_y1 /= KMPERS_TO_MPERS;
    v_y2 /= KMPERS_TO_MPERS;

    mSinks[0]->SetV(Vec3(0.0, v_y1, 0.0));
    mSinks[1]->SetV(Vec3(0.0, v_y2, 0.0));
  } else {
    mSinks[0]->SetV(Vec3(0.0, 0.0, 0.0));
  }
}
