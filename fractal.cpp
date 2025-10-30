#include <complex>
#include <cmath>
#include "timing.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string.h>
using namespace std;
// using namespace ispc;

struct color {
    float red;
    float green;
    float blue;
};
constexpr color WHITE {1, 1, 1};
constexpr color BLACK {0, 0, 0};
constexpr color RED {1, 0, 0};
constexpr color GREEN {0, 1, 0};
constexpr color BLUE {0, 0, 1};

constexpr int MAX_ITERATIONS = 30;
constexpr float TOLERANCE = 0.000001f;

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

vector<complex<float>> rootsOfF(int n) {
    std::vector<std::complex<float>> roots;
    roots.reserve(n);
    for (int k = 0; k < n; ++k) {
        float angle = 2.0f * M_PI * k / n;
        std::complex<float> root = polar(1.0f, angle);
        roots.push_back(root);
        cout << root << endl;
    }
    return roots;
}

// Each value provided in range [0, 1]
color hsv2rgb(float h, float s, float v) {
    color result;
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

void fractal_serial(float x0, float y0, float x1, float y1, int width, int height, color output[]) {
    const int n = 5;

    auto roots = rootsOfF(n);

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

                for (size_t rootID = 0; rootID < roots.size(); ++rootID) {
                    complex diff = z - roots[rootID];

                    if (abs(diff.real()) < TOLERANCE and abs(diff.imag()) < TOLERANCE) {
                        const float hue = float(rootID) / float(roots.size());
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

/* Write a PPM image file with the image */
static void writePPM(color *buf, int width, int height, const char *fn) {
    FILE *fp = fopen(fn, "wb");
    if (!fp) {
        printf("Couldn't open a file '%s'\n", fn);
        exit(-1);
    }
    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", width, height);
    fprintf(fp, "255\n");
    for (int i = 0; i < width * height; ++i) {
        char r = clamp(buf[i].red * 255.f, 0.0f, 255.0f);
        char g = clamp(buf[i].green * 255.f, 0.0f, 255.0f);
        char b = clamp(buf[i].blue * 255.f, 0.0f, 255.0f);

        fputc(r, fp);
        fputc(g, fp);
        fputc(b, fp);
    }
    fclose(fp);
}

int main(int argc, char *argv[]) {
    static unsigned int test_iterations[] = {3, 1};
    // unsigned int width = 768;
    // unsigned int height = 768;
    unsigned int width = 400;
    unsigned int height = 400;
    float x0 = -2.5f;
    float x1 = 1;
    float y0 = -2;
    float y1 = 1;

    if (argc > 1) {
        if (strncmp(argv[1], "--scale=", 8) == 0) {
            float scale = atof(argv[1] + 8);
            width *= scale;
            height *= scale;
        }
    }
    if ((argc == 3) || (argc == 4)) {
        for (int i = 0; i < 2; i++) {
            test_iterations[i] = atoi(argv[argc - 2 + i]);
        }
    }
    color buf[width * height];

    // //
    // // Compute the image using the ispc implementation; report the minimum
    // // time of three runs.
    // //
    double minISPC = 1e30;
    // for (unsigned int i = 0; i < test_iterations[0]; ++i) {
    //     reset_and_start_timer();
    //     noise_ispc(x0, y0, x1, y1, width, height, buf);
    //     double dt = get_elapsed_mcycles();
    //     printf("@time of ISPC run:\t\t\t[%.3f] million cycles\n", dt);
    //     minISPC = std::min(minISPC, dt);
    // }
    //
    // printf("[fractal ispc]:\t\t\t[%.3f] million cycles\n", minISPC);
    // writePPM(buf, width, height, "fractal-ispc.ppm");
    //
    // // Clear out the buffer
    // for (unsigned int i = 0; i < width * height; ++i)
    //     buf[i] = 0;

    //
    // And run the serial implementation 3 times, again reporting the
    // minimum time.
    //
    double minSerial = 1e30;
    for (unsigned int i = 0; i < test_iterations[1]; ++i) {
        reset_and_start_timer();
        fractal_serial(x0, y0, x1, y1, width, height, buf);
        double dt = get_elapsed_mcycles();
        printf("@time of serial run:\t\t\t[%.3f] million cycles\n", dt);
        minSerial = std::min(minSerial, dt);
    }

    printf("[fractal serial]:\t\t\t[%.3f] million cycles\n", minSerial);
    writePPM(buf, width, height, "fractal-serial.ppm");

    printf("\t\t\t\t(%.2fx speedup from ISPC)\n", minSerial / minISPC);

    return 0;
}
