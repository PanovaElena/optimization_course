#include <iostream>
#include <cmath>
#include <chrono>
#include <utility>

#include "Model.h"


const FP E0 = -0.005;
const FP B0 = 1.0;
const Vector3 E(0, E0, 0);
const Vector3 B(0, 0, B0);

const FP V0 = 1e-2 * LIGHT_VELOCITY;
const Vector3 V(V0, 0, 0);
const Vector3 R(0, 0, 0);

int PARTICLE_NUMBER = 1048576;  // 2^20

const FP TIME_STEP = 1e-10;
const int ITERATION_NUMBER = 256;


std::pair<Vector3, Vector3> getAnalyticalSolution(FP time) {
    FP omega = ELECTRON_CHARGE * B0 / (ELECTRON_MASS * LIGHT_VELOCITY);
    FP b = -E0 / B0 * LIGHT_VELOCITY;
    FP a = V0 + b;
    return std::make_pair(
        Vector3(  // analytical r
            a / omega * sin(omega * time) - b * time,
            a / omega * (cos(omega * time) - 1),
            0.0
        ),
        Vector3(  // analytical v
            a * cos(omega * time) - b,
            -a * sin(omega * time),
            0.0
        )
    );
}


bool checkResult(const ParticleEnsemble& particles, FP time) {
    FP eps = 1e-4*LIGHT_VELOCITY;
    for (int i = 0; i < particles.getSize(); i++) {
        std::pair<Vector3, Vector3> analyticalSolution = getAnalyticalSolution(time);
		if ((analyticalSolution.first - particles.getR(i)).getNorm() > eps ||  // r
            (analyticalSolution.second - particles.getV(i)).getNorm() > eps)   // v
            return false;
    }
    return true;
}


int main()
{
    std::vector<Particle> particles(PARTICLE_NUMBER);
    for (int i = 0; i < PARTICLE_NUMBER; i++)
        particles[i] = Particle(R, V);

    Model model(E, B, particles);

    auto t0 = std::chrono::steady_clock::now();
    for (int iter = 0; iter < ITERATION_NUMBER; iter++)
        model.update(TIME_STEP);
    auto t1 = std::chrono::steady_clock::now();
    FP time = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
	
    auto particle = model.getParticles().getParticle(0);
	std::cout << particle.r.x << " " << particle.r.y << " " << particle.r.z << "; " 
		<< particle.v.x << " " << particle.v.y << " " << particle.v.z << std::endl;

    if (!checkResult(model.getParticles(), ITERATION_NUMBER * TIME_STEP))
        std::cout << "ERROR: WRONG RESULT!!!" << std::endl;
    else {
        std::cout << "CORRECT RESULT" << std::endl;
		std::cout << "TIME IS " << time << " ms" << std::endl;
    }

    return 0;
}

