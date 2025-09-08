//#pragma once

void add_word(word* c, word a, word b, int* carry);
void add_bigint(bigint** c, bigint* a, bigint* b);

void sub_word(word* c, word a, word b, int* borrow);
void sub_bigint(bigint** c, bigint* a, bigint* b);

void MULword(word* dest, word src1, word src2);
void mul_schoolBook(bigint** dest, bigint* src1, bigint* src2);

void improved_text_mul_bigint(bigint** dest, bigint* src1, bigint* src2);
void SQRword(word* dest, word src);
void SQR_schoolBook(bigint** dest, bigint* src);

void karatsuba_mul_core(bigint** dest, bigint* src1, bigint* src2, uint64_t len);
void karatsuba_mul(bigint** dest, bigint* src1, bigint* src2);


void bi_barrett_compute_t(bigint** dest, bigint* src);
int bi_barrett_reduction_core(bigint** dest, bigint* src, bigint* mod, bigint* pre_comp);
int bi_barrett_reduction(bigint** dest, bigint* src, bigint* mod);


void bi_LtR(bigint** dest, bigint* src1, bigint* src2, bigint* mod);
void DIVISION(bigint** dest_Q, bigint** dest_R, bigint* src1, bigint* src2);