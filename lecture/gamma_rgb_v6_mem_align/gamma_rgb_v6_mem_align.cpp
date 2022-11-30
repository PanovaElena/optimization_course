#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <chrono>
#include <windows.h>

#include "bmp_reader.h"


using IntensityType = float;
const IntensityType MIN_INTENSITY = 0, MAX_INTENSITY = 255;
const int N_CHANNELS = 3;  // RGB
const float GAMMA = 0.45f;
const float FACTOR = std::pow(MAX_INTENSITY, 1.0f - GAMMA);

float correctPixelIntensity(float x)
{
	float res = FACTOR * std::pow(x, GAMMA);
	if (res < MIN_INTENSITY) res = MIN_INTENSITY;
	if (res > MAX_INTENSITY) res = MAX_INTENSITY;
	return res;
}

__declspec(noinline) void nonlinearCorrection(int height, int width,
	IntensityType* pixels, IntensityType* result)
{
	#pragma ivdep
	#pragma vector aligned
	for (int i = 0; i < height * width * N_CHANNELS; i++) {
		result[i] = correctPixelIntensity(pixels[i]);
	}
}


int main(int argc, char** argv) {

	BMPReader reader;
	if (!reader.open("photo.bmp")) {
		std::cout << "Error when reading" << std::endl;
		return 0;
	}

	std::vector<IntensityType> pixels = reader.getRGBPixels<IntensityType>();
	const int height = reader.getHeight(), width = reader.getWidth();
	std::vector<IntensityType> resPixels(pixels.size());
	
	IntensityType* pixels_ = (IntensityType*)_aligned_malloc(pixels.size() * sizeof(IntensityType), 64);
	IntensityType* resPixels_ = (IntensityType*)_aligned_malloc(pixels.size() * sizeof(IntensityType), 64);
	for (int i = 0; i < pixels.size(); i++)
		pixels_[i] = pixels[i];

	auto t0 = std::chrono::steady_clock::now();
	nonlinearCorrection(height, width, pixels.data(), resPixels.data());
	auto t1 = std::chrono::steady_clock::now();
	float time = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
	std::cout << "Time is " << time/1e6 << " sec" << std::endl;
	
	for (int i = 0; i < pixels.size(); i++)
		resPixels[i] = resPixels_[i];
	
	_aligned_free(resPixels_);
	_aligned_free(pixels_);
	
	reader.setRGBPixels(resPixels);
		
	reader.save(std::string(argv[0]) + "result.bmp");
		
	return 0;
}