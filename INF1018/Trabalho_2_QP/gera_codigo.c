/* Caio Faria Brito Martins Chaves 2410162 Turma 3WB */
/* LHM Turma 3WB */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "gera_codigo.h"

/* imprime mensagem de erro e encerra */
static void error(const char *msg, int line) {
    fprintf(stderr, "erro %s na linha %d\n", msg, line);
    exit(EXIT_FAILURE);
}

/* gera_codigo
 * Traduz código LBS em código x86-64, armazenando em code[].
 * Retorna via *entry o endereço da última função LBS traduzida.
 */
void gera_codigo(FILE *f, unsigned char code[], funcp *entry) {
    int line = 1, c, pos = 0;
    long func_start[50]; 
    int func_count = 0;

    struct {
        int func_dest;
        int pos_disp;
    } calls[100];
    int call_count = 0;

    while ((c = fgetc(f)) != EOF) {
        switch (c) {

        /* início de função */
        case 'f': {
            fscanf(f, "unction");
            func_start[func_count++] = (long)&code[pos];

            /* prólogo padrão: push rbp; mov rbp, rsp; sub rsp, 32 */
            unsigned char prologo[] = {0x55, 0x48,0x89,0xe5, 0x48,0x83,0xec,0x20};
            memcpy(&code[pos], prologo, sizeof(prologo)); 
            pos += sizeof(prologo);
            break;
        }

        /* fim de função */
        case 'e': {
            fscanf(f, "nd");
            /* epílogo: leave; ret */
            code[pos++] = 0xc9; 
            code[pos++] = 0xc3;
            break;
        }

        /* retorno incondicional */
        case 'r': {
            int idx; 
            char v;
            if (fscanf(f, "et %c%d", &v, &idx) != 2) error("comando invalido", line);

            if (v == 'v') {
                code[pos++] = 0x8b; code[pos++] = 0x45; code[pos++] = (char)(-4 * (idx + 1));
            } else if (v == 'p') {
                code[pos++] = 0x89; code[pos++] = 0xf8; /* mov eax, edi */
            } else if (v == '$') {
                int val = idx; code[pos++] = 0xb8; memcpy(&code[pos], &val, 4); pos += 4;
            } else error("ret invalido", line);

            code[pos++] = 0xc9; code[pos++] = 0xc3;
            break;
        }

        /* retorno condicional */
        case 'z': {
            int i0, i1; 
            char v0, v1;
            if (fscanf(f, "ret %c%d %c%d", &v0, &i0, &v1, &i1) != 4) error("comando invalido", line);

            /* cmp operando com 0 */
            if (v0 == 'v') {
                code[pos++] = 0x83; code[pos++] = 0x7d; code[pos++] = (char)(-4 * (i0 + 1)); code[pos++] = 0x00;
            } else if (v0 == 'p') {
                code[pos++] = 0x83; code[pos++] = 0xff; code[pos++] = 0x00;
            } else error("zret operando invalido", line);

            /* jne <offset> */
            int pos_jne = pos;
            code[pos++] = 0x75; 
            code[pos++] = 0x00; /* placeholder */

            /* bloco de retorno se zero */
            if (v1 == 'v') {
                code[pos++] = 0x8b; code[pos++] = 0x45; code[pos++] = (char)(-4 * (i1 + 1));
            } else if (v1 == 'p') {
                code[pos++] = 0x89; code[pos++] = 0xf8; /* mov eax, edi */
            } else if (v1 == '$') {
                int val = i1; code[pos++] = 0xb8; memcpy(&code[pos], &val, 4); pos += 4;
            } else error("zret segundo operando invalido", line);

            code[pos++] = 0xc9; code[pos++] = 0xc3; /* leave; ret */
            int bytes_ret_end = pos;

            /* offset relativo ao fim da instrução JNE */
            int offset = bytes_ret_end - (pos_jne + 2);
            if (offset > 127) error("zret bloco muito grande", line);
            code[pos_jne + 1] = (unsigned char)offset;
            break;
        }

        /* atribuições vN = ... */
        case 'v': {
            int idx0; char c0;
            if (fscanf(f, "%d = %c", &idx0, &c0) != 2) error("comando invalido", line);

            /* chamada de função */
            if (c0 == 'c') {
                int fnum, i1; char v1;
                if (fscanf(f, "all %d %c%d", &fnum, &v1, &i1) != 3) error("comando invalido", line);

                /* move argumento para edi */
                if (v1 == 'v') {
                    code[pos++] = 0x8b; code[pos++] = 0x7d; code[pos++] = (char)(-4 * (i1 + 1));
                } else if (v1 == '$') {
                    int val = i1; code[pos++] = 0xbf; memcpy(&code[pos], &val, 4); pos += 4;
                } else if (v1 == 'p') {
                    /* já está em edi */
                } else error("operando de call invalido", line);

                /* call rel32 (placeholder) */
                code[pos++] = 0xe8;
                calls[call_count].func_dest = fnum;
                calls[call_count].pos_disp = pos;
                call_count++;

                int zero = 0; memcpy(&code[pos], &zero, 4); pos += 4;

                /* mov eax -> [rbp - var] */
                code[pos++] = 0x89; code[pos++] = 0x45; code[pos++] = (char)(-4 * (idx0 + 1));
            }

            /* operação aritmética */
            else {
                int i1, i2; 
                char v1 = c0, v2, op;
                if (fscanf(f, "%d %c %c%d", &i1, &op, &v2, &i2) != 4) error("comando invalido", line);

                /* carrega op1 em eax */
                if (v1 == 'v') { code[pos++] = 0x8b; code[pos++] = 0x45; code[pos++] = (char)(-4 * (i1 + 1)); }
                else if (v1 == 'p') { code[pos++] = 0x89; code[pos++] = 0xf8; }
                else if (v1 == '$') { int val = i1; code[pos++] = 0xb8; memcpy(&code[pos], &val, 4); pos += 4; }
                else error("operando 1 invalido", line);

                /* aplica operação */
                if (op == '+') {
                    if (v2 == 'v') { code[pos++] = 0x03; code[pos++] = 0x45; code[pos++] = (char)(-4 * (i2 + 1)); }
                    else if (v2 == 'p') { code[pos++] = 0x01; code[pos++] = 0xf8; }
                    else if (v2 == '$') {
                        if (i2 >= -128 && i2 <= 127) { code[pos++] = 0x83; code[pos++] = 0xc0; code[pos++] = (char)i2; }
                        else { code[pos++] = 0x05; memcpy(&code[pos], &i2, 4); pos += 4; }
                    } else error("operando 2 invalido", line);

                } else if (op == '-') {
                    if (v2 == 'v') { code[pos++] = 0x2b; code[pos++] = 0x45; code[pos++] = (char)(-4 * (i2 + 1)); }
                    else if (v2 == 'p') { code[pos++] = 0x29; code[pos++] = 0xf8; }
                    else if (v2 == '$') { code[pos++] = 0x83; code[pos++] = 0xe8; code[pos++] = (unsigned char)i2; }
                    else error("operando 2 invalido", line);

                } else if (op == '*') {
                    if (v2 == 'v') { code[pos++] = 0x0f; code[pos++] = 0xaf; code[pos++] = 0x45; code[pos++] = (char)(-4 * (i2 + 1)); }
                    else if (v2 == 'p') { code[pos++] = 0x0f; code[pos++] = 0xaf; code[pos++] = 0xc7; }
                    else if (v2 == '$') { int val = i2; code[pos++] = 0x69; code[pos++] = 0xc0; memcpy(&code[pos], &val, 4); pos += 4; }
                    else error("operando 2 invalido", line);
                } else error("operacao invalida", line);

                /* salva resultado */
                code[pos++] = 0x89; code[pos++] = 0x45; code[pos++] = (char)(-4 * (idx0 + 1));
            }
            break;
        }

        /* caracteres ignorados */
        default:
            if (!isspace(c)) error("comando desconhecido", line);
        }

        if (c == '\n') line++;
        int ch;
        while ((ch = fgetc(f)) == ' ' || ch == '\t' || ch == '\r');
        if (ch != EOF) ungetc(ch, f);
    }

    /* patch final dos CALLs */
    for (int i = 0; i < call_count; i++) {
        long desloc = func_start[calls[i].func_dest] - ((long)&code[calls[i].pos_disp] + 4);
        memcpy(&code[calls[i].pos_disp], &desloc, 4);
    }

    /* entry: última função */
    *entry = (funcp)func_start[func_count - 1];
}