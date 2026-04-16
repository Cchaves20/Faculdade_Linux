#include <stdio.h>

struct exam { 
    float val; 
    int   tipo; 
};

/* Stub de inv: simplesmente retorna val (pode trocar por outra lógica) */
double inv(double val) {
    return val;
}

float boo(double d, float bias);

int main(void) {
    double  d    = 4.2;
    float   bias = 1.5f;
    float   resultado;

    resultado = boo(d, bias);

    printf("Chamando boo(%.2f, %.2f) → %.2f\n", d, bias, resultado);
    return 0;
}
