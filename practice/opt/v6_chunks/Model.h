#pragma once
#include "Particle.h"


const FP ELECTRON_MASS = 9.10938215e-28;
const FP ELECTRON_CHARGE = -4.80320427e-10;
const FP LIGHT_VELOCITY = 29979245800.0;  // CGS


#if defined(__CHUNK_SIZE_16__)
	#define CHUNK_SIZE 16
#elif defined(__CHUNK_SIZE_32__)
	#define CHUNK_SIZE 32
#elif defined(__CHUNK_SIZE_64__)
	#define CHUNK_SIZE 64
#endif



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
		const int chunkSize = CHUNK_SIZE;
		const int nChunks = nParticles / chunkSize;  // we consider that nParticles % chunkSize == 0
		
		const FP coeffR = dt / (FP)6;
		const FP coeffV = coeffR / ELECTRON_MASS;
		const FP invLightVelocity = 1.0 / LIGHT_VELOCITY;

		for (int chunk = 0; chunk < nChunks; chunk++) {

			FP* rxPtr = particles.getRxPtr() + chunk*chunkSize,
				* ryPtr = particles.getRyPtr() + chunk * chunkSize,
				* rzPtr = particles.getRzPtr() + chunk * chunkSize;
			FP* vxPtr = particles.getVxPtr() + chunk * chunkSize,
				* vyPtr = particles.getVyPtr() + chunk * chunkSize,
				* vzPtr = particles.getVzPtr() + chunk * chunkSize;
			
			FP k1x[chunkSize], k1y[chunkSize], k1z[chunkSize];
			FP k2x[chunkSize], k2y[chunkSize], k2z[chunkSize];
			FP k3x[chunkSize], k3y[chunkSize], k3z[chunkSize];
			FP k4x[chunkSize], k4y[chunkSize], k4z[chunkSize];

			// compute new r

#ifdef __NOVECTOR__
#pragma novector
#pragma distribute_point
#else
#pragma omp simd
#pragma distribute_point
#endif
			for (int i = 0; i < chunkSize; i++) {
				FP vx = vxPtr[i], vy = vyPtr[i], vz = vzPtr[i];
				
				k1x[i] = vx;
				k1y[i] = vy;
				k1z[i] = vz;
			
				k2x[i] = vx + (FP)0.5 * dt * k1x[i];
				k2y[i] = vy + (FP)0.5 * dt * k1y[i];
				k2z[i] = vz + (FP)0.5 * dt * k1z[i];
			
				k3x[i] = vx + (FP)0.5 * dt * k2x[i];
				k3y[i] = vy + (FP)0.5 * dt * k2y[i];
				k3z[i] = vz + (FP)0.5 * dt * k2z[i];
			
				k4x[i] = vx + dt * k3x[i];
				k4y[i] = vy + dt * k3y[i];
				k4z[i] = vz + dt * k3z[i];
			}

#ifdef __NOVECTOR__
#pragma novector
#pragma distribute_point
#else
#pragma omp simd
#pragma distribute_point
#endif
			for (int i = 0; i < chunkSize; i++) {			
				rxPtr[i] += coeffR * (k1x[i] + (FP)2 * k2x[i] + (FP)2 * k3x[i] + k4x[i]);
				ryPtr[i] += coeffR * (k1y[i] + (FP)2 * k2y[i] + (FP)2 * k3y[i] + k4y[i]);
				rzPtr[i] += coeffR * (k1z[i] + (FP)2 * k2z[i] + (FP)2 * k3z[i] + k4z[i]);
			}
				
			// compute new v
				
#ifdef __NOVECTOR__
#pragma novector
#pragma distribute_point
#else
#pragma omp simd
#pragma distribute_point
#endif
			for (int i = 0; i < chunkSize; i++) {
				FP vx = vxPtr[i], vy = vyPtr[i], vz = vzPtr[i];
				
				k1x[i] = ELECTRON_CHARGE * (E.x + (vy * B.z - vz * B.y) * invLightVelocity);
				k1y[i] = ELECTRON_CHARGE * (E.y + (vz * B.x - vx * B.z) * invLightVelocity);
				k1z[i] = ELECTRON_CHARGE * (E.z + (vx * B.y - vy * B.x) * invLightVelocity);
			
				FP tmpx = vx + (FP)0.5 * dt * k1x[i];
				FP tmpy = vy + (FP)0.5 * dt * k1y[i];
				FP tmpz = vz + (FP)0.5 * dt * k1z[i];

				k2x[i] = ELECTRON_CHARGE * (E.x + (tmpy * B.z - tmpz * B.y) * invLightVelocity);
				k2y[i] = ELECTRON_CHARGE * (E.y + (tmpz * B.x - tmpx * B.z) * invLightVelocity);
				k2z[i] = ELECTRON_CHARGE * (E.z + (tmpx * B.y - tmpy * B.x) * invLightVelocity);
			
				tmpx = vx + (FP)0.5 * dt * k2x[i];
				tmpy = vy + (FP)0.5 * dt * k2y[i];
				tmpz = vz + (FP)0.5 * dt * k2z[i];

				k3x[i] = ELECTRON_CHARGE * (E.x + (tmpy * B.z - tmpz * B.y) * invLightVelocity);
				k3y[i] = ELECTRON_CHARGE * (E.y + (tmpz * B.x - tmpx * B.z) * invLightVelocity);
				k3z[i] = ELECTRON_CHARGE * (E.z + (tmpx * B.y - tmpy * B.x) * invLightVelocity);
			
				tmpx = vx + dt * k3x[i];
				tmpy = vy + dt * k3y[i];
				tmpz = vz + dt * k3z[i];

				k4x[i] = ELECTRON_CHARGE * (E.x + (tmpy * B.z - tmpz * B.y) * invLightVelocity);
				k4y[i] = ELECTRON_CHARGE * (E.y + (tmpz * B.x - tmpx * B.z) * invLightVelocity);
				k4z[i] = ELECTRON_CHARGE * (E.z + (tmpx * B.y - tmpy * B.x) * invLightVelocity);
			}
			
#ifdef __NOVECTOR__
#pragma novector
#pragma distribute_point
#else
#pragma omp simd
#pragma distribute_point
#endif
			for (int i = 0; i < chunkSize; i++) {	
				vxPtr[i] += coeffV * (k1x[i] + (FP)2 * k2x[i] + (FP)2 * k3x[i] + k4x[i]);
				vyPtr[i] += coeffV * (k1y[i] + (FP)2 * k2y[i] + (FP)2 * k3y[i] + k4y[i]);
				vzPtr[i] += coeffV * (k1z[i] + (FP)2 * k2z[i] + (FP)2 * k3z[i] + k4z[i]);
			}
		
		}
	}

};