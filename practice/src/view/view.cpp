#include <windows.h>
#include <iostream>
#include <cmath>
#include <string>

#include "Model.h"


const FP E0 = -0.005;
const FP B0 = 1.0;
const Vector3 E(0, E0, 0);
const Vector3 B(0, 0, B0);

const FP V0 = 1e-2 * LIGHT_VELOCITY;

const FP TIME_STEP = 1e-10;
const int ITERATION_NUMBER = 20000;

const FP MIN_X = -400, MAX_X = 400, MIN_Y = -280, MAX_Y = 280;


void drawModel(Model& model, FP timeStep, int nIter) {
    const int width = 1280, height = 720;
    int shiftX = width * 3 / 5, shiftY = height / 2;

    std::vector<DWORD> pixelBuffer(width * height, 0x00000000);

    DWORD colors[] = { 0x00FFFFFF, 0x0000FF00, 0x000000FF };

    for (int iter = 0; iter < nIter; iter++) {
        for (int i = 0; i < model.getParticles().getSize(); i++) {
            Vector3 r = model.getParticles().getR(i);
            int xPixel = (int)(r.x / (MAX_X - MIN_X) * width) + shiftX;
            int yPixel = (int)(r.y / (MAX_Y - MIN_Y) * height) + shiftY;
            if (xPixel >= 0 && xPixel < width && yPixel >= 0 && yPixel < height) {
                pixelBuffer[yPixel * width + xPixel] = colors[i];
            }
        }
        model.update(timeStep);
    }

    HWND hwnd = CreateWindowExA(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        "STATIC", "Simulation Results (PRESS ENTER TO CLOSE!!!)",
        WS_POPUP | WS_CAPTION | WS_BORDER | WS_VISIBLE,
        200, 100, width + 2, height + 25,
        NULL, NULL, NULL, NULL
    );

    EnableWindow(hwnd, FALSE);

    HDC hdc = GetDC(hwnd);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    StretchDIBits(hdc, 0, 0, width, height, 0, 0, width, height, pixelBuffer.data(), &bmi, DIB_RGB_COLORS, SRCCOPY);

    std::cin.get();

    ReleaseDC(hwnd, hdc);
    DestroyWindow(hwnd);
}


int main()
{
    std::vector<Particle> particles;
    particles.push_back(Particle(Vector3(0, 0, 0), Vector3(0, 0, 0)));
    particles.push_back(Particle(Vector3(0, 0, 0), Vector3(-3 * V0, -3 * V0, 0)));
    particles.push_back(Particle(Vector3(0, 0, 0), Vector3(V0, 0, 0)));

    Model model(E, B, particles);

    drawModel(model, TIME_STEP, ITERATION_NUMBER);

    return 0;
}