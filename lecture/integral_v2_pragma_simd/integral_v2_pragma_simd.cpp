#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <chrono>
#include <omp.h>
#include <algorithm>

#include "bmp_reader.h"


using IntensityType = float;
const IntensityType MIN_INTENSITY = 0, MAX_INTENSITY = 255;
const int N_CHANNELS = 3;  // RGB

__declspec(noinline) void integral(int height, int width,
	float* pixels)
{
	for (int i = 0; i < height-1; i++) {
		#pragma omp simd
		for (int j = 0; j < width-1; j++)
			#pragma unroll
			for (int k = 0; k < N_CHANNELS; k++) {
				int index_i0_j0 = (i * width + j) * N_CHANNELS + k;
				int index_i0_j1 = (i * width + (j+1)) * N_CHANNELS + k;
				int index_i1_j0 = ((i+1) * width + j) * N_CHANNELS + k;
				int index_i1_j1 = ((i+1) * width + (j+1)) * N_CHANNELS + k;
				pixels[index_i1_j1] = pixels[index_i1_j1] - pixels[index_i0_j0] +
					pixels[index_i1_j0] + pixels[index_i0_j1];
			}
	}
}


int main(int argc, char** argv) {

	BMPReader reader;
	if (!reader.open("smile.bmp")) {
		std::cout << "Error when reading" << std::endl;
		return 0;
	}

	std::vector<IntensityType> pixels = reader.getRGBPixels<IntensityType>();
	const int height = reader.getHeight(), width = reader.getWidth();

	integral(height, width, pixels.data());
	
	float maxValue = 0.0f;
	for (int i = 0; i < height * width * N_CHANNELS; i++)
		if (maxValue < pixels[i]) maxValue = pixels[i];
	for (int i = 0; i < height * width * N_CHANNELS; i++)
		pixels[i] *= MAX_INTENSITY/maxValue;
	reader.saveRGB(std::string(argv[0]) + "_result.bmp", pixels);
		
	return 0;
}
