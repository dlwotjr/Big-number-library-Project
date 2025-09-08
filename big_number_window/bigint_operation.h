#ifndef _BIGINT_OP_H
#define _BIGINT_OP_H

#include "bigint_struct.h"

// ����
void add_word(word* dest, word src1, word src2, int* carry);
void add_bigint(bigint** dest, bigint* src1, bigint* src2);

// ����
void sub_word(word* dest, word src1, word src2, int* borrow);
void sub_bigint(bigint** dest, bigint* src1, bigint* src2);

// ���� Textbook
void MULword(word* dest, word src1, word src2);
void mul_schoolBook(bigint** dest, bigint* src1, bigint* src2);

// ���� Improved Textbook
void improved_text_mul_bigint(bigint** dest, bigint* src1, bigint* src2);

// ���� Karatsuba
void karatsuba_mul_core(bigint** dest, bigint* src1, bigint* src2, uint64_t len);
void karatsuba_mul(bigint** dest, bigint* src1, bigint* src2);

// ����
void SQRword(word* dest, word src);
void SQR_schoolBook(bigint** dest, bigint* src);

// ���� barrett
void bi_barrett_compute_t(bigint** dest, bigint* src);
int bi_barrett_reduction_core(bigint** dest, bigint* src, bigint* mod, bigint* pre_comp);
int bi_barrett_reduction(bigint** dest, bigint* src, bigint* mod);

// ���� ������ LtR 
void bi_LtR(bigint** dest, bigint* src1, bigint* src2, bigint* mod);

#endif 
