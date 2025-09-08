#ifndef _BIGINT_OP_H
#define _BIGINT_OP_H

#include "bigint_struct.h"

// µ¡¼À
void add_word(word* dest, word src1, word src2, int* carry);
void add_bigint(bigint** dest, bigint* src1, bigint* src2);

// »¬¼À
void sub_word(word* dest, word src1, word src2, int* borrow);
void sub_bigint(bigint** dest, bigint* src1, bigint* src2);

// °ö¼À Textbook
void MULword(word* dest, word src1, word src2);
void mul_schoolBook(bigint** dest, bigint* src1, bigint* src2);

// °ö¼À Improved Textbook
void improved_text_mul_bigint(bigint** dest, bigint* src1, bigint* src2);

// °ö¼À Karatsuba
void karatsuba_mul_core(bigint** dest, bigint* src1, bigint* src2, uint64_t len);
void karatsuba_mul(bigint** dest, bigint* src1, bigint* src2);

// Á¦°ö
void SQRword(word* dest, word src);
void SQR_schoolBook(bigint** dest, bigint* src);

// °¨»ê barrett
void bi_barrett_compute_t(bigint** dest, bigint* src);
int bi_barrett_reduction_core(bigint** dest, bigint* src, bigint* mod, bigint* pre_comp);
int bi_barrett_reduction(bigint** dest, bigint* src, bigint* mod);

// °¨»ê Áö¼ö½Â LtR 
void bi_LtR(bigint** dest, bigint* src1, bigint* src2, bigint* mod);

#endif 
