/* Caio Faria Brito 2410162 3WB */
/* Luiz Henrique Machado 2411819 3WB */

#include <stdio.h>
#include "bigint.h"

void print_bigint(const char* label, BigInt num) {
    printf("%s", label);
    for (int i = 15; i >= 0; i--) {
        printf("%02X", num[i]);
    }
    printf("\n");
}

void test_big_val() {
    BigInt res;
    printf("\n--- Testando big_val ---\n");

    printf("  Teste 1: val = 0\n");
    big_val(res, 0);
    print_bigint("  Resultado: ", res);

    printf("  Teste 2: val = 2000000\n");
    big_val(res, 2000000);
    print_bigint("  Resultado: ", res);

    printf("  Teste 3: val = -1\n");
    big_val(res, -1);
    print_bigint("  Resultado: ", res);
}

void test_big_comp2() {
    BigInt a, res;
    printf("\n--- Testando big_comp2 (res = -a) ---\n");

    printf("  Teste 1: -(1)\n");
    big_val(a, 1);
    big_comp2(res, a);
    print_bigint("  Resultado: ", res);

    printf("  Teste 2: -(-1)\n");
    big_val(a, -1);
    big_comp2(res, a);
    print_bigint("  Resultado: ", res);

    printf("  Teste 3: -(0)\n");
    big_val(a, 0);
    big_comp2(res, a);
    print_bigint("  Resultado: ", res);
}

void test_big_sum() {
    BigInt a, b, res;
    printf("\n--- Testando big_sum (res = a + b) ---\n");

    printf("  Teste 1: 100 + 25\n");
    big_val(a, 100);
    big_val(b, 25);
    big_sum(res, a, b);
    print_bigint("  Resultado: ", res);

    printf("  Teste 2: 100 + (-25)\n");
    big_val(a, 100);
    big_val(b, -25);
    big_sum(res, a, b);
    print_bigint("  Resultado: ", res);
    
    printf("  Teste 3: (-100) + (-25)\n");
    big_val(a, -100);
    big_val(b, -25);
    big_sum(res, a, b);
    print_bigint("  Resultado: ", res);
}

void test_big_sub() {
    BigInt a, b, res;
    printf("\n--- Testando big_sub (res = a - b) ---\n");

    printf("  Teste 1: 100 - 25\n");
    big_val(a, 100);
    big_val(b, 25);
    big_sub(res, a, b);
    print_bigint("  Resultado: ", res);
    
    printf("  Teste 2: 25 - 100\n");
    big_val(a, 25);
    big_val(b, 100);
    big_sub(res, a, b);
    print_bigint("  Resultado: ", res);
    
    printf("  Teste 3: (-50) - 50\n");
    big_val(a, -50);
    big_val(b, 50);
    big_sub(res, a, b);
    print_bigint("  Resultado: ", res);
}

void test_big_mul() {
    BigInt a, b, res;
    printf("\n--- Testando big_mul (res = a * b) ---\n");

    printf("  Teste 1: 1000 * 1000\n");
    big_val(a, 1000);
    big_val(b, 1000);
    big_mul(res, a, b);
    print_bigint("  Resultado: ", res);
    
    printf("  Teste 2: 100 * (-10)\n");
    big_val(a, 100);
    big_val(b, -10);
    big_mul(res, a, b);
    print_bigint("  Resultado: ", res);

    printf("  Teste 3: (-50) * (-20)\n");
    big_val(a, -50);
    big_val(b, -20);
    big_mul(res, a, b);
    print_bigint("  Resultado: ", res);
}

void test_big_shl() {
    BigInt a, res;
    printf("\n--- Testando big_shl (res = a << n) ---\n");

    printf("  Teste 1: 1 << 8\n");
    big_val(a, 1);
    big_shl(res, a, 8);
    print_bigint("  Resultado: ", res);

    printf("  Teste 2: 15 << 2\n");
    big_val(a, 15);
    big_shl(res, a, 2);
    print_bigint("  Resultado: ", res);

    printf("  Teste 3: (-1) << 4\n");
    big_val(a, -1);
    big_shl(res, a, 4);
    print_bigint("  Resultado: ", res);
}

void test_big_shr() {
    BigInt a, res;
    printf("\n--- Testando big_shr (res = a >> n) (Lógico) ---\n");

    printf("  Teste 1: 256 >> 8\n");
    big_val(a, 256);
    big_shr(res, a, 8);
    print_bigint("  Resultado: ", res);

    printf("  Teste 2: 15 >> 1\n");
    big_val(a, 15);
    big_shr(res, a, 1);
    print_bigint("  Resultado: ", res);

    printf("  Teste 3: (-1) >> 1\n");
    big_val(a, -1);
    big_shr(res, a, 1);
    print_bigint("  Resultado: ", res);
}

void test_big_sar() {
    BigInt a, res;
    printf("\n--- Testando big_sar (res = a >> n) (Aritmético) ---\n");

    printf("  Teste 1: 256 >> 8\n");
    big_val(a, 256);
    big_sar(res, a, 8);
    print_bigint("  Resultado: ", res);
    
    printf("  Teste 2: (-100) >> 2\n");
    big_val(a, -100);
    big_sar(res, a, 2);
    print_bigint("  Resultado: ", res);

    printf("  Teste 3: (-1) >> 1\n");
    big_val(a, -1);
    big_sar(res, a, 1);
    print_bigint("  Resultado: ", res);
}

int main() {
    test_big_val();
    test_big_comp2();
    test_big_sum();
    test_big_sub();
    test_big_mul();
    test_big_shl();
    test_big_shr();
    test_big_sar();
    return 0;
}
