#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "api.h"
#include "bigint_struct.h"
#include "bigint_operation.h"

/**
 * @brief Add two words with carry and store the result in dest.
 *
 * This function adds two words (src1 and src2) considering a carry and stores the result in dest.
 *
 * @param dest Pointer to the destination word where the result will be stored.
 * @param src1 The first source word.
 * @param src2 The second source word.
 * @param carry Pointer to the carry flag.
 */
void add_word(word* dest, word src1, word src2, int* carry) {
    *dest = src1 + src2 + *carry;

    // Update carry flag
    if (*carry == 1) {
        if (*dest <= src1);
        else *carry = 0;
    }
    else {
        if (*dest < src1) *carry = 1;
        else;
    }
}

/**
 * @brief Add two big integers (src1 and src2) and store the result in dest.
 *
 * This function adds the big integers src1 and src2 and stores the result in dest.
 * The function handles cases where signs of src1 and src2 are different.
 *
 * @param dest Pointer to the destination big integer where the result will be stored.
 * @param src1 Pointer to the first source big integer.
 * @param src2 Pointer to the second source big integer.
 */
void add_bigint(bigint** dest, bigint* src1, bigint* src2) {
    // Ensure both source integers have the same word length
    bi_expand(src1, src2);

    // Initialize carry flag
    int carry = 0;

    if ((src1->sign == NON_NEGATIVE) && (src2->sign == NEGATIVE)) {
        // If src2 is negative, subtract its absolute value from src1
        flip_sign_of_bigint(&src2);
        sub_bigint(dest, src1, src2);
        flip_sign_of_bigint(&src2);
    }
    else if ((src1->sign == NEGATIVE) && (src2->sign == NON_NEGATIVE)) {
        // If src1 is negative, subtract its absolute value from src2
        flip_sign_of_bigint(&src1);
        sub_bigint(dest, src2, src1);
        flip_sign_of_bigint(&src1);
    }
    else {
        // If signs are the same, perform addition
        (*dest)->sign = src1->sign;
        int k = ((src1->wordlen >= src2->wordlen) ? (src1->wordlen) : (src2->wordlen));

        // Add corresponding words and propagate carry
        for (int i = 0; i < k; i++)
            add_word(&(*dest)->a[i], src1->a[i], src2->a[i], &carry);

        // Handle final carry, if any
        if (carry == 1) {
            if (src1->wordlen == src2->wordlen) {
                (*dest)->a = (word*)realloc((*dest)->a, sizeof(word) * (k + 1));
                (*dest)->a[k] = 1;
                (*dest)->wordlen = (k + 1);
            }
            else {
                (*dest)->a = (word*)realloc((*dest)->a, sizeof(word) * k);
                (*dest)->wordlen = k;
            }
        }
    }
}

/**
 * @brief Subtract two words with borrow and store the result in dest.
 *
 * This function subtracts two words (src2 from src1) considering a borrow and stores the result in dest.
 *
 * @param dest Pointer to the destination word where the result will be stored.
 * @param src1 The first source word.
 * @param src2 The second source word.
 * @param borrow Pointer to the borrow flag.
 */
void sub_word(word* dest, word src1, word src2, int* borrow) {
    *dest = src1 - src2 - *borrow;

    // Update borrow flag
    if (src1 < src2) *borrow = 1;
    else if (src1 == src2 && *borrow == 1) *borrow = 1;
    else *borrow = 0;
}

/**
 * @brief Subtract two big integers (src2 from src1) and store the result in dest.
 *
 * This function subtracts the big integer src2 from src1 and stores the result in dest.
 *
 * @param dest Pointer to the destination big integer where the result will be stored.
 * @param src1 Pointer to the first source big integer.
 * @param src2 Pointer to the second source big integer.
 */
void sub_bigint(bigint** dest, bigint* src1, bigint* src2) {
    // Ensure both source integers have the same word length
    bi_expand(src1, src2);

    // Initialize borrow flag
    int borrow = 0;

    if ((src1->sign == NON_NEGATIVE) && (src2->sign == NON_NEGATIVE)) {
        // If src1 is greater than src2 in absolute value
        if (compare_abs(src1, src2) == 1) {
            (*dest)->sign = NON_NEGATIVE;
            for (int i = 0; i < src1->wordlen; i++)
                sub_word(&(*dest)->a[i], src1->a[i], src2->a[i], &borrow);
            bi_refine(*dest);
        }
        // If src2 is greater than or equal to src1 in absolute value
        else {
            (*dest)->sign = NEGATIVE;
            for (int i = 0; i < src1->wordlen; i++)
                sub_word(&(*dest)->a[i], src2->a[i], src1->a[i], &borrow);
            bi_refine(*dest);
        }
    }
    else if ((src1->sign == NEGATIVE) && (src2->sign == NEGATIVE)) {
        // If src1 is greater than src2 in absolute value
        if (compare_abs(src1, src2) == 1) {
            (*dest)->sign = NEGATIVE;
            for (int i = 0; i < src1->wordlen; i++)
                sub_word(&(*dest)->a[i], src1->a[i], src2->a[i], &borrow);
            bi_refine(*dest);
        }
        // If src2 is greater than or equal to src1 in absolute value
        else {
            (*dest)->sign = NON_NEGATIVE;
            for (int i = 0; i < src1->wordlen; i++)
                sub_word(&(*dest)->a[i], src2->a[i], src1->a[i], &borrow);
            bi_refine(*dest);
        }
    }
    else {
        // Handle cases where signs are different by adding the absolute values
        flip_sign_of_bigint(&src2);
        add_bigint(dest, src1, src2);
        flip_sign_of_bigint(&src2);
    }
    bi_refine(src1);
    bi_refine(src2);
}

/**
 * @brief Multiply two words (A and B) using the standard word multiplication method.
 *
 * This function performs multiplication of two words (A and B) using the standard word
 * multiplication method and stores the result in the destination array dest.
 *
 * @param dest Pointer to the destination array where the result will be stored.
 * @param A The first word to be multiplied.
 * @param B The second word to be multiplied.
 */
void MULword(word* dest, word A, word B) {
    // Extract the lower and upper halves of A and B
    word A0 = A & (((word)1 << (sizeof(word) * 8 / 2)) - 1);
    word A1 = A >> (sizeof(word) * 8 / 2);

    word B0 = B & (((word)1 << (sizeof(word) * 8 / 2)) - 1);
    word B1 = B >> (sizeof(word) * 8 / 2);

    // Temporary variables for intermediate results
    word t, t0, t1, c0, c1;

    // Compute cross-products and accumulate
    t0 = A0 * B1;
    t1 = A1 * B0;
    t0 = t0 + t1;
    t1 = (t0 < t1) ? 1 : 0;

    // Compute diagonal products
    c0 = A0 * B0;
    c1 = A1 * B1;

    // Sum the diagonal and cross products with carry propagation
    t = c0;
    c0 = c0 + (t0 << (sizeof(word) * 8 / 2));
    c1 = c1 + (t1 << (sizeof(word) * 8 / 2)) + (t0 >> (sizeof(word) * 8 / 2)) + (c0 < t ? 1 : 0);

    // Store the result in the destination array
    dest[1] = c1;
    dest[0] = c0;
}

/**
 * @brief Perform multiplication of two bigints using the schoolbook method.
 *
 * This function multiplies two bigints, src1 and src2, using the traditional schoolbook method.
 * The result is stored in dest.
 *
 * @param dest Pointer to the destination bigint where the result will be stored.
 * @param src1 Pointer to the first source bigint.
 * @param src2 Pointer to the second source bigint.
 */
void mul_schoolBook(bigint** dest, bigint* src1, bigint* src2) {
    uint64_t n = src1->wordlen;
    uint64_t m = src2->wordlen;

    // Temporary bigints for intermediate results
    bigint* temp_wordMUL = NULL;
    bi_new(&temp_wordMUL, 2);
    bigint* temp_add_result = NULL;
    bi_new(&temp_add_result, n + m);

    // Perform multiplication using the schoolbook method
    for (uint64_t j = 0; j < n; j++) {
        for (uint64_t i = 0; i < m; i++) {
            // Initialize temporary bigint to zero
            memset(temp_wordMUL->a, 0, sizeof(word) * temp_wordMUL->wordlen);

            // Multiply each pair of words
            word Aj = src1->a[j];
            word Bi = src2->a[i];
            MULword(temp_wordMUL->a, Aj, Bi);

            // Shift the result and accumulate in the temporary result
            Left_shift(&temp_wordMUL, (i + j) * sizeof(word) * 8);
            add_bigint(&temp_add_result, temp_add_result, temp_wordMUL);
        }
    }

    // Update the sign of the result and assign to the destination bigint
    temp_add_result->sign = src1->sign ^ src2->sign;
    bi_assign(dest, temp_add_result);

    // Cleanup temporary bigints
    bi_delete(&temp_wordMUL);
    bi_delete(&temp_add_result);
}

/**
 * @brief Perform multiplication of two bigints using an improved algorithm.
 *
 * This function multiplies two bigints, src1 and src2, using an optimized algorithm.
 * It handles cases where the word length of src1 is odd by adjusting its size.
 * The result is stored in dest.
 *
 * @param dest Pointer to the destination bigint where the result will be stored.
 * @param src1 Pointer to the first source bigint.
 * @param src2 Pointer to the second source bigint.
 */
void improved_text_mul_bigint(bigint** dest, bigint* src1, bigint* src2) {
    uint64_t n = src1->wordlen;
    uint64_t m = src2->wordlen;

    // Adjust the size of src1 if its word length is odd
    if (n % 2 == 1) {
        src1->wordlen = n + 1;
        src1->a = (word*)realloc(src1->a, sizeof(word) * (n + 1));
        src1->a[n] = 0x00;
        n += 1;
    }

    // Temporary bigints for intermediate results
    bigint* temp_int1 = NULL;
    bigint* temp_int2 = NULL;
    bigint* temp_result = NULL;
    bi_new(&temp_int1, n + m);
    bi_new(&temp_int2, n + m);
    bi_new(&temp_result, n + m);

    // Initialize the destination bigint to zero
    bi_set_zero(dest);

    // Perform multiplication
    for (uint64_t i = 0; i < m; i++) {
        // Initialize temporary bigints to zero
        memset(temp_int1->a, 0, sizeof(word) * temp_int1->wordlen);
        memset(temp_int2->a, 0, sizeof(word) * temp_int2->wordlen);

        // Multiply each pair of words and accumulate the results
        for (uint64_t j = 0; j < (n / 2); j++) {
            word A1 = src1->a[2 * j];
            word B2 = src2->a[i];
            word A3 = src1->a[2 * j + 1];
            word B4 = src2->a[i];
            MULword(temp_int1->a + (2 * j + i), A1, B2);
            MULword(temp_int2->a + (2 * j + 1 + i), A3, B4);
        }

        // Combine the results
        add_bigint(&temp_result, temp_int1, temp_int2);

        // Shift the result and add it to the destination bigint
        add_bigint(dest, *dest, temp_result);
    }

    // Refine and update the sign of the destination bigint
    (*dest)->sign = src1->sign ^ src2->sign;

    // Cleanup temporary bigints
    bi_delete(&temp_result);
    bi_delete(&temp_int1);
    bi_delete(&temp_int2);
}

/**
 * @brief Square a single word using schoolbook method.
 *
 * This function squares a single word using the schoolbook method.
 *
 * @param dest Pointer to the destination word where the result will be stored.
 * @param src Source word to be squared.
 */
void SQRword(word* dest, word src) {
    word A0 = src & (((word)1 << (sizeof(word) * 8 / 2)) - 1);
    word A1 = src >> (sizeof(word) * 8 / 2);

    word T, C0, C1, temp;

    C1 = A1 * A1;
    C0 = A0 * A0;

    temp = C0;
    T = A0 * A1;
    C0 = (T << (sizeof(word) * 4) + 1) + C0;
    C1 = C1 + (T >> (sizeof(word) * 4) - 1) + (C0 < temp ? 1 : 0);
    dest[0] = C0;
    dest[1] = C1;
}

/**
 * @brief Square a bigint using schoolbook method.
 *
 * This function squares a bigint using the schoolbook method.
 *
 * @param dest Pointer to the destination bigint where the result will be stored.
 * @param src Pointer to the source bigint to be squared.
 */
void SQR_schoolBook(bigint** dest, bigint* src) {
    uint64_t n = src->wordlen;

    // Temporary variables for intermediate results
    bigint* temp_wordMUL1 = NULL;
    bi_new(&temp_wordMUL1, 2 * n);

    bigint* temp_wordMUL2 = NULL;
    bi_new(&temp_wordMUL2, 2 * n);

    bigint* temp_add_result1 = NULL;
    bi_new(&temp_add_result1, n + n);

    bigint* temp_add_result2 = NULL;
    bi_new(&temp_add_result2, n + n);

    bigint* temp_result = NULL;
    bi_new(&temp_result, n + n);

    for (uint64_t j = 0; j < n; j++) {
        // Initialize temp_wordMUL1 with zeros
        memset(temp_wordMUL1->a, 0, sizeof(word) * temp_wordMUL1->wordlen);

        // Compute T1 <- A(j)^2
        word A = src->a[j];
        SQRword(temp_wordMUL1->a + 2 * j, A);

        // Accumulate the results in temp_add_result1
        add_bigint(&temp_add_result1, temp_add_result1, temp_wordMUL1);

        for (uint64_t i = j + 1; i < n; i++) {
            // Initialize temp_wordMUL2 with zeros
            memset(temp_wordMUL2->a, 0, sizeof(word) * temp_wordMUL2->wordlen);

            // Compute T2 <- A(j) * A(i)
            word Aj = src->a[j];
            word Bi = src->a[i];
            MULword(temp_wordMUL2->a + i + j, Aj, Bi);

            // Accumulate the results in temp_add_result2
            add_bigint(&temp_add_result2, temp_add_result2, temp_wordMUL2);
        }
    }

    // Left shift temp_add_result2 by 1
    Left_shift(&temp_add_result2, 1);

    // Compute the final result by adding temp_add_result1 and temp_add_result2
    add_bigint(&temp_result, temp_add_result1, temp_add_result2);

    // Assign the result to the destination bigint
    bi_assign(dest, temp_result);
    (*dest)->sign = NON_NEGATIVE;

    // Cleanup temporary variables
    bi_delete(&temp_wordMUL1);
    bi_delete(&temp_wordMUL2);
    bi_delete(&temp_add_result1);
    bi_delete(&temp_add_result2);
    bi_delete(&temp_result);
}

/**
 * @brief Perform Karatsuba multiplication for bigints.
 *
 * This function computes the product of two bigints using the Karatsuba multiplication algorithm.
 *
 * @param dest Pointer to the destination bigint where the result will be stored.
 * @param src1 Pointer to the first operand bigint.
 * @param src2 Pointer to the second operand bigint.
 */
void karatsuba_mul(bigint** dest, bigint* src1, bigint* src2) {
    uint64_t len = (src1->wordlen < src2->wordlen) ? src1->wordlen : src2->wordlen;
    karatsuba_mul_core(dest, src1, src2, len);
    (*dest)->sign = src1->sign ^ src2->sign;
}

/**
 * @brief Core implementation of Karatsuba multiplication for bigints.
 *
 * This function is the core implementation of the Karatsuba multiplication algorithm for bigints.
 *
 * @param dest Pointer to the destination bigint where the result will be stored.
 * @param src1 Pointer to the first operand bigint.
 * @param src2 Pointer to the second operand bigint.
 * @param len Length parameter used in recursion.
 */
void karatsuba_mul_core(bigint** dest, bigint* src1, bigint* src2, uint64_t len) {
    // Base case: if the length is 1, perform simple multiplication
    if (len == 1) {
        improved_text_mul_bigint(dest, src1, src2);
        return;
    }

    // Divide the operands into high and low parts
    len >>= 1;
    bigint* A_h = NULL;
    bigint* A_l = NULL;
    bigint* B_h = NULL;
    bigint* B_l = NULL;

    bi_new(&A_h, src1->wordlen - len);
    bi_new(&A_l, len);
    bi_new(&B_h, src2->wordlen - len);
    bi_new(&B_l, len);

    array_copy(A_h->a, src1->a + len, src1->wordlen - len);
    array_copy(A_l->a, src1->a, len);
    array_copy(B_h->a, src2->a + len, src2->wordlen - len);
    array_copy(B_l->a, src2->a, len);

    // Temporary variables for intermediate results
    bigint* A_A = NULL;
    bigint* B_B = NULL;
    bigint* A = NULL;
    bigint* B = NULL;
    bigint* C = NULL;
    bigint* D = NULL;

    bi_new(&A_A, A_h->wordlen + 1);
    bi_new(&B_B, B_h->wordlen + 1);
    bi_new(&A, A_h->wordlen + B_h->wordlen);
    bi_new(&B, A_l->wordlen + B_l->wordlen);
    bi_new(&C, A_A->wordlen + B_B->wordlen);
    bi_new(&D, A_A->wordlen + B_B->wordlen);

    // Compute recursively
    uint64_t len_A = ((src1->wordlen - len) < (src2->wordlen - len)) ? (src1->wordlen - len) : (src2->wordlen - len);
    karatsuba_mul_core(&A, A_h, B_h, len_A);
    karatsuba_mul_core(&B, A_l, B_l, len);

    // Combine the results
    bi_expand(A_h, A_l);
    add_bigint(&A_A, A_h, A_l);
    bi_refine(A_A);
    bi_expand(B_h, B_l);
    add_bigint(&B_B, B_h, B_l);
    bi_refine(B_B);

    uint64_t len_C = (A_A->wordlen < B_B->wordlen) ? A_A->wordlen : B_B->wordlen;
    karatsuba_mul_core(&C, A_A, B_B, len_C);

    // Compute the final result using Karatsuba's formula
    bi_expand(C, A);
    sub_bigint(&D, C, A);
    bi_expand(D, B);
    sub_bigint(&D, D, B);

    Left_shift(&D, len * (8 * sizeof(word)));
    len <<= 1;
    Left_shift(&A, len * (8 * sizeof(word)));

    bi_expand(D, B);
    add_bigint(dest, D, B);
    bi_expand(*dest, A);
    add_bigint(dest, *dest, A);

    // Cleanup temporary variables
    bi_delete(&A);
    bi_delete(&B);
    bi_delete(&C);
    bi_delete(&D);
    bi_delete(&A_A);
    bi_delete(&B_B);
    bi_delete(&A_h);
    bi_delete(&A_l);
    bi_delete(&B_h);
    bi_delete(&B_l);
}

/**
 * @brief Perform division of two bigints using binary long division.
 *
 * This function computes the quotient and remainder of src1 divided by src2 using binary long division.
 *
 * @param dest_Q Pointer to the destination bigint where the quotient will be stored.
 * @param dest_R Pointer to the destination bigint where the remainder will be stored.
 * @param src1 Pointer to the dividend bigint.
 * @param src2 Pointer to the divisor bigint.
 */
void binary_DIVISION(bigint** dest_Q, bigint** dest_R, bigint* src1, bigint* src2) {
    uint64_t n = src1->wordlen * sizeof(word) * 8;

    bigint* Q = NULL;
    bigint* R = NULL;
    bigint* tmp = NULL;
    bi_set_zero(&Q);
    bi_set_zero(&R);
    bi_set_zero(&tmp);
    bigint* one = NULL;

    for (int j = n - 1; j >= 0; j--) {
        // Shift left by 1 (R = (R << 1) ⊕ aj)
        Left_shift(&R, 1);
        R->a[0] += get_j_th_bit(src1, j);

        // Copy src2 to tmp
        bi_assign(&tmp, src2);

        // While R >= tmp, subtract tmp from R and increment Q by 2^j
        while (compare_abs(R, tmp) >= 0) {
            bi_set_one(&one);
            Left_shift(&one, j);
            bi_expand(Q, one);
            add_bigint(&Q, Q, one);
            sub_bigint(&R, R, tmp);
        }
    }

    // Set the results
    //bi_set_by_array(dest_Q, NON_NEGATIVE, Q->a, Q->wordlen);
    bi_assign(dest_Q, Q);
    //bi_set_by_array(dest_R, NON_NEGATIVE, R->a, R->wordlen);
    bi_assign(dest_R, R);

    // Clean up temporary variables
    bi_delete(&Q);
    bi_delete(&R);
}

/**
 * @brief Perform division of two bigints.
 *
 * This function computes the quotient and remainder of src1 divided by src2.
 *
 * @param dest_Q Pointer to the destination bigint where the quotient will be stored.
 * @param dest_R Pointer to the destination bigint where the remainder will be stored.
 * @param src1 Pointer to the dividend bigint.
 * @param src2 Pointer to the divisor bigint.
 */
void DIVISION(bigint** dest_Q, bigint** dest_R, bigint* src1, bigint* src2) {
    int com = compare(src1, src2);

    if (is_zero(src2)) {
        printf("Cannot divide by 0\n");
        return;
    }

    if (com == -1) {
        // src1 < src2
        bi_set_zero(dest_Q);
        bi_assign(dest_R, src1);
    }
    else if (com == 1) {
        // src1 > src2
        binary_DIVISION(dest_Q, dest_R, src1, src2);
    }
    else {
        // src1 == src2
        bi_set_one(dest_Q);
        bi_set_zero(dest_R);
    }
}

/**
 * @brief Compute the Barrett reduction constant.
 *
 * This function computes the Barrett reduction constant for the given modulus.
 *
 * @param dest Pointer to the destination bigint where the computed constant will be stored.
 * @param src Pointer to the modulus bigint.
 */
void bi_barrett_compute_t(bigint** dest, bigint* src) {
    uint64_t n = src->wordlen;

    // Initialize temporary variables
    bigint* temp1 = NULL;
    bigint* temp2 = NULL;
    bigint* temp3 = NULL;
    bi_new(&temp1, src->wordlen);
    bi_new(&temp2, 2 * n + 1);
    bi_new(&temp3, 2 * n + 1);

    // Compute the Barrett reduction constant
    temp2->a[2 * n] = 1;
    DIVISION(&temp3, &temp1, temp2, src);
    bi_assign(dest, temp3);
    // bi_set_by_array(dest, NON_NEGATIVE, temp3->a, temp3->wordlen);

    // Clean up temporary variables
    bi_set_one(&temp3);
    bi_delete(&temp2);
    bi_delete(&temp3);
    bi_delete(&temp1);
}

/**
 * @brief Core Barrett reduction operation.
 *
 * This function performs the core Barrett reduction operation on the source bigint with respect to the given modulus and precomputed value.
 *
 * @param dest Pointer to the destination bigint where the result of the reduction will be stored.
 * @param src Pointer to the source bigint to be reduced.
 * @param mod Pointer to the modulus bigint.
 * @param pre_comp Pointer to the precomputed value for Barrett reduction.
 * @return 0 if successful, -1 if invalid inputs.
 */
int bi_barrett_reduction_core(bigint** dest, bigint* src, bigint* mod, bigint* pre_comp) {
    // Check for invalid inputs
    if (src == NULL || mod == NULL || pre_comp == NULL) {
        return -1;
    }

    int n = mod->wordlen;

    // Initialize temporary variables
    bigint* temp1 = NULL;
    bigint* temp2 = NULL;
    bigint* temp3 = NULL;
    bi_new(&temp1, 1);
    bi_new(&temp2, 1);
    bi_new(&temp3, src->wordlen);

    // Perform Barrett reduction computation
    bi_assign(&temp3, src);
    Right_shift(&temp3, (sizeof(word) * 8) * (n - 1));
    improved_text_mul_bigint(&temp2, temp3, pre_comp);
    Right_shift(&temp2, (sizeof(word) * 8) * (n + 1));
    improved_text_mul_bigint(&temp1, mod, temp2);
    bi_assign(&temp3, src);

    bi_expand(temp3, temp1);
    sub_bigint(dest, temp3, temp1);
    bi_refine(*dest);
    bi_refine(mod);

    // Perform modular reduction until the result is smaller than the modulus
    while (compare_abs(*dest, mod) >= 0) {
        sub_bigint(dest, *dest, mod);
    }
    bi_refine(src);
    bi_refine(mod);

    // Clean up temporary variables
    bi_delete(&temp2);
    // printf("\nhello\n");
    bi_delete(&temp1);
    bi_delete(&temp3);

    return 0;
}

/**
 * @brief Perform Barrett reduction on a bigint.
 *
 * This function performs Barrett reduction on the source bigint with respect to the given modulus.
 *
 * @param dest Pointer to the destination bigint where the result of the reduction will be stored.
 * @param src Pointer to the source bigint to be reduced.
 * @param mod Pointer to the modulus bigint.
 * @return 0 if successful, -1 if invalid inputs, or -2 if src->wordlen > 2 * mod->wordlen.
 */
int bi_barrett_reduction(bigint** dest, bigint* src, bigint* mod) {
    bigint* temp = NULL;
    bi_new(&temp, 1);

    // Check for invalid inputs
    if (is_one(mod)) {
        printf("\nmodular is one\n");
        return 0;
    }
    if (is_one(mod)) {
        bi_set_zero(dest);
        return 0;
    }
    if (mod->sign == 1) {
        printf("\nmodular sign is negative\n");
        return 0;
    }
    // Check if src->wordlen > 2 * mod->wordlen
    if (src->wordlen > (mod->wordlen * 2)) {
        return -2;
    }

    // Compute Barrett reduction
    bi_barrett_compute_t(&temp, mod);

    // Set the final result in the destination bigint
    bi_barrett_reduction_core(dest, src, mod, temp);

    // Clean up temporary variables
    bi_delete(&temp);
    return 0;
}

/**
* @brief Calculate exponentiation of x^n modulo mod using the Left-to-Right method.
*
* This function computes the result of x^n modulo mod using the Left-to-Right exponentiation method.
*
* @param dest Pointer to the destination bigint where the result of x^n will be stored.
* @param src1 Pointer to the base bigint (x).
* @param src2 Pointer to the power bigint (n).
* @param mod Pointer to the modulus bigint.
*/
void bi_LtR(bigint** dest, bigint* src1, bigint* src2, bigint* mod) {
    // Initialize temporary variables
    bigint* temp_x = NULL;     // Temporary variable for src1
    bi_set_by_array(&temp_x, NON_NEGATIVE, src1->a, src1->wordlen);
    bigint* temp1 = NULL;      // Temporary variable for the result
    bi_set_one(&temp1);
    bigint* temp_Q = NULL;     // Temporary variable for quotient in division
    bi_set_zero(&temp_Q);
    bigint* temp2 = NULL;      // Temporary variable for multiplication result
    bi_set_one(&temp2);
    bigint* temp_sqr = NULL;   // Temporary variable for squaring result
    bi_set_one(&temp_sqr);

    word _n = 0;               // Temporary variable for storing individual bits of src2

    // Main loop for Left-to-Right exponentiation
    for (int i = src2->wordlen - 1; i >= 0; i--) {
        for (int j = (sizeof(word) * 8) - 1; j >= 0; j--) {
            // Compute the square of temp1
            SQR_schoolBook(&temp_sqr, temp1);
            // Compute the quotient of temp1 divided by temp_sqr
            DIVISION(&temp_Q, &temp1, temp_sqr, mod);
            _n = (src2->a[i] >> j) & (word)0x01;

            if (_n == 1) {      // n_i = 1            
                // Multiply temp1 by temp_x
                improved_text_mul_bigint(&temp2, temp1, temp_x);
                // Compute the quotient of temp1 divided by temp2
                DIVISION(&temp_Q, &temp1, temp2, mod);
            }
        }
    }

    // Set the final result in the destination bigint
    bi_set_by_array(dest, NON_NEGATIVE, temp1->a, temp1->wordlen);

    // Clean up temporary variables
    bi_delete(&temp_x);
    bi_delete(&temp2);
    bi_delete(&temp_sqr);
    bi_delete(&temp1);
    bi_delete(&temp_Q);
}