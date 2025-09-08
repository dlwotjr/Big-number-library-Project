#pragma once
#include <stdint.h>

typedef uint8_t byte;
typedef uint64_t word;
#define NON_NEGATIVE 0
#define NEGATIVE 1
#define TRUE 1
#define FALSE 0

typedef struct {
	int sign;
	uint64_t wordlen;
	word* a;
} bigint;

//2.2 Create/ Delete Bifint 
void bi_new(bigint** x, int wordlen);
void bi_delete(bigint** x);

//2.3 Show Bigint
void bi_show_hex(bigint* x); // show x in hexa . representation .
void bi_show_dec(bigint* x); // show x in deci . representation .
void bi_show_bin(bigint* x); // show x in binary representation .

//2.4 Set Bigint
// bigint x <- sign and array a
void bi_set_by_array(bigint** x, int sign, word* a, int wordlen);
// bigint x <- base string
int bi_set_by_string(bigint** x, int sign, char* str, int base);

//2.5 Refine Bigint(Remove Last Zero Words)
void bi_refine(bigint* x);
void bi_expand(bigint* x, bigint* y);

//2.6 Assign Bigint
void bi_assign(bigint** y, bigint* x);
void array_copy(word* dst, word* src, int wordlen);

//2.7 Generate Random Bigint
void bi_gen_rand(bigint** x, int sign, int wordlen);
void array_rand(word* dst, int wordlen);

//2.8 Set One/Zero
void bi_set_one(bigint** x);
void bi_set_zero(bigint** x);

//2.9 Is One/Zero?
int is_one(bigint* src);
int is_zero(bigint* src);

//2.10 Compare
int compare_abs(bigint* src1, bigint* src2);
int compare(bigint* src1, bigint* src2);

//2.11 Left/Right Shift A << r, A >> r
void Left_shift(bigint** src, uint64_t k);
void Right_shift(bigint** src, uint64_t k);

//2.12 Reduction: A mod 2^r
void reduction(bigint** src, int r);

//2.13 Get Word / Bit Length
void get_word(bigint* src, int n_th_Word, word* result);
uint64_t bit_length(bigint* src);
int get_wordlen(bigint* x);
int get_bitlen(bigint* x);

//2.14 Get j - th Bit
int get_j_th_bit(bigint* src, int j);

//2.15 Get Sign and Flip Sign of BitInt
int get_sign(bigint* src);
void flip_sign_of_bigint(bigint** src);
