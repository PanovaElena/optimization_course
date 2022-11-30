#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <tuple>

#include "bmp_reader.h"


using IntensityType = float;
const IntensityType MIN_INTENSITY = 0, MAX_INTENSITY = 255;
const int N_CHANNELS = 3;  // RGB

__declspec(noinline) std::tuple<float, float, float> average(int height, int width,
	IntensityType* pixels)
{	
	float avgR = 0.0f, avgG = 0.0f, avgB = 0.0f;
	
	#pragma novector
	for (int i = 0; i < height * width; i++) {
		avgR += pixels[N_CHANNELS * i];
		avgG += pixels[N_CHANNELS * i + 1];
		avgB += pixels[N_CHANNELS * i + 2];
	}
	
	avgR /= height * width;
	avgG /= height * width;
	avgB /= height * width;
	
	return std::make_tuple(avgR, avgG, avgB);
}


int main(int argc, char** argv) {

	BMPReader reader;
	if (!reader.open("photo.bmp")) {
		std::cout << "Error when reading" << std::endl;
		return 0;
	}

	std::vector<IntensityType> pixels = reader.getRGBPixels<IntensityType>();
	const int height = reader.getHeight(), width = reader.getWidth();

	auto t0 = std::chrono::steady_clock::now();
	auto avg = average(height, width, pixels.data());
	auto t1 = std::chrono::steady_clock::now();
	float time = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
	
	std::cout << "Time is " << time/1e6 << " sec" << std::endl;		
	std::cout << "Avg RGB is " << std::get<0>(avg) << ", " << std::get<1>(avg) << ", " <<
		std::get<2>(avg) << std::endl;
	
	return 0;
}
