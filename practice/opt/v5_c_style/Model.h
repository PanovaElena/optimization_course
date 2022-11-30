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
		const int nParticles = particles.getSize();
		FP coeff = (FP)1.0 / ELECTRON_MASS;
		
		const FP coeffR = dt / (FP)6;
		const FP coeffV = coeffR / ELECTRON_MASS;
		const FP invLightVelocity = 1.0 / LIGHT_VELOCITY;

		FP* rxPtr = particles.getRxPtr(), * ryPtr = particles.getRyPtr(), * rzPtr = particles.getRzPtr();
		FP* vxPtr = particles.getVxPtr(), * vyPtr = particles.getVyPtr(), * vzPtr = particles.getVzPtr();

#ifdef __NOVECTOR__
#pragma novector
#else
#pragma omp simd
#endif
		for (int i = 0; i < nParticles; i++) {
			FP vx = vxPtr[i], vy = vyPtr[i], vz = vzPtr[i];
			FP rx = rxPtr[i], ry = ryPtr[i], rz = rzPtr[i];
			
			// compute new r
			
			FP k1x = vx;
			FP k1y = vy;
			FP k1z = vz;
			
			FP k2x = vx + (FP)0.5 * dt * k1x;
			FP k2y = vy + (FP)0.5 * dt * k1y;
			FP k2z = vz + (FP)0.5 * dt * k1z;
			
			FP k3x = vx + (FP)0.5 * dt * k2x;
			FP k3y = vy + (FP)0.5 * dt * k2y;
			FP k3z = vz + (FP)0.5 * dt * k2z;
			
			FP k4x = vx + dt * k3x;
			FP k4y = vy + dt * k3y;
			FP k4z = vz + dt * k3z;
			
			FP dx = coeffR * (k1x + (FP)2 * k2x + (FP)2 * k3x + k4x);
			FP dy = coeffR * (k1y + (FP)2 * k2y + (FP)2 * k3y + k4y);
			FP dz = coeffR * (k1z + (FP)2 * k2z + (FP)2 * k3z + k4z);
			
			rxPtr[i] += dx;
			ryPtr[i] += dy;
			rzPtr[i] += dz;
			
			// compute new v
				
			k1x = ELECTRON_CHARGE * (E.x + (vy * B.z - vz * B.y) * invLightVelocity);
			k1y = ELECTRON_CHARGE * (E.y + (vz * B.x - vx * B.z) * invLightVelocity);
			k1z = ELECTRON_CHARGE * (E.z + (vx * B.y - vy * B.x) * invLightVelocity);
			
			FP tmpx = vx + (FP)0.5 * dt * k1x;
			FP tmpy = vy + (FP)0.5 * dt * k1y;
			FP tmpz = vz + (FP)0.5 * dt * k1z;
			
			k2x = ELECTRON_CHARGE * (E.x + (tmpy * B.z - tmpz * B.y) * invLightVelocity);
			k2y = ELECTRON_CHARGE * (E.y + (tmpz * B.x - tmpx * B.z) * invLightVelocity);
			k2z = ELECTRON_CHARGE * (E.z + (tmpx * B.y - tmpy * B.x) * invLightVelocity);
			
			tmpx = vx + (FP)0.5 * dt * k2x;
			tmpy = vy + (FP)0.5 * dt * k2y;
			tmpz = vz + (FP)0.5 * dt * k2z;
			
			k3x = ELECTRON_CHARGE * (E.x + (tmpy * B.z - tmpz * B.y) * invLightVelocity);
			k3y = ELECTRON_CHARGE * (E.y + (tmpz * B.x - tmpx * B.z) * invLightVelocity);
			k3z = ELECTRON_CHARGE * (E.z + (tmpx * B.y - tmpy * B.x) * invLightVelocity);
			
			tmpx = vx + dt * k3x;
			tmpy = vy + dt * k3y;
			tmpz = vz + dt * k3z;
			
			k4x = ELECTRON_CHARGE * (E.x + (tmpy * B.z - tmpz * B.y) * invLightVelocity);
			k4y = ELECTRON_CHARGE * (E.y + (tmpz * B.x - tmpx * B.z) * invLightVelocity);
			k4z = ELECTRON_CHARGE * (E.z + (tmpx * B.y - tmpy * B.x) * invLightVelocity);
			
			dx = coeffV * (k1x + (FP)2 * k2x + (FP)2 * k3x + k4x);
			dy = coeffV * (k1y + (FP)2 * k2y + (FP)2 * k3y + k4y);
			dz = coeffV * (k1z + (FP)2 * k2z + (FP)2 * k3z + k4z);
			
			vxPtr[i] += dx;
			vyPtr[i] += dy;
			vzPtr[i] += dz;
			
		}
	}

};

