#include <windows.h>
#include <iostream>
#include <cmath>

#include "Model.h"


const FP E0 = -0.005;
const FP B0 = 1.0;
const Vector3 E(0, E0, 0);
const Vector3 B(0, 0, B0);

FP V0 = 1e-2 * LIGHT_VELOCITY;

const FP TIME_STEP = 1e-10;
const int ITERATION_NUMBER = 20000;

FP MIN_X = -400, MAX_X = 400, MIN_Y = -280, MAX_Y = 280;


void drawModel(Model& model, FP timeStep, int nIter) {
    HWND myconsole = GetConsoleWindow();
    HDC mydc = GetDC(myconsole);

    int windowSizeX = 1280, windowSizeY = 720;
    MoveWindow(myconsole, 0, 0, windowSizeX, windowSizeY, true);
    Sleep(1000);

    int windowShiftX = windowSizeX * 3 / 5, windowShiftY = windowSizeY / 2;
    COLORREF colors[] = { RGB(255, 255, 255), RGB(0, 255, 0), RGB(0, 0, 255) };

    for (int iter = 0; iter < nIter; iter++) {
        for (int i = 0; i < model.getParticles().getSize(); i++) {
            Vector3 r = model.getParticles().getR(i);
            int xPixel = (int)(r.x / (MAX_X - MIN_X) * windowSizeX) + windowShiftX;
            int yPixel = (int)(r.y / (MAX_Y - MIN_Y) * windowSizeY) + windowShiftY;
            
            SetPixel(mydc, xPixel, yPixel, colors[i]);
        }

        model.update(timeStep);
    }

    std::cin.ignore();
    ReleaseDC(myconsole, mydc);
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