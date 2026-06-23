#!/bin/bash
#
# run_all.sh - Executa o simulador para TODAS as combinacoes pedidas no
# enunciado e gera uma tabela (CSV + texto) com os resultados.
#
# Combinacoes:
#   Algoritmos    : LRU, NRU, Relogio, Otimo
#   Tamanho pagina: 4 KB e 8 KB
#   Tamanho mem.  : 1 MB, 2 MB e 4 MB
#   Arquivos .log : todos os .log no diretorio (ou os passados como argumento)
#
# Uso:
#   ./run_all.sh                 # usa todos os *.log do diretorio atual
#   ./run_all.sh arq1.log arq2.log
#
# Os arquivos .log NAO estao incluidos no kit de entrega (vide enunciado);
# copie-os para este diretorio antes de executar o script.

set -euo pipefail

SIM=./sim-virtual
ALGOS=(LRU NRU Relogio Otimo)
PAGINAS=(4 8)
MEMORIAS=(1 2 4)
SAIDA=resultados.csv

# Garante que o simulador esta compilado.
make >/dev/null

# Define a lista de arquivos .log a processar.
if [ "$#" -gt 0 ]; then
    LOGS=("$@")
else
    shopt -s nullglob
    LOGS=(*.log)
fi

if [ "${#LOGS[@]}" -eq 0 ]; then
    echo "Nenhum arquivo .log encontrado. Copie os .log para este diretorio"
    echo "ou passe-os como argumento: ./run_all.sh compilador.log ..."
    exit 1
fi

# Cabecalho da tabela.
printf "arquivo,algoritmo,pagina_KB,memoria_MB,page_faults,paginas_escritas\n" > "$SAIDA"
printf "%-16s %-9s %-9s %-10s %-12s %-16s\n" \
       "ARQUIVO" "ALGORITMO" "PAGINA_KB" "MEMORIA_MB" "PAGE_FAULTS" "PAGINAS_ESCRITAS"

for log in "${LOGS[@]}"; do
    for alg in "${ALGOS[@]}"; do
        for pag in "${PAGINAS[@]}"; do
            for mem in "${MEMORIAS[@]}"; do
                out=$("$SIM" "$alg" "$log" "$pag" "$mem")
                faults=$(echo "$out" | grep "Faltas"   | grep -o '[0-9]*')
                dirty=$( echo "$out" | grep "Escritas" | grep -o '[0-9]*')
                printf "%s,%s,%s,%s,%s,%s\n" \
                       "$log" "$alg" "$pag" "$mem" "$faults" "$dirty" >> "$SAIDA"
                printf "%-16s %-9s %-9s %-10s %-12s %-16s\n" \
                       "$log" "$alg" "$pag" "$mem" "$faults" "$dirty"
            done
        done
    done
done

echo
echo "Tabela completa salva em: $SAIDA"
