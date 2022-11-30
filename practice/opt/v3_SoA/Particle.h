#pragma once
#include <vector>
#include "Vector3.h"


struct Particle {

	Vector3 r, v;

	Particle() {}
	Particle(Vector3 r, Vector3 v) :r(r), v(v) {}

};


class ParticleEnsemble {

	// SoA (Structure of Arrays)
	std::vector<FP> rx, ry, rz;
	std::vector<FP> vx, vy, vz;

public:

	ParticleEnsemble(const std::vector<Particle>& particles) :
		rx(particles.size()), ry(particles.size()), rz(particles.size()),
		vx(particles.size()), vy(particles.size()), vz(particles.size())
	{
		for (int i = 0; i < (int)particles.size(); i++) {
			rx[i] = particles[i].r.x;
			ry[i] = particles[i].r.y;
			rz[i] = particles[i].r.z;

			vx[i] = particles[i].v.x;
			vy[i] = particles[i].v.y;
			vz[i] = particles[i].v.z;
		}
	}

	// read-write access
	FP& Rx(int index) { return rx[index]; }
	FP& Ry(int index) { return ry[index]; }
	FP& Rz(int index) { return rz[index]; }

	FP& Vx(int index) { return vx[index]; }
	FP& Vy(int index) { return vy[index]; }
	FP& Vz(int index) { return vz[index]; }
	
	FP* getRxPtr() { return rx.data(); }
	FP* getRyPtr() { return ry.data(); }
	FP* getRzPtr() { return rz.data(); }
	
	FP* getVxPtr() { return vx.data(); }
	FP* getVyPtr() { return vy.data(); }
	FP* getVzPtr() { return vz.data(); }

	// read access
	Vector3 getR(int index) const { return Vector3(rx[index], ry[index], rz[index]); }
	Vector3 getV(int index) const { return Vector3(vx[index], vy[index], vz[index]); }

	// write access
	void setR(int index, const Vector3& r) {
		rx[index] = r.x;
		ry[index] = r.y;
		rz[index] = r.z;
	}
	void setV(int index, const Vector3& v) {
		vx[index] = v.x;
		vy[index] = v.y;
		vz[index] = v.z;
	}
	
	Particle getParticle(int index) const {
		return Particle(
			Vector3(rx[index], ry[index], rz[index]),
			Vector3(vx[index], vy[index], vz[index])
		);
	}

	int getSize() const { return rx.size(); }

};
