#include <complex>
#include "fractal_ispc.h"
#include "settings.h"
using namespace std;
using namespace ispc;

complex<float> pow(complex<float> z, int n) {
    complex<float> result = 1.0f;
    for (int i = 0; i < n; ++i) result *= z;
    return result;
}

complex<float> f(complex<float> z, int n) {
    return pow(z, n) - 1.0f;
}

complex<float> fd(complex<float> z, int n) {
    return float(n) * pow(z, n-1);
}

// Each value provided in range [0, 1]
Color hsv2rgb(float h, float s, float v) {
    Color result;
    auto &[r, g, b] = result;

    int i = floor(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    switch (i % 6) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }

    return result;
}

void fractal_serial(float x0, float y0, float x1, float y1, int width, int height, Color output[], complex<float> *roots, size_t n) {
    float dx = (x1 - x0) / width;
    float dy = (y1 - y0) / height;

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; ++i) {
            int index = (j * width + i);
            float x = x0 + i * dx;
            float y = y0 + j * dy;

            complex z(x, y);

            bool root_found = false;
            for (int iteration = 0; iteration < MAX_ITERATIONS and not root_found; ++iteration) {
                z -= f(z, n) / fd(z, n);

                for (size_t rootID = 0; rootID < n; ++rootID) {
                    complex diff = z - roots[rootID];

                    if (abs(diff.real()) < TOLERANCE and abs(diff.imag()) < TOLERANCE) {
                        const float hue = float(rootID) / float(n);
                        float brightness = float(iteration+1) / float(MAX_ITERATIONS);
                        brightness = 1.0f - brightness;
                        output[index] = hsv2rgb(hue, 1.0f, brightness);
                        root_found = true;
                        break;
                    }
                }
            }
        }
    }
}

