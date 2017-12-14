//===-- OpticalDepthOctree.cpp --------------------------------------------===//
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
/// OpticalDepthOctree.cpp
///
//===----------------------------------------------------------------------===//

#include "OpticalDepthOctree.h"

OpticalDepthOctree::OpticalDepthOctree
(const Vec3& O,
 const Vec3& HD,
 OpticalDepthOctree* Next,
 OpticalDepthOctree* More) :
	Origin(O), HalfDimension(HD), _Next(Next), _More(More) {
	for (int i = 0; i < 8; ++i) {
		Children[i] = NULL;
	}
}

OpticalDepthOctree::OpticalDepthOctree(const OpticalDepthOctree& Copy) :
Origin(Copy.Origin), HalfDimension(Copy.HalfDimension), Data(Copy.Data) {

}

OpticalDepthOctree::~OpticalDepthOctree() {
	for (int i = 0; i < 8; ++i) {
		delete Children[i];
	}
}

void OpticalDepthOctree::Construct
(std::vector<Particle*> Particles,
 const FLOAT Shift) {
	OpticalDepthPoint* OP = new OpticalDepthPoint[Particles.size()];
	for (int i = 0; i < Particles.size(); ++i) {
		Particle* P = Particles[i];
		Vec3 Position = P->GetX();

		Position.x += Shift;
		Position.y += Shift;
		Position.z += Shift;

		OP[i].pos = Position;
    OP[i].h = P->GetH();
		OP[i].dens = P->GetD();
		OP[i].temp = P->GetT();
		this->Insert(OP + i);
	}
}

void OpticalDepthOctree::Walk
(std::vector<Particle*> &Particles,
 const FLOAT Shift,
 OpacityTable* Opacity) {
	for (int i = 0; i < Particles.size(); ++i) {
		Particle* P = Particles[i];
		Vec3 Pos = P->GetX();
		FLOAT Contrib = 0.0;

    Pos.x += Shift;
    Pos.y += Shift;
    Pos.z += Shift;

		TraverseTree(Pos, Contrib, Opacity);

		Particles[i]->SetRealTau(Contrib);
	}
}

void OpticalDepthOctree::Insert(OpticalDepthPoint* Point) {
	TotalPoints++;
	if (IsLeafNode()) {
		// LEAF: creates children
		if (Data == NULL) {
			Data = Point;
			return;
		}
		else if (TotalPoints > 1) {
			OpticalDepthPoint* OldPoint = Data;
			Data = NULL;
			for (int i = 0; i < 8; ++i) {
				Vec3 NewOrigin = Origin;
				NewOrigin.x += HalfDimension.x * (i & 4 ? 0.5 : -0.5);
				NewOrigin.y += HalfDimension.y * (i & 2 ? 0.5 : -0.5);
				NewOrigin.z += HalfDimension.z * (i & 1 ? 0.5 : -0.5);
				Children[i] = new OpticalDepthOctree(NewOrigin, HalfDimension * 0.5, this, NULL);
			}

			// More points to the first child
			_More = Children[0];

			// Each child points to it's sibling
			for (int i = 0; i < 7; ++i) Children[i]->_Next = Children[i + 1];

			// The final child points back to its parent's next pointer
			Children[7]->_Next = _Next;

			Children[GetOctantContainingPoint(OldPoint->pos)]->Insert(OldPoint);
			Children[GetOctantContainingPoint(Point->pos)]->Insert(Point);
		}
	}
	else {
		// Node: has children
		int Octant = GetOctantContainingPoint(Point->pos);
		_More = Children[Octant];
		Children[Octant]->Insert(Point);
	}
}

void OpticalDepthOctree::LinkTree(std::vector<OpticalDepthOctree*> &List) {
	if (IsLeafNode()) {
		List.push_back(this);
	}
	else {
		List.push_back(this);
		for (int i = 0; i < 8; ++i) {
			Children[i]->LinkTree(List);
		}
	}
}

void OpticalDepthOctree::TraverseTree
(const Vec3 ParticlePos,
 FLOAT &Contrib,
 OpacityTable *opacity) {
  // If the particle x-y does not intersect with the current cell boundary,
  // then we do not need to add any optical depth contribution.
  if (!Intersects(this, ParticlePos) /* && Data == NULL*/) return;

  if (IsLeafNode()) {
    // Leaf: Add the contribution.
    if (Data != NULL) {
      // We do not want to add the contribution from cells below the particle.
      if (Data->pos.z < ParticlePos.z) return;

      FLOAT dens = Data->dens;
      FLOAT temp = Data->temp;

      Contrib += dens * opacity->GetKappar(dens, temp) *
                 this->HalfDimension.z * 2.0 * AU_TO_CM;
		}
	} else {
    // Node: Go to children.
    for (int i = 0; i < 8; ++i) {
      Children[i]->TraverseTree(ParticlePos, Contrib, opacity);
    }
	}
}

int OpticalDepthOctree::GetOctantContainingPoint(const Vec3& Point) const {
	int Result = 0;
	if (Point.x >= Origin.x) Result |= 4;
	if (Point.y >= Origin.y) Result |= 2;
	if (Point.z >= Origin.z) Result |= 1;

	return Result;
}

bool OpticalDepthOctree::IsLeafNode() const {
	return (Children[0] == NULL);
}

bool OpticalDepthOctree::Intersects
(OpticalDepthOctree* Cell,
 const Vec3 ParticlePos) const {

  FLOAT x = ParticlePos.x;
  FLOAT y = ParticlePos.y;

  FLOAT l = Cell->Origin.x - Cell->HalfDimension.x;
  FLOAT r = Cell->Origin.x + Cell->HalfDimension.x;
  FLOAT b = Cell->Origin.y - Cell->HalfDimension.y;
  FLOAT t = Cell->Origin.y + Cell->HalfDimension.y;

  if (x > l && x < r && y > b && y < t) return true;

  return false;
}
