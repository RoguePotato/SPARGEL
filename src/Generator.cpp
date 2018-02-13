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

Generator::~Generator(void) {}

void Generator::Create(void) {
  SetupParams();

  if (mParams->GetString("IC_TYPE") == "disc") {
    CreateDisc();
    CreateStars();
    CalculateVelocity();
  } else if (mParams->GetString("IC_TYPE") == "cloud") {
    CreateCloud();
  }
}

void Generator::SetupParams(void) {
  mSeed = mParams->GetInt("SEED");
  mNumHydro = mParams->GetInt("N_HYDRO");
  mDim = mParams->GetInt("DIMENSIONS");
  mMStar = mParams->GetFloat("M_STAR");
  mMDisc = mParams->GetFloat("M_DISC");
  mRin = mParams->GetFloat("R_IN");
  mRout = mParams->GetFloat("R_OUT");
  mR0 = mParams->GetFloat("R_0");
  mT0 = mParams->GetFloat("T_0");
  mTinf = mParams->GetFloat("T_INF");
  mNumNeigh = mParams->GetInt("N_NEIGH");
  mP = mParams->GetFloat("P");
  mQ = mParams->GetFloat("Q");

  mOmegaIn = (mRin * mRin) / (mR0 * mR0);
  mOmegaOut = (mRout * mRout) / (mR0 * mR0);
  mSigma0 = ((mMDisc * (2.0 - mP)) / (2 * PI * mR0 * mR0)) *
            powf(powf((mR0 * mR0 + mRout * mRout) / (mR0 * mR0), 1.0 - (mP / 2.0)) -
                     powf((mR0 * mR0 + mRin * mRin) / (mR0 * mR0), 1.0 - (mP / 2.0)),
                 -1.0f);

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

void Generator::CreateDisc(void) {
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
                  pow((mR0 * mR0) / (mR0 * mR0 + R * R), mP / 2.0) *
                  MSOLPERAU3_TO_GPERCM3;

    FLOAT rho = (rho_0 * cos((PI * z) / (2 * z_0)));

    FLOAT m = mMDisc / mNumHydro;

    FLOAT h =
        pow((3 * mNumNeigh * m) / (32.0 * PI * (rho / MSOLPERAU3_TO_GPERCM3)),
            (1.0 / 3.0));

    FLOAT U = mOpacity->GetEnergy(rho, T);

    mParticles[i]->SetID(i);
    mParticles[i]->SetX(Vec3(x, y, z));
    mParticles[i]->SetR(Vec3(x, y, z).Norm());
    mParticles[i]->SetT(T);
    mParticles[i]->SetH(h);
    mParticles[i]->SetD(rho);
    mParticles[i]->SetM(m);
    mParticles[i]->SetU(U);
    mParticles[i]->SetSigma(sigma);
    mParticles[i]->SetType(1);
  }
}

void Generator::CreateCloud(void) {}

void Generator::CreateStars(void) {
  Sink *s = new Sink();
  s->SetID(mParticles.size() + 1);
  s->SetH(1.0);
  s->SetM(mMStar);
  s->SetType(-1);
  mSinks.push_back(s);
}

void Generator::CalculateVelocity(void) {
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
}
