#pragma once
#include "Particle.h"


const FP ELECTRON_MASS = 9.10938215e-28;
const FP ELECTRON_CHARGE = -4.80320427e-10;
const FP LIGHT_VELOCITY = 29979245800.0;  // CGS


class Model {

	const Vector3 E, B;  // constant electromagnetic field
	ParticleEnsemble particles;

public:

	Model(const Vector3& E, const Vector3& B,
		const std::vector<Particle>& particles) :
		E(E), B(B), particles(particles) {}

	const ParticleEnsemble& getParticles() { return particles; }

__declspec(noinline) void update(const FP& dt) {
		auto particles = this->particles.getAccessor();

		const FP coeffR = dt / (FP)6;
		const FP coeffV = coeffR / ELECTRON_MASS;

#ifdef __NOVECTOR__
#pragma novector
#else
#pragma omp simd
#endif
		for (int i = 0; i < this->particles.getSize(); i++) {
			Vector3 v = particles[i].v();
			
			Vector3 k1 = v;			
			Vector3 k2 = v + (FP)0.5 * dt * k1;		
			Vector3 k3 = v + (FP)0.5 * dt * k2;
			Vector3 k4 = v + dt * k3;
			particles[i].r() += coeffR * (k1 + (FP)2 * k2 + (FP)2 * k3 + k4);
			
			k1 = getLorentzForce(v);			
			k2 = getLorentzForce(v + (FP)0.5 * dt * k1);			
			k3 = getLorentzForce(v + (FP)0.5 * dt * k2);			
			k4 = getLorentzForce(v + dt * k3);		
			particles[i].v() += coeffV * (k1 + (FP)2 * k2 + (FP)2 * k3 + k4);
		}
	}

private:

	Vector3 getLorentzForce(const Vector3& v) {
		return ELECTRON_CHARGE * (E + cross(v, B) / LIGHT_VELOCITY);
	}

};

