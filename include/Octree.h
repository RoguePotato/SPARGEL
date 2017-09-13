//===-- Octree.h -------------------------------------------------------===//
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
/// Octree.h contains the functions to create an Barnes-Hut octree via recursion
/// (because we are lazy).
///
//===----------------------------------------------------------------------===//

#pragma once

#include "Constants.h"
#include "Definitions.h"
#include "Vec.h"

class OctreePoint {
	Vec3 pos;
	double mass;
public:
	OctreePoint() { }
	OctreePoint(const Vec3& p, double m) : pos(p), mass(m) { }
	inline Vec3& GetPosition() { return pos; }
	inline void SetPosition(const Vec3& p) { pos = p; }
	inline const double& GetMass() const { return mass; }
	inline void SetMass(const double& m) { mass = m; }
};


class Octree {
	public:

	Vec3 origin;
	Vec3 halfDimension;

	Octree *children[8];
	OctreePoint *data;
	double totalMass = 0.0f;
	Vec3 totalMassRadius = Vec3(0.0f, 0.0f, 0.0f);

	Octree(const Vec3& origin, const Vec3& halfDimension) : origin(origin), halfDimension(halfDimension), data(NULL) {
		for (int i = 0; i < 8; ++i) {
			children[i] = NULL;
		}
	}

	Octree(const Octree& copy) : origin(copy.origin), halfDimension(copy.halfDimension), data(copy.data) {

	}

	~Octree() {
		for (uint i = 0; i < 8; ++i) {
			delete children[i];
		}
	}

	int GetOctantContainingPoint(const Vec3& point) const {
		int oct = 0;
		if (point.x >= origin.x) oct |= 4;
		if (point.y >= origin.y) oct |= 2;
		if (point.z >= origin.z) oct |= 1;
		return oct;
	}

	bool IsLeafNode() const {
		return children[0] == NULL;
	}

	void Insert(OctreePoint* point) {
		totalMass += point->GetMass();
		totalMassRadius += point->GetMass() * point->GetPosition();

		if (IsLeafNode()) {
			if (data == NULL) {
				data = point;
				return;
			} else {
				OctreePoint *oldPoint = data;
				data = NULL;

				for (uint i = 0; i < 8; ++i) {
					Vec3 newOrigin = origin;
					newOrigin.x += halfDimension.x * (i & 4 ? 0.5 : -0.5);
					newOrigin.y += halfDimension.y * (i & 2 ? 0.5 : -0.5);
					newOrigin.z += halfDimension.z * (i & 1 ? 0.5 : -0.5);
					children[i] = new Octree(newOrigin, halfDimension * 0.5);
				}

				children[GetOctantContainingPoint(oldPoint->GetPosition())]->Insert(oldPoint);
				children[GetOctantContainingPoint(point->GetPosition())]->Insert(point);
			}
		} else {
			int Octant = GetOctantContainingPoint(point->GetPosition());
			children[Octant]->Insert(point);
		}
	}

	void TraverseTree(const Vec3 particlePos, Vec3 &acc, double h) {
		if (IsLeafNode()) {
			if (data != NULL) {
				Vec3 R = data->GetPosition() - particlePos;
				R = (totalMassRadius / totalMass) - particlePos;

				Vec3 sumAcc = ((G_AU * totalMass)) / (R.NormSquared() + h * h) * R.Normalized();
				if (!sumAcc.IsNaN()) acc += sumAcc;
			}
		} else {
			for (uint i = 0; i < 8; ++i) {
				Vec3 R = (totalMassRadius / totalMass) - particlePos;

				double s = 2.0f * children[i]->halfDimension[0];
				double d = (children[i]->origin - particlePos).Norm();

				if (s / d <= THETA) {
					acc += ((G_AU * children[i]->totalMass)) / (R.NormSquared() + h * h) * R.Normalized();
				} else {
					children[i]->TraverseTree(particlePos, acc, h);
				}
			}
		}
	}

};
