
#include "complex.isph"

Complex operator+(Complex a, Complex b) {  
    Complex result;  
    result.re = a.re + b.re;  
    result.im = a.im + b.im;  
    return result;  
}  

Complex operator-(Complex a, Complex b) {  
    Complex result;  
    result.re = a.re - b.re;  
    result.im = a.im - b.im;  
    return result;  
}  

Complex operator*(Complex a, Complex b) {  
    Complex result;  
    result.re = a.re * b.re - a.im * b.im;  
    result.im = a.re * b.im + a.im * b.re;  
    return result;  
}  

Complex operator/(Complex a, Complex b) {  
    Complex result;  
    float denominator = b.re * b.re + b.im * b.im;  
    result.re = (a.re * b.re + a.im * b.im) / denominator;  
    result.im = (a.im * b.re - a.re * b.im) / denominator;  
    return result;  
}

Complex polar(float r, float theta) {  
    Complex result;  
    result.re = r * cos(theta);  
    result.im = r * sin(theta);  
    return result;  
}

Complex powUniformExp(Complex z, uniform int n) {
    Complex result = {1.0f, 0.0f};
    for (uniform int i = 0; i < n; ++i) {
        result = result * z;
    }
    return result;
}
