#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include "bigint_struct.h"

//====================2.2 Create/ Delete Bifint====================
//Create
void bi_new(bigint** x, int wordlen)
{
    if (*x != NULL)
        bi_delete(x);

    *x = (bigint*)malloc(sizeof(bigint));
    (*x)->sign = NON_NEGATIVE; // 0
    (*x)->wordlen = wordlen;
    (*x)->a = (word*)calloc(wordlen, sizeof(word));
}
//Delete
void bi_delete(bigint** x)
{
    if (*x == NULL)
        return;
# ifdef ZERORIZE
    array_init((*x)->a, (*x)->wordlen);
# endif
    free((*x)->a);
    free(*x);
    *x = NULL;
}

//====================2.3 Show Bigint====================
void bi_show_hex(bigint* x) {
    if (x->wordlen == 0 || x->a == NULL) {
        printf("bi_show_hex ERROR, wordLen = 0 or Its NULL pointer \n");
        return;
    }

    uint8_t* temp = x->a;
    printf("%c", '+' + ((x->sign) * 2));
    printf("0x");
    for (int i = x->wordlen - 1; i >= 0; i--) {
        for (int j = sizeof(word) - 1; j >= 0; j--) {
            printf("%02x", *((uint8_t*)temp + i * sizeof(word) + j));
        }
        // printf(" ");
    }
    // printf("\n");
}

void bi_show_dec(bigint* x) {
    //after divide ftn
}

void bi_show_bin(bigint* x) {
    if (x->wordlen == 0 || x->a == NULL) {
        printf("bi_show_bin ERROR, wordLen = 0 or Its NULL pointer \n");
        return;
    }

    uint8_t* temp = x->a;
    printf("%c", '+' + (x->sign) * 2);
    printf("0b");
    for (int i = 0; i <= x->wordlen - 1; i++) {
        for (int j = sizeof(word) - 1; j >= 0; j--) {
            for (int k = 7; k >= 0; k--) {
                printf("%d", ((*((uint8_t*)temp + i * sizeof(word) + j)) >> k) & 0x01);
            }
            printf(" ");
        }
        printf("\n");
    }
    printf("\n");
}

//====================2.4 Set Bigint====================
// bigint x <- sign and array a
int bi_set_by_array(bigint** x, int sign, word* a, int wordlen) {
    if (a == NULL || wordlen == 0 || !(sign == 0 || sign == 1)) {
        printf("bi_set_by_array ERROR: a == NULL || wordlen == 0 || !(sign == 0 || sign == 1)\n");
        return 0;
    }
    bi_delete(x);
    bi_new(x, wordlen);
    (*x)->sign = sign; // 0
    for (int i = 0; i < wordlen; i++) {
        (*x)->a[i] = a[i];
    }
}
// bigint x <- base string
//   Է            ΰ ?
int bi_set_by_string(bigint** x, int sign, char* str, int base) {
    //          Ŀ  ©         
}

//====================2.5 Refine Bigint====================
// Remove Last Zero Words
void bi_refine(bigint* x) {
    if (x == NULL)
        return;
    int new_wordlen = x->wordlen;
    while (new_wordlen > 1) // at least one word needed
    {
        if (x->a[new_wordlen - 1] != 0)
            break;
        new_wordlen--;
    }
    if (x->wordlen != new_wordlen)
    {
        x->wordlen = new_wordlen;
        x->a = (word*)realloc(x->a, sizeof(word) * new_wordlen);
    }
    if ((x->wordlen == 1) && (x->a[0] == 0x0))
        x->sign = NON_NEGATIVE;
}

// Zero Words Expansion
void bi_expand(bigint* x, bigint* y) {
    if (x->wordlen == y->wordlen || x->wordlen <= 0 || y->wordlen <= 0) return;
    else if (x->wordlen > y->wordlen) {
        int big_wordlen = x->wordlen;
        y->a = (word*)realloc(y->a, sizeof(word) * big_wordlen);

        for (int i = x->wordlen - 1; i >= y->wordlen; i--)
            y->a[i] = 0;
        y->wordlen = big_wordlen;
    }
    else {
        int big_wordlen = y->wordlen;
        x->a = (word*)realloc(x->a, sizeof(word) * big_wordlen);

        for (int i = y->wordlen - 1; i >= x->wordlen; i--)
            x->a[i] = 0;
        x->wordlen = big_wordlen;
    }
}

//====================2.6 Assign Bigint(    )====================
void bi_assign(bigint** y, bigint* x) {
    if (*y != NULL)
        bi_delete(y);
    bi_new(y, x->wordlen);
    (*y)->sign = x->sign;
    array_copy((*y)->a, x->a, x->wordlen);
}
void array_copy(word* dst, word* src, int wordlen) {
    // int cnt = wordlen * sizeof(word);
    int cnt = wordlen;
    while (cnt > 0)
    {
        *dst = *src;
        dst++; src++;
        cnt--;
    }
}

//====================2.7 Generate Random Bigint====================
void bi_gen_rand(bigint** x, int sign, int wordlen) {
    bi_new(x, wordlen);
    (*x)->sign = sign;
    array_rand((*x)->a, wordlen);
    bi_refine(*x);
}
void array_rand(word* dst, int wordlen) {
    byte* p = (byte*)dst;
    int cnt = wordlen * sizeof(word);
    while (cnt > 0)
    {
        *p = rand() & 0xff; // rand = DRBG
        p++;
        cnt--;
    }

}

//====================2.8 Set One/Zero====================
void bi_set_one(bigint** x) {
    bi_new(x, 1);
    (*x)->sign = NON_NEGATIVE;
    (*x)->a[0] = 0x1;

}
void bi_set_zero(bigint** x) {
    bi_new(x, 1);
    (*x)->sign = NON_NEGATIVE;
    (*x)->a[0] = 0x0;

}

//====================2.9 Is One/Zero?====================
int is_one(bigint* src) {
    if (src->sign == 1 || src->a[0] != 1) {
        return 0; // Not positive number
    }
    for (int j = src->wordlen - 1; j > 0; j--) {
        if (src->a[j] != 0) {
            return 0; // Not equal to 0
        }
    }
    return 1; // Not equal to 1
}
int is_zero(bigint* src) {
    if (src->sign == 1 || src->a[0] != 0) {
        return 0; // Not a non-negative number
    }
    for (int j = src->wordlen - 1; j > 0; j--) {
        if (src->a[j] != 0) {
            return 0; // Not equal to 0
        }
    }
    return 1; // Value is 0
}

//====================2.10 Compare====================
// |A| > |B| -> 1
// |A| < |B| -> -1
// |A| = |B| -> 0
int compare_abs(bigint* src1, bigint* src2) {
    if (src1->wordlen > src2->wordlen) {
        return 1;
    }
    else if (src1->wordlen < src2->wordlen) {
        return -1;
    }
    for (int64_t i = src1->wordlen - 1; i >= 0; i--) {
        if (src1->a[i] > src2->a[i]) {
            return 1;
        }
        else if (src1->a[i] < src2->a[i]) {
            return -1;
        }
    }
    return 0;
}
// A > B -> 1
// A < B -> -1
// A = B -> 0
int compare(bigint* src1, bigint* src2) {
    if (src1->sign < src2->sign) {
        return 1;
    }
    else if (src1->sign > src2->sign) {
        return -1;
    }
    int ret = compare_abs(src1, src2);
    if (src1->sign == NON_NEGATIVE) {
        return ret;
    }
    else {
        return -ret;
    }
}

//====================2.11 Left/Right Shift====================
void Left_shift(bigint** src, uint64_t k) {
    // Check for invalid inputs: NULL source or non-positive shift value.
    if (*src == NULL) {
        printf("Invalid inputs. Function: Left_shift\n");
        return;
    }
    if (k == 0) return;

    // Calculate word and bit offsets for the shift operation.
    uint64_t word_shift = k / (8 * sizeof(word)); // Offset in words
    uint64_t bit_offset = k % (8 * sizeof(word));  // Offset in bits

    // Create a temporary bigint to hold the shifted value.
    bigint* temp = NULL;  // Changed to bigint* from bigint**
    bi_new(&temp, (*src)->wordlen + word_shift + 1);  // Pass the address of temp
    temp->sign = (*src)->sign;

    // Perform word shift.
    if (word_shift == 0) {
        memcpy(temp->a, (*src)->a, sizeof(word) * (*src)->wordlen);
    }
    else {
        for (uint64_t i = 0; i < (*src)->wordlen; i++) {
            uint64_t shifted_index = i + word_shift;
            temp->a[shifted_index] = (*src)->a[i];
        }
    }

    if (bit_offset != 0) {
        word carry = 0;
        for (int i = 0; i < temp->wordlen; i++) {
            word temp_word = temp->a[i];
            word shifted_word = temp->a[i] << bit_offset;
            temp->a[i] = (shifted_word | carry);
            carry = temp_word >> ((8 * sizeof(word)) - bit_offset);
        }
        // Handle the final carry.
        if (temp->wordlen > 0 && carry > 0) {
            temp->a[temp->wordlen - 1] |= carry;
        }
    }

    bi_refine(temp);
    bi_assign(src, temp);
    bi_delete(&temp);  // Pass the address of temp to bi_delete
}

void Right_shift(bigint** src, uint64_t k) {
    // Check for invalid inputs: NULL source or non-positive shift value.
    if (*src == NULL || k < 0) {
        printf("Invalid inputs. Function: Right_shift\n");
        return;
    }

    // Calculate word and bit offsets for the shift operation.
    uint64_t word_shift = k / (8 * sizeof(word)); // Offset in words
    uint64_t bit_offset = k % (8 * sizeof(word));  // Offset in bits
    //printf("word_shift : %llu, bit_shift : %llu\n", word_shift, bit_offset);
    // Create a temporary bigint to hold the shifted value.
    bigint* temp = NULL;
    bi_new(&temp, (*src)->wordlen - word_shift);
    if (word_shift >= (*src)->wordlen) {
        bi_set_zero(src);
        return;
    }
    // Perform word shift.
    for (uint64_t i = 0; i < temp->wordlen; i++) {
        //  printf("i : %llu", i);
        uint64_t shifted_index = i + word_shift;
        temp->a[i] = (*src)->a[shifted_index];
    }

    // Perform bit shift within each word.
    if (bit_offset > 0) {
        word carry = 0;
        for (int i = temp->wordlen - 1; i >= 0; i--) {
            word temp_word = temp->a[i];
            word shifted_word = temp->a[i] >> bit_offset;
            temp->a[i] = (shifted_word | carry);
            carry = temp_word << ((8 * sizeof(word)) - bit_offset);
        }
    }


    bi_refine(temp);
    bi_new(src, temp->wordlen);
    memcpy((*src)->a, temp->a, sizeof(word) * temp->wordlen);
    bi_delete(&temp);
}
//====================2.12 Reduction: A mod 2^r====================
void reduction(bigint** src, int r) {
    if (*src == NULL || r <= 0) {
        printf("Invalid inputs._ftn : reduction\n");
        return;
    }

    uint64_t byte_offset = r / (sizeof(word) * 8); // Offset in bytes
    uint64_t bit_offset = r % (sizeof(word) * 8);  // Offset in bits
    if (bit_offset == 0)
        byte_offset--;
    for (uint64_t i = (*src)->wordlen; i > byte_offset + 1; i--)
        (*src)->a[i - 1] = 0x00;

    (*src)->a[byte_offset] <<= ((sizeof(word) * 8) - bit_offset);
    (*src)->a[byte_offset] >>= ((sizeof(word) * 8) - bit_offset);

    bi_refine(*src);
}

//====================2.13 Get Word / Bit Length====================
void get_word(bigint* src, int n_th_Word, word* result) {
    if (src == NULL || n_th_Word <= 0 || n_th_Word > src->wordlen) {
        printf("Invalid inputs. FTN : Get_Word\n");
        return;
    }
    *result = src->a[n_th_Word - 1];
}

uint64_t bit_length(bigint* src) {
    word a = src->a[src->wordlen];
    int i = 0;
    while (TRUE) {
        if (a & 0x01 == 0) {
            i++;
            a >> 1;
        }
        else break;
    }
    return  src->wordlen * sizeof(word) - i;
}

int get_wordlen(bigint* x) {
    return x->wordlen;
}

// 활성화 된 비트 수만 뽑아오는건지 확인하기
int get_bitlen(bigint* x) {
    return x->wordlen * sizeof(word) * 8;
}

//====================2.14 Get j - th Bit====================
// 제일 처음 비트를 0번째로 정의함
int get_j_th_bit(bigint* src, int j) {
    if (src == NULL || src->a == NULL || src->wordlen == 0 || j < 0) {
        printf("Invalid inputs.  FTN : Get_j_th_Bit \n");
        return -1;
    }
    int q = j / (sizeof(word) * 8);
    int r = j % (sizeof(word) * 8);
    return (src->a[q] >> r) & 0x01;
}

//====================2.15 Get Sign and Flip Sign of BitInt====================
int get_sign(bigint* src) {
    if (src == NULL) {
        printf("Invalid bigint. FTN : Get_Sign\n");
        return 0;
    }
    return src->sign;
}
void flip_sign_of_bigint(bigint** src) {
    if (*src == NULL) {
        printf("Invalid bigint. FTN : Flip_SignOfBitint\n");
        return;
    }
    (*src)->sign = (-1 * (*src)->sign) + 1; // constant time
}