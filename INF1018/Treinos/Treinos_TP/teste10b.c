#include <stdio.h>

// Sua função
double diff(double a, double b, int n);

// Função principal (main) para testar
int main()
{
    // 1. Defina os valores de entrada para o teste
    double valor_a = 7.5;
    double valor_b = 3.0;
    int numero_n = 10;
    
    // 2. Chame a função 'diff' com os valores de teste
    double resultado = diff(valor_a, valor_b, numero_n);
    
    // 3. Imprima os valores de entrada e o resultado
    printf("Testando a função diff com a = %.2f, b = %.2f, e n = %d\n", valor_a, valor_b, numero_n);
    printf("O resultado é: %.2f\n", resultado);
    
    // O valor esperado é 10 * (7.5 - 3.0) = 10 * 4.5 = 45.0
    
    return 0; // Indica que o programa terminou com sucesso
}