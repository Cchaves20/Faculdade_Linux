#include <stdio.h>
#include <stdlib.h>

// Definição da estrutura
struct S
{
    int v;
    struct S *prox;
};

// --- Funções fornecidas ---

// O array 'norma' precisa ser definido globalmente para que 'boo' o encontre.
int norma[] = {100, 101, 102, 103, 104, 105, 106, 107, 108, 109};

int acerta(int u[], int i)
{
    return u[i];
}

int boo(struct S *s, int n);

// --- Função principal para testar ---

int main()
{
    // 1. Criar uma lista encadeada com 3 nós
    struct S *head = (struct S*) malloc(sizeof(struct S));
    struct S *node2 = (struct S*) malloc(sizeof(struct S));
    struct S *node3 = (struct S*) malloc(sizeof(struct S));

    // Verifica se a alocação de memória foi bem-sucedida
    if (!head || !node2 || !node3) {
        printf("Erro ao alocar memória.\n");
        return 1;
    }

    // Inicializa e liga os nós
    head->v = 1;
    head->prox = node2;

    node2->v = 2;
    node2->prox = node3;

    node3->v = 3;
    node3->prox = NULL; // O último nó aponta para NULL

    // 2. Imprime os valores da lista ANTES de chamar boo
    printf("--- Lista Original ---\n");
    struct S *current = head;
    while (current)
    {
        printf("Nó com valor: %d\n", current->v);
        current = current->prox;
    }

    // 3. Chama a função boo com a lista e n = 2
    int n_valor = 2;
    printf("\nChamando boo(head, %d)...\n", n_valor);
    int resultado_acum = boo(head, n_valor);

    // 4. Imprime o resultado e os valores da lista DEPOIS de chamar boo
    printf("\n--- Resultados ---\n");
    printf("Valor retornado por boo (acum): %d\n", resultado_acum);

    printf("\n--- Lista Modificada ---\n");
    current = head;
    while (current)
    {
        printf("Nó agora com valor: %d\n", current->v);
        current = current->prox;
    }

    // Libera a memória alocada para evitar memory leaks
    free(head);
    free(node2);
    free(node3);

    return 0;
}