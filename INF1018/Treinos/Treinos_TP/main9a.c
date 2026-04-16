#include <stdio.h>

/**
 * @brief Acessa um array de inteiros em um índice específico.
 * * @param u O array de inteiros.
 * @param i O índice do elemento a ser retornado.
 * @return O valor do inteiro no índice i.
 */
int acerta(int u[], int i);

/**
 * @brief Função principal para demonstrar o uso da função acerta.
 */
int main()
{
    // 1. Criar os dados necessários para chamar a função: um array e um índice.
    int meu_array[5] = {10, 20, 30, 40, 50};
    int indice = 2; // Vamos acessar o elemento na posição 2 (que é o valor 30)
    int valor_retornado;

    // 2. Chamar a função 'acerta' com nosso array e índice.
    valor_retornado = acerta(meu_array, indice);

    // 3. Imprimir o resultado para verificar se funcionou.
    printf("O valor no array no índice %d é: %d\n", indice, valor_retornado);

    // --- Outro exemplo ---
    indice = 4; // Agora acessando o último elemento
    valor_retornado = acerta(meu_array, indice);
    printf("O valor no array no índice %d é: %d\n", indice, valor_retornado);

    return 0; // Indica que o programa terminou com sucesso
}