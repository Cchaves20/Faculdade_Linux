#include <stdio.h>

// Protótipo da função 'calcula'
int calcula(float f);

// Implementação da função 'boba'
double boba(float val);

/*
 * Implementação de exemplo para a função 'calcula'.
 * Esta função precisa retornar um valor inteiro entre 0 e 4
 * para ser usada como índice para o array 'd'.
 * Esta implementação simples sempre retorna 0.
*/
int calcula(float f)
{
    // Lógica de exemplo: pode ser alterada conforme a necessidade.
    // O importante é retornar um índice válido (0 a 4).
    if (f > 10.0) {
        return 1;
    }
    return 0;
}

// Função principal 'main'
int main()
{
    float valor_exemplo = 5.5f;
    double resultado;

    // Chama a função 'boba' com um valor de exemplo
    resultado = boba(valor_exemplo);

    // Imprime o resultado retornado
    printf("O valor retornado por boba() é: %f\n", resultado);

    return 0;
}