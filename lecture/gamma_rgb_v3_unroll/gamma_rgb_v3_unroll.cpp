#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <chrono>

#include "bmp_reader.h"


using IntensityType = uint8_t;  // 1 byte
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
	for (int i = 0; i < height; i++)
		#pragma ivdep
		for (int j = 0; j < width; j++) {
			int index = N_CHANNELS * (i * width + j);
			result[index] = correctPixelIntensity(pixels[index]);
			result[index + 1] = correctPixelIntensity(pixels[index + 1]);
			result[index + 2] = correctPixelIntensity(pixels[index + 2]);
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
	
	   auto t0 = std::chrono::steady_clock::now();
	   nonlinearCorrection(height, width, pixels.data(), resPixels.data());
	   auto t1 = std::chrono::steady_clock::now();
	   float time = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
	
	   std::cout << "Time is " << time/1e6 << " sec" << std::endl;
	reader.setRGBPixels(resPixels);
	
	reader.save(std::string(argv[0]) + "_result.bmp");
		
	return 0;
}