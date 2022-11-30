#pragma once
#include <vector>
#include "Vector3.h"


struct Particle {

	Vector3 r, v;

	Particle() {}
	Particle(Vector3 r, Vector3 v) :r(r), v(v) {}

};


class ParticleEnsemble {

	std::vector<Particle> ensemble;  // AoS (Array of Structures)

public:

	ParticleEnsemble(const std::vector<Particle>& particles) :
		ensemble(particles) {}

	// hint: use only for AoS data stucture
	Particle& operator[] (int index) { return ensemble[index]; }

	// read-write access
	FP& Rx(int index) { return ensemble[index].r.x; }
	FP& Ry(int index) { return ensemble[index].r.y; }
	FP& Rz(int index) { return ensemble[index].r.z; }
	
	FP& Vx(int index) { return ensemble[index].v.x; }
	FP& Vy(int index) { return ensemble[index].v.y; }
	FP& Vz(int index) { return ensemble[index].v.z; }

	// read access
	Vector3 getR(int index) const { return ensemble[index].r; }
	Vector3 getV(int index) const { return ensemble[index].v; }

	// write access
	void setR(int index, const Vector3& r) { ensemble[index].r = r; }
	void setV(int index, const Vector3& v) { ensemble[index].v = v; }
	
	Particle getParticle(int index) const {
		return ensemble[index];
	}

	int getSize() const { return ensemble.size(); }

};

