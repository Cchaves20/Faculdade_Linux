/* Caio Faria Brito 2410162 3WB */
/* Nome_do_Aluno2 Matricula Turma */

#define NUM_BITS 128
typedef unsigned char BigInt[NUM_BITS/8]; // Um BigInt é representado como 16 bytes (128 bits).

/* =============================
   Funções de atribuição
   ============================= */

/* res = val (extensão de sinal)
   Essa função inicializa o BigInt "res" a partir de um long (64 bits).
   O valor é copiado bit a bit e estendido para 128 bits com sinal.
*/
void big_val (BigInt res, long val)
{
    int vBits[NUM_BITS]; // array temporário pra guardar cada bit separadamente
    int i = 0;

    // pega os 64 bits do long e guarda em vBits
    for(i = 0; i < 64; i++)
    {
        vBits[i] = (val >> i) & 1;
    }

    if (val == 0) // caso especial: tudo zero
    {
        for(i = 0; i < 16; i++)
        {
            res[i] = 0x00;
        }
    }
    else if (val > 0) // positivo → completa os bits mais altos com 0
    {
        for (i = 64; i < NUM_BITS; i++)
        {
            vBits[i] = 0;
        }
        // junta os bits em bytes
        for(i = 0; i < 16; i++)
        {
            res[i] = 0;
            for (int j = 0; j < 8; j++)
            {
                int indiceBit = i * 8 + j;
                int bit = vBits[indiceBit];
                res[i] |= (bit << j);
            }
        }
    }
    else // negativo → completa os bits mais altos com 1 (extensão de sinal)
    {
        for (i = 64; i < NUM_BITS; i++)
        {
            vBits[i] = 1;
        }
        for(i = 0; i < 16; i++)
        {
            res[i] = 0;
            for (int j = 0; j < 8; j++)
            {
                int indiceBit = i * 8 + j;
                int bit = vBits[indiceBit];
                res[i] |= (bit << j);
            }
        }
    }
}

/* =============================
   Operações aritméticas
   ============================= */

/* res = -a
   Calcula o complemento de dois: inverte os bits e soma 1.
*/
void big_comp2(BigInt res, BigInt a) {
    BigInt one;
    int i;
    unsigned short sum;
    unsigned char carry;

    // inverte todos os bits
    for (i = 0; i < 16; i++) {
        res[i] = ~a[i];
    }
    
    // prepara o valor "1" (para somar)
    for (i = 1; i < 16; i++) {
        one[i] = 0;
    }
    one[0] = 1;
    
    // soma res + 1 (com carry propagando)
    carry = 0;
    for (i = 0; i < 16; i++) {
        sum = res[i] + one[i] + carry;
        res[i] = (unsigned char)sum;
        carry = sum >> 8;
    }
}

/* res = a + b
   Soma byte a byte, cuidando do carry entre eles.
*/
void big_sum(BigInt res, BigInt a, BigInt b) {
    int i;
    unsigned short sum;
    unsigned char carry = 0;

    for (i = 0; i < 16; i++) {
        sum = a[i] + b[i] + carry;
        res[i] = (unsigned char)sum;
        carry = sum >> 8; // sobra vai para o próximo byte
    }
}

/* res = a - b
   Subtrai usando complemento de dois: a + (-b).
*/
void big_sub(BigInt res, BigInt a, BigInt b) {
    BigInt neg_b;
    big_comp2(neg_b, b);
    big_sum(res, a, neg_b);
}

/* res = a * b
   Multiplicação de 128 bits truncada:
   faz a multiplicação byte a byte e depois normaliza os carries.
   O resultado final cabe em 128 bits (descarta o resto).
*/
void big_mul(BigInt res, BigInt a, BigInt b) {
    unsigned int temp_res[32] = {0}; // guarda resultado temporário até 256 bits
    int i, j;

    // multiplicação básica estilo "escola primária"
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 16; j++) {
            temp_res[i + j] += (unsigned int)a[j] * b[i];
        }
    }

    // normaliza os carries (base 256 por byte)
    for (i = 0; i < 31; i++) {
        temp_res[i + 1] += temp_res[i] / 256;
        temp_res[i] %= 256;
    }

    // pega apenas os 16 bytes menos significativos
    for (i = 0; i < 16; i++) {
        res[i] = (unsigned char)temp_res[i];
    }
}

/* =============================
   Operações de deslocamento
   ============================= */

/* res = a << n
   Shift lógico para a esquerda.
   Preenche com zeros.
*/
void big_shl(BigInt res, BigInt a, int n) {
    int byte_shift;
    int bit_shift;
    int i;

    // deslocamento maior que 128 → tudo vira zero
    if (n >= NUM_BITS) {
        for (i = 0; i < 16; i++) {
            res[i] = 0;
        }
        return;
    }
    if (n <= 0) {
        for (i = 0; i < 16; i++) {
            res[i] = a[i];
        }
        return;
    }

    byte_shift = n / 8;
    bit_shift = n % 8;

    for (i = 15; i >= 0; i--) {
        unsigned char part1 = 0;
        unsigned char part2 = 0;

        if (i >= byte_shift) {
            part1 = a[i - byte_shift] << bit_shift;
        }
        if (bit_shift > 0 && i > byte_shift) {
            part2 = a[i - byte_shift - 1] >> (8 - bit_shift);
        }
        res[i] = part1 | part2;
    }
}

/* res = a >> n (lógico)
   Shift lógico para a direita.
   Preenche com zeros.
*/
void big_shr(BigInt res, BigInt a, int n) {
    int byte_shift;
    int bit_shift;
    int i;

    if (n >= NUM_BITS) {
        for (i = 0; i < 16; i++) {
            res[i] = 0;
        }
        return;
    }
    if (n <= 0) {
        for (i = 0; i < 16; i++) {
            res[i] = a[i];
        }
        return;
    }

    byte_shift = n / 8;
    bit_shift = n % 8;

    for (i = 0; i < 16; i++) {
        unsigned char part1 = 0;
        unsigned char part2 = 0;

        if (i + byte_shift < 16) {
            part1 = a[i + byte_shift] >> bit_shift;
        }
        if (bit_shift > 0 && i + byte_shift + 1 < 16) {
            part2 = a[i + byte_shift + 1] << (8 - bit_shift);
        }
        res[i] = part1 | part2;
    }
}

/* res = a >> n (aritmético)
   Shift aritmético para a direita.
   Preenche com o bit de sinal (1 se negativo, 0 se positivo).
*/
void big_sar(BigInt res, BigInt a, int n) {
    int byte_shift;
    int bit_shift;
    int i;
    unsigned char sign_fill;

    if (n <= 0) {
        for (i = 0; i < 16; i++) {
            res[i] = a[i];
        }
        return;
    }

    // determina se deve preencher com 1s ou 0s
    sign_fill = (a[15] & 0x80) ? 0xFF : 0x00;
    
    if (n >= NUM_BITS) {
        for (i = 0; i < 16; i++) {
            res[i] = sign_fill;
        }
        return;
    }
    
    byte_shift = n / 8;
    bit_shift = n % 8;
    
    for (i = 0; i < 16; i++) {
        unsigned char src1, src2;
        unsigned char part1, part2;

        // pega bytes deslocados, preenchendo com sinal se passar do limite
        src1 = (i + byte_shift < 16) ? a[i + byte_shift] : sign_fill;
        src2 = (i + byte_shift + 1 < 16) ? a[i + byte_shift + 1] : sign_fill;
        
        part1 = src1 >> bit_shift;
        part2 = (bit_shift > 0) ? (src2 << (8 - bit_shift)) : 0;
        
        res[i] = part1 | part2;
    }
}
