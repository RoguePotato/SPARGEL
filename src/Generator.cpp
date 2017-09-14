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

Generator::Generator
(Parameters *params,
 OpacityTable *opacity) : mParams(params), mOpacity(opacity) {

}

Generator::~Generator(void) {

}

void Generator::Create(void) {
  SetupParams();

  if (mParams->GetString("IC_TYPE") == "disc") {
    CreateDisc();
    CreateStars();
    CalculateVelocity();
  } else
  if (mParams->GetString("IC_TYPE") == "cloud") {
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
  mSigma0 = ((mMDisc) / (2 * PI * mR0 * mR0)) *
            powf(powf((mR0 * mR0 + mRout * mRout) /
            (mR0 * mR0), 0.5) -
            powf((mR0 * mR0 + mRin * mRin) / (mR0 * mR0), 0.5), -1.0f);

  if (mSeed > 0) {
    srand(mSeed);
  } else {
    srand(time(0));
  }
}

void Generator::GenerateRandoms() {
  // TODO: Add Mersenne twister
  for (int i = 0; i < 3; ++i) {
    mRands[i] = rand() / (double) RAND_MAX;
  }
}

void Generator::CreateDisc(void) {
  // Allocate memory
  for (int i = 0; i < mNumHydro; ++i) {
    mParticles.push_back(new Particle());
  }

  for (int i = 0; i < mNumHydro; ++i) {
    GenerateRandoms();

    double index[2];
    index[0] = 1.0f - (mP / 2.0f);
    index[1] = 2.0f / (2.0f - mP);

    double omegaIn = pow(1.0 + mOmegaIn, index[0]);
    double omegaOut = pow(1.0 + mOmegaOut, index[0]);
    double inner = (omegaIn + mRands[0] * (omegaOut - omegaIn));

    double omega = pow(inner, index[1]) - 1.0;
    double R = mR0 * pow(omega, 0.5f);
    double phi = 2 * PI * mRands[1];

    double x = R * cos(phi);
    double y = R * sin(phi);

    double sigma = mSigma0 *
                   pow((mR0 * mR0) / (mR0 * mR0 + R * R), mP / 2);

    double T = pow(pow(mTinf, 4.0) + pow(mT0, 4.0) *
               pow(pow(R, 2.0) + pow(mR0, 2.0), -2.0 * mQ), 0.25);
    double cS2 = ((K * T) / (MU * M_P)) / (AU_TO_M * AU_TO_M);

    double z_0 = -((PI * sigma * R * R * R) / (2 * mMStar)) +
                 pow(pow((PI * sigma * R * R * R) / (2 * mMStar), 2.0f) +
                 ((cS2 * R * R * R) / (G_AU * mMStar)), 0.5f);

    double z = (2 / PI) * z_0 * asin(2 * mRands[2] - 1);

    double rho = ((PI * mSigma0) / (4 * z_0)) *
                 pow((mR0 * mR0) / (mR0 * mR0 + R * R), 0.5) *
                 cos((PI * z) / (2 * z_0));

    double m = mMDisc / mNumHydro;

    double h = pow((3 * mNumNeigh * m) / (32 * PI * rho), (1.0f / 3.0f));

    double U = mOpacity->GetEnergy(rho, T);

    mParticles.at(i)->SetID(i);
    mParticles.at(i)->SetX(Vec3(x, y, z));
    mParticles.at(i)->SetT(T);
    mParticles.at(i)->SetH(h);
    mParticles.at(i)->SetD(rho);
    mParticles.at(i)->SetM(m);
    mParticles.at(i)->SetU(U);
    mParticles.at(i)->SetType(1);
  }
}

void Generator::CreateCloud(void) {

}

void Generator::CreateStars(void) {
  Sink* s = new Sink();
  s->SetID(mNumHydro + 1);
  s->SetH(1.0);
  s->SetM(mMStar);
  s->SetType(-1);
  mSinks.push_back(s);
}

void Generator::CalculateVelocity(void) {
  double shift = mRout * 2.0;
  mOctree = new Octree(Vec3(0.0, 0.0, 0.0), Vec3(5000.0, 5000.0, 5000.0));

  mOctreePoints = new OctreePoint[mParticles.size()];
  for(int i = 0; i < mParticles.size(); ++i) {
    double x = mParticles.at(i)->GetX()[0];
    double y = mParticles.at(i)->GetX()[1];
    double z = mParticles.at(i)->GetX()[2];
    double M = mParticles.at(i)->GetM();

    // shift the positions such that they are positive
    x += shift;
    y += shift;
    z += shift;

    mOctreePoints[i].SetPosition(Vec3(x, y, z));
    mOctreePoints[i].SetMass(M);
    mOctree->Insert(mOctreePoints + i);
  }

  for (int i = 0; i < mParticles.size(); ++i) {
    Vec3 acc = Vec3(0.0, 0.0, 0.0);
    Vec3 pos = mParticles.at(i)->GetX();
    double R = mParticles.at(i)->GetR();
    double x = mParticles.at(i)->GetX()[0];
    double y = mParticles.at(i)->GetX()[1];
    double h = mParticles.at(i)->GetH();

    pos[0] += shift;
    pos[1] += shift;
    pos[2] += shift;

    mOctree->TraverseTree(pos, acc, h);

    double v = sqrt(acc.Norm() * R) * AU_TO_KM;
    double vX = (-v * y) / (R + 0.000001);
    double vY = (v * x) / (R + 0.000001);

    mParticles.at(i)->SetV(Vec3(vX, vY, 0.0));
  }
}
