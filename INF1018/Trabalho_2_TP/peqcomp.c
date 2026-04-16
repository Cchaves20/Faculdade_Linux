/*Nome: Caio Faria Brito Martins Chaves*/
/*Matricula: 2410162*/
/*Nome: Joao Paulo de Araujo Frighetto*/
/*Matricula: 2410207*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Tipo de valor empacotado de 32 bits */
typedef unsigned packed_t;
/* Ponteiro para a função gerada (void → int) */
typedef int (*funcp) ();

/*
 * xbyte: extrai um byte (0..3) de uma palavra de 32 bits,
 * em ordem little-endian.
 */
int xbyte(packed_t word, int bytenum) {
    /* shift left para descartar bytes altos, depois shift aritmético */
    return (int)(word << (24 - 8*bytenum)) >> 24;
}

/*
 * error: imprime mensagem de erro com o número da linha
 * e encerra o programa.
 */
static void error(const char *msg, int line) {
    fprintf(stderr, "erro %s na linha %d\n", msg, line);
    exit(EXIT_FAILURE);
}

/*
 * peqcomp: lê comandos SBAS do FILE *f e emite o código
 * de máquina em codigo[]. Retorna um ponteiro para a função
 * compilada.
 */
funcp peqcomp(FILE *f, unsigned char codigo[]) {
    int line = 0;         /* contador de linhas do SBAS (para erros) */
    int indice = 0;       /* índice atual em codigo[] */
    int cmd_index = 0;    /* quantos comandos SBAS já processamos */

    /* cmd_start[i] = deslocamento em bytes onde começa o i-ésimo comando */
    int cmd_start[1000];

    /* Para patch de iflez: guardamos posição, índice SBAS e offset */
    struct { int pos, sidx, offs, line; } branches[100];
    int branch_count = 0;

    /* Mapeamento de variáveis SBAS v1..v5 → registradores x86 */
    /*   v1→ecx (1), v2→r8d (8), v3→r9d (9), v4→r10d (10), v5→r11d (11) */
    /*  Mapeamento usado para salvar o indice de cada registrador, usado no cálculo do endereço de cada variável/operação mais a frente */
    int reg_map[6]   = { 0, 1, 8, 9, 10, 11 };
    /* Mapeamento de parâmetros p1..p3 → registradores x86 */
    /*   p1→edi (7), p2→esi (6), p3→edx (2) */
    /*  mesmo objetivo do mapeamento dos registradores*/
    int param_map[4] = { 0, 7, 6, 2 };

    char c;
    /* ─── Prólogo da função compilada ─── */
    codigo[indice++] = 0x55;                    // push   rbp
    codigo[indice++] = 0x48; codigo[indice++] = 0x89; codigo[indice++] = 0xe5; // mov rbp,rsp
    codigo[indice++] = 0x48; codigo[indice++] = 0x83; codigo[indice++] = 0xec;
    codigo[indice++] = 0x10;                    // sub    rsp,16
    codigo[indice++] = 0x48; codigo[indice++] = 0x89; codigo[indice++] = 0x5d;
    codigo[indice++] = 0xf8;                    // mov    [rbp-8],rbx

    /* Loop principal: lê até EOF */
    while ((c = fgetc(f)) != EOF) {
        if (isspace(c)) continue;  // ignora espaços/linhas vazias
        line++;                    // conta linha SBAS

        /* registra início do código deste comando SBAS */
        cmd_start[cmd_index] = indice;

        if (c == 'v') {
            /* comando que começa com 'v' pode ser:
             *   v# : varpc     (atribuição)
             *   v# = varc op varc  (expressão)
             */
            int idx0; char op1;
            if (fscanf(f, "%d %c", &idx0, &op1) != 2)
                error("comando invalido", line);
            int dest = reg_map[idx0];  // registrador destino

            if (op1 == ':') {
                /* v# : varpc */
                char t; int val;
                if (fscanf(f, " %c%d", &t, &val) != 2)
                    error("comando invalido", line);

                /* trata alocação imediata */
                if (t == '$') {
                    /* movl imm32, com o imm32 sendo qualquer valor imediato(constante),para %dest */
                    if (dest >= 8) codigo[indice++] = 0x41;  // inicio de atribuição caso o parametro nao seja rxd (por ex:r8d, r9d,etc)
                    codigo[indice++] = 0xB8 + (dest & 7); /*soma o indice do destino até o 3° bit menos significativo com B8(10111000). Isso é feito pois esse é o cálculo do enderecamento de atribuições com registradores, onde cada registrador tem seu próprio índice*/
                    for (int i = 0; i < 4; i++)
                        codigo[indice++] = xbyte(val, i); /*adiciona o valor da constante no registrador desejado*/
                }
                /* trata cópia de variável SBAS */
                else if (t == 'v') {
                    int src = reg_map[val];
                    unsigned char rex = 0x40;
                    if (src  >= 8) rex |= 0x04;  // REX.R
                    if (dest >= 8) rex |= 0x01;  // REX.B
                    if (rex != 0x40) codigo[indice++] = rex;
                    codigo[indice++] = 0x89;     // MOV r/m32,r32
                    codigo[indice++] = 0xC0 | ((src & 7) << 3) | (dest & 7); /*calculo do endereço da operação, onde src sao os 3 bits menos significativos do registrador cujo valor será copiado e dest são os 3 bits menos significativos do registrador que receberá o valor*/
                }
                /* trata uso de parâmetro? não permitido em lado esquerdo */
                else if (t == 'p') {
                    error("atribuição em parâmetro não permitida", line);
                }
                else {
                    error("comando invalido", line);
                }
            }
            else if (op1 == '=') {
                /* v# = varc1 op varc2 */
                char t1, t2, op2;
                int v1, v2;
                if (fscanf(f, " %c%d %c %c%d", &t1, &v1, &op2, &t2, &v2) != 5)
                    error("comando invalido", line);

                /* 1) move primeiro operando → dest */
                if (t1 == '$') {
                    if (dest >= 8) codigo[indice++] = 0x41;
                    codigo[indice++] = 0xB8 + (dest & 7);
                    for (int i = 0; i < 4; i++)
                        codigo[indice++] = xbyte(v1, i);
                }
                else {
                    /* tanto variável quanto parâmetro são tratados aqui */
                    /*em todas as entradas do parametro que recebe o resultado da operação, e dos que serão os da operação, sao tratados com o mesmo cálculo da atribuição*/
                    int src1;
                    /*checa se está sendo realizado com parametros ou com registradores*/
                    if (t1 == 'v') src1 = reg_map[v1];
                    else if (t1 == 'p') src1 = param_map[v1];
                    else error("comando invalido", line);
                    
                    /*calculo do endereço baseado no registrador/parametro da operação*/
                    unsigned char rex = 0x40;
                    if (src1 >= 8) rex |= 0x04;
                    if (dest >= 8) rex |= 0x01;
                    if (rex != 0x40) codigo[indice++] = rex;
                    codigo[indice++] = 0x89;  // MOV r/m32,r32
                    codigo[indice++] = 0xC0 | ((src1 & 7) << 3) | (dest & 7);
                }

                /* 2) aplica o operador +, - ou * */
                if (op2 == '+') {
                    if (t2 == '$') {
                        /* addl imm32, %dest */
                        if (dest >= 8) codigo[indice++] = 0x41;
                        codigo[indice++] = 0x81;
                        codigo[indice++] = 0xC0 | (0<<3) | (dest & 7);
                        for (int i = 0; i < 4; i++)
                            codigo[indice++] = xbyte(v2, i);
                    } else {
                        int src2;
                        /*addl %src %dest*/
                        if (t2 == 'v') src2 = reg_map[v2];
                        else if (t2 == 'p') src2 = param_map[v2];
                        else error("comando invalido", line);

                        unsigned char rex = 0x40;
                        if (src2 >= 8) rex |= 0x04;
                        if (dest >= 8) rex |= 0x01;
                        if (rex != 0x40) codigo[indice++] = rex;
                        codigo[indice++] = 0x01;  // ADD r/m32,r32
                        codigo[indice++] =
                            0xC0 | ((src2 & 7) << 3) | (dest & 7);
                    }
                }
                else if (op2 == '-') {
                    if (t2 == '$') {
                        /* subl imm32, %dest */
                        if (dest >= 8) codigo[indice++] = 0x41;
                        codigo[indice++] = 0x81;
                        codigo[indice++] = 0xC0 | (5<<3) | (dest & 7);
                        for (int i = 0; i < 4; i++)
                            codigo[indice++] = xbyte(v2, i);
                    } else {
                        int src2;
                        /*subl %src %dest*/
                        if (t2 == 'v') src2 = reg_map[v2];
                        else if (t2 == 'p') src2 = param_map[v2];
                        else error("comando invalido", line);

                        unsigned char rex = 0x40;
                        if (src2 >= 8) rex |= 0x04;
                        if (dest >= 8) rex |= 0x01;
                        if (rex != 0x40) codigo[indice++] = rex;
                        codigo[indice++] = 0x29;  // SUB r/m32,r32
                        codigo[indice++] =
                            0xC0 | ((src2 & 7) << 3) | (dest & 7);
                    }
                }
                else if (op2 == '*') {
                    if (t2 == 'v' || t2 == 'p') {
                        /* imul %src %dest */
                        int src2 = (t2=='v' ? reg_map[v2] : param_map[v2]);
                        unsigned char rex = 0x40;
                        if (dest >= 8) rex |= 0x01;
                        if (src2  >= 8) rex |= 0x04;
                        if (rex != 0x40) codigo[indice++] = rex;
                        codigo[indice++] = 0x0F; codigo[indice++] = 0xAF;
                        codigo[indice++] =
                          0xC0 | ((dest & 7)<<3) | (src2 & 7);
                    }
                    else if (t2 == '$') {
                        /* imul $imm32, %dest */
                        if (dest >= 8) codigo[indice++] = 0x41;
                        codigo[indice++] = 0x69;  // IMUL r32,r/m32,imm32
                        codigo[indice++] =
                          0xC0 | ((dest & 7)<<3) | (dest & 7);
                        for (int i = 0; i < 4; i++)
                            codigo[indice++] = xbyte(v2, i);
                    }
                    else error("operador * invalido", line);
                }
                else {
                    error("operador invalido", line);
                }
            }
            else {
                error("comando invalido", line);
            }

            cmd_index++;
        }
        else if (c == 'r') {
            /* ret varpc: retorna $C, vM ou pM */
            char t; int v;
            if (fscanf(f, "et %c%d", &t, &v) != 2)
                error("comando invalido", line);

            if (t == '$') {
                /* movl imm32 → eax */
                codigo[indice++] = 0xB8;
                for (int i = 0; i < 4; i++)
                    codigo[indice++] = xbyte(v, i);
            }
            else {
                int src;
                /*movl %src, %eax*/
                if (t == 'v') src = reg_map[v];
                else if (t == 'p') src = param_map[v];
                else error("comando invalido", line);

                unsigned char rex = 0x40;
                if (src >= 8) rex |= 0x04;
                if (rex != 0x40) codigo[indice++] = rex;
                codigo[indice++] = 0x89;  // MOV r/m32,r32
                codigo[indice++] = 0xC0 | ((src & 7)<<3) | 0;
            }

            cmd_index++;
            break;
        }
        else if (c == 'i') {
            /* iflez varpc offset: checa se ≤0 e salta */
            char t; int v, offs;
            if (fscanf(f, "flez %c%d %d", &t, &v, &offs) != 3)
                error("comando invalido", line);

            /*mapeia o reg para o indice do registrador encontrado*/
            int srcreg;
            if (t == 'v') srcreg = reg_map[v];
            else if (t == 'p') srcreg = param_map[v];
            else error("comando invalido", line);

            /* salva o rex caso o indice do registrador seja maior ou igual a 8 */
            {
                unsigned char rex = 0x40;
                if (srcreg >= 8) rex |= 0x01;
                if (rex != 0x40) codigo[indice++] = rex;
            }
            codigo[indice++] = 0x83;                 // CMP r/m32,imm8
            codigo[indice++] = 0xF8 | (srcreg & 7); //calcula o código formado pelo registrador fonte (o que será comparado com o novo)
            codigo[indice++] = 0x00;                 // imm8 = 0

            /* jle rel32 (adição de placeholder) */
            codigo[indice++] = 0x0F;
            codigo[indice++] = 0x8E;                 // JLE rel32
            int pos = indice;
            for (int i = 0; i < 4; i++) codigo[indice++] = 0;

            /* salva para patch (feito pois a linha da qual será pulada será descoberta depois)*/
            branches[branch_count].pos  = pos;
            branches[branch_count].sidx = cmd_index;
            branches[branch_count].offs = offs;
            branches[branch_count].line = line;
            branch_count++;
            cmd_index++;
        }
        else {
            error("comando desconhecido", line);
        }
    }

    /* ─── Patch dos deslocamentos de cada iflez ─── */
    for (int i = 0; i < branch_count; i++) {
        int sidx = branches[i].sidx;
        int offs = branches[i].offs;
        int lino = branches[i].line;
        int target = sidx + 1 + offs;
        if (target < 0 || target >= cmd_index)
            error("offset de iflez invalido", lino);
        int dest_byte = cmd_start[target];
        int rel = dest_byte - (branches[i].pos + 4);
        for (int b = 0; b < 4; b++)
            codigo[ branches[i].pos + b ] = xbyte(rel, b);
    }

    /* ─── Epílogo da função compilada ─── */
    codigo[indice++] = 0x48; codigo[indice++] = 0x8B; codigo[indice++] = 0x5D;
    codigo[indice++] = 0xF8;                // mov rbx,[rbp-8]
    codigo[indice++] = 0xC9;                // leave
    codigo[indice++] = 0xC3;                // ret

    return (funcp) codigo;
}
