#include <cstdio>
#include <algorithm>
#include <complex>
#include <cmath>
#include <chrono>

#include "fractal_ispc.h"
#include "settings.h"
using namespace std;
using namespace ispc;

complex<float>* rootsOfF(int n) {
    complex<float> *roots = new complex<float>[n];
    for (int k = 0; k < n; ++k) {
        float angle = 2.0f * M_PI * k / n;
        std::complex<float> root = polar(1.0f, angle);
        roots[k] = root;
    }
    return roots;
}

static void writePPM(Color *buf, int width, int height, const char *fn) {
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

int main(int argc, char **argv) {
    int n = 1;
    if (argc <= 1) {
        fprintf(stderr,
        "Usage: ./fractal.e N\n"
        "\tN - positive integer defining the function f(z) = z^N - 1\n");
        return 1;
    } else {
        n = atoi(argv[1]);
    }

    Color *buf = new Color[WIDTH * HEIGHT];
    auto roots = rootsOfF(n);

    printf("Newton Fractal generator.\n");
    printf("f(z) = z^%d - 1\n", n);
    printf("WIDTH = %d\n", WIDTH);
    printf("WIDTH = %d\n", HEIGHT);
    printf("MAX_ITERATIONS = %d\n", MAX_ITERATIONS);
    printf("Started the parallel generation via ISPC...\n");
    auto start = std::chrono::steady_clock::now();
    fractal_ispc(X0, Y0, X1, Y1, WIDTH, HEIGHT, buf, reinterpret_cast<Complex*>(roots), n);
    auto end = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    printf("Generation finished in %lld ms. Writing to file " OUTPUT_FILE "\n", static_cast<long long>(elapsed_ms));
    writePPM(buf, WIDTH, HEIGHT, OUTPUT_FILE);

    delete[] buf;
    delete[] roots;
    return 0;
}
