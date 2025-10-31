#include "complex.isph"
#include "settings.h"

struct Color {
    float red;
    float green;
    float blue;
};

// Each value provided in range [0, 1]
Color hsv2rgb(float h, float s, float v) {
    float r, g, b;

    int i = floor(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    #pragma ignore warning(perf)
    switch (i % 6) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }

    Color result = {r, g, b};
    return result;
}

Complex f(Complex z, uniform int n) {
    Complex one = {1.0f, 0.0f};
    return powUniformExp(z, n) - one;
}

Complex fd(Complex z, uniform int n) {
    Complex scalar = {(float)n, 0.0f};
    return scalar * powUniformExp(z, n-1);
}


task void fractal_task(uniform float x0, uniform float y0,   
        uniform float x1, uniform float y1,  
        uniform int width, uniform int height,  
        uniform Color output[],   
        uniform Complex roots[],   
        uniform size_t n) {  

    uniform int num_tasks = taskCount;
    uniform int rows_per_task = height / num_tasks;  
    int j_start = taskIndex * rows_per_task;
    int j_end = (taskIndex + 1) * rows_per_task;

    float dx = (x1 - x0) / width;  
    float dy = (y1 - y0) / height;  

    for (int j = j_start; j < j_end; j++) {  
        for (uniform int i = 0; i < width; i += programCount) {  
            int index = (j * width + i + programIndex);  
            float x = x0 + (i + programIndex) * dx;  
            float y = y0 + j * dy;  

            Complex z = {x, y};  

            bool root_found = false;  
            for (int iteration = 0; iteration < MAX_ITERATIONS && !root_found; ++iteration) {  
                z = z - f(z, n) / fd(z, n);  

                for (uniform size_t rootID = 0; rootID < n; ++rootID) {  
                    Complex diff = z - roots[rootID];  

                    if (abs(diff.re) < TOLERANCE && abs(diff.im) < TOLERANCE) {  
                        const float hue = (float)rootID / (float)n;  
                        float brightness = (float)(iteration+1) / (float)MAX_ITERATIONS;  
                        brightness = 1.0f - brightness;  
                        #pragma ignore warning(perf)
                        output[index] = hsv2rgb(hue, 1.0f, brightness);  
                        root_found = true;  
                        break;  
                    }  
                }  
            }  
        }  
    }  
}  

export void fractal_ispc(uniform float x0, uniform float y0,   
        uniform float x1, uniform float y1,  
        uniform int width, uniform int height,  
        uniform Color output[],   
        uniform Complex roots[],   
        uniform size_t n) {  

    launch[num_cores()] fractal_task(x0, y0, x1, y1, width, height, output, roots, n);
}
