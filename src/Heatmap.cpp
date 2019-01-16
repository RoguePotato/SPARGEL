//===-- Heatmap.cpp -------------------------------------------------------===//
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
/// Heatmap.cpp
///
//===----------------------------------------------------------------------===//

#include "Heatmap.h"

Heatmap::Heatmap(const int res) : mRes(res) {}

Heatmap::~Heatmap() {}

void Heatmap::Create(SnapshotFile *file) {
  const std::vector<Particle *> part = file->GetParticles();
  float rout = file->GetOuterRadius(1);
  float grid_size = (2.0f * rout) / (float)mRes;

  std::cout << "   Heatmap: rout = " << rout << " with resolution " << mRes
            << "\n";

  for (int i = 0; i < mRes; ++i) {
    mGrid.push_back(std::vector<float>(mRes, 0.0f));
    mGridNum.push_back(std::vector<int>(mRes, 0));
  }

  for (int i = 0; i < part.size(); ++i) {
    if (part[i]->GetX().Norm() > rout) {
      continue;
    }

    float x = part[i]->GetX().x;
    float y = part[i]->GetX().y;

    int ind_x = Bin(rout * 2.0f, x);
    int ind_y = Bin(rout * 2.0f, y);

    if (ind_x < 0 || ind_x > mRes || ind_y < 0 || ind_y > mRes) {
      std::cout << "Error in assigning bins in heatmap!\n";
    }

    mGrid[ind_x][ind_y] += part[i]->GetTau();
    mGridNum[ind_x][ind_y]++;
  }

  NameData nd = file->GetNameData();
  mFileName = nd.dir + "/SPARGEL." + nd.id + "." + nd.format + "." + nd.snap +
              ".heatmap" + std::to_string(mRes);
}

void Heatmap::Output() {
  std::ofstream out;
  out.open(mFileName);
  for (int i = 0; i < mRes; ++i) {
    for (int j = 0; j < mRes; ++j) {
      if (mGridNum[i][j] == 0) {
        out << this->AverageNeighbours(i, j) << "\t";
      } else {
        out << mGrid[i][j] / mGridNum[i][j] << "\t";
      }
    }
    out << "\n";
  }
  out.close();
}

int Heatmap::Bin(float d, float x) {
  // Move to positive space.
  x += d / 2.0f;

  // Fraction along the axis.
  float frac = x / d;

  return frac * mRes;
}

float Heatmap::AverageNeighbours(const int x, const float y) {
  if (x == 0 || y == 0 || x == mRes - 1 || y == mRes - 1) {
    return 0.0f;
  }

  float total = 0.0f;
  for (int i = -1; i < 1; ++i) {
    for (int j = -1; j < 1; ++j) {
      total += mGrid[x + j][y + i];
    }
  }

  return total / 8.0f;
}