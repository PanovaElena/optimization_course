#pragma once
#include <vector>
#include <sdlt/sdlt.h>
#include "Vector3.h"


struct Particle {

	Vector3 r, v;

	Particle() {}
	Particle(Vector3 r, Vector3 v) :r(r), v(v) {}

};

SDLT_PRIMITIVE(Particle, r, v)


class ParticleEnsemble {

	sdlt::soa1d_container<Particle> ensembleSLTD;  // SoA

public:

	ParticleEnsemble(const std::vector<Particle>& particles) :
		ensembleSLTD(particles.size())
	{
		auto ensemble = ensembleSLTD.access();
		for (int i = 0; i < particles.size(); i++)
			ensemble[i] = particles[i];
	}
	
	auto getAccessor() { return ensembleSLTD.access(); }
	auto getConstAccessor() const { return ensembleSLTD.const_access(); }

	int getSize() const { return ensembleSLTD.size(); }

};
