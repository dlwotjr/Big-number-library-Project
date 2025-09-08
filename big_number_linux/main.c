#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bigint_struct.h"
#include "bigint_operation.h"
#include "aes.h"
#include "nistseedexpander.h"
#include "randombytes.h"
//#include <intrin.h>

void verfication_add(bigint* z, bigint* x, bigint* y) {
    printf("if ((");
    bi_show_hex(x);
    printf(") + (");
    bi_show_hex(y);
    printf(") == ");
    bi_show_hex(z);
    printf("):");
    printf("\n  print(\"PASS\\n\")\n");
    printf("else :");
    printf("\n  print(\"====================WRONG====================\\n\")");
    printf("\n  exit()");
}
void verfication_sub(bigint* z, bigint* x, bigint* y) {
    printf("if ((");
    bi_show_hex(x);
    printf(") - (");
    bi_show_hex(y);
    printf(") == ");
    bi_show_hex(z);
    printf("):");
    printf("\n  print(\"PASS\\n\")\n");
    printf("else :");
    printf("\n  print(\"====================WRONG====================\\n\")");
    printf("\n  exit()");
}
void verfication_mul(bigint* z, bigint* x, bigint* y) {
    printf("if ((");
    bi_show_hex(x);
    printf(") * (");
    bi_show_hex(y);
    printf(") == ");
    bi_show_hex(z);
    printf("):");
    printf("\n  print(\"PASS\\n\")\n");
    printf("else :");
    printf("\n  print(\"====================WRONG====================\\n\")");
    printf("\n  exit()");
}
void verfication_barret(bigint* z, bigint* x, bigint* y) {
    printf("if ((");
    bi_show_hex(x);
    printf(") %% (");        //python 언어 호환을 위함
    bi_show_hex(y);
    printf(") == ");
    bi_show_hex(z);
    printf("):");
    printf("\n  print(\"PASS\\n\")\n");
    printf("else :");
    printf("\n  print(\"====================WRONG====================\\n\")");
    printf("\n  exit()");
}
void verfication_DIVISION(bigint* z1, bigint* z2, bigint* x, bigint* y) {
    printf("if ((");
    bi_show_hex(z1);
    printf(") * (");
    bi_show_hex(y);
    printf(") + (");
    bi_show_hex(z2);
    printf(") == ");
    bi_show_hex(x);
    printf("):");
    printf("\n  print(\"PASS\\n\")\n");
    printf("else :");
    printf("\n  print(\"====================WRONG====================\\n\")");
    printf("\n  exit()");
}
void verfication_l2r(bigint* z, bigint* x, bigint* n, bigint* mod) {
    printf("if ((");
    bi_show_hex(x);
    printf(") ** (");
    bi_show_hex(n);
    printf(") %% (");
    bi_show_hex(mod);
    printf(") == ");
    bi_show_hex(z);
    printf("):");
    printf("\n  print(\"PASS\\n\")\n");
    printf("else :");
    printf("\n  print(\"====================WRONG====================\\n\")");
    printf("\n  exit()");
}

void check_add() {
    uint8_t buffer[32]; // 예를 들어 32 바이트의 난수를 생성하려면 버퍼 크기를 조절하세요.
    randombytes(buffer, sizeof(buffer));
    // AES seed expander 초기화
    AES_XOF_struct ctx;
    uint8_t seed[32];
    uint8_t diversifier[8];  // 다이버시파이어는 필요에 따라 무작위로 생성할 수 있습니다.
    size_t maxlen = 1024 * 1024;    // 생성할 난수의 최대 길이 (예: 1024 * 1024 바이트)

    memcpy(seed, buffer, 32);
    seedexpander_init(&ctx, seed, diversifier, maxlen);
    srand(time(NULL));

    bigint* x = NULL;
    bigint* y = NULL;
    bigint* z = NULL;

    word xarr[20];
    word yarr[20];

    uint64_t rd1 = 0;
    uint64_t rd2 = 0;
    uint64_t total = 0;
    
    for (int iter = 0; iter < 10000; iter++) {
        int ylen = rand() % 9 + 1;
        int xlen = rand() % 9 + 1;

        bi_new(&x, xlen);
        bi_new(&y, ylen);
        bi_new(&z, ylen + xlen);

        seedexpander(&ctx, (uint8_t*)xarr, xlen * sizeof(word));
        seedexpander(&ctx, (uint8_t*)yarr, ylen * sizeof(word));

        bi_set_by_array(&x, NON_NEGATIVE, xarr, xlen);
        bi_set_by_array(&y, NON_NEGATIVE, yarr, ylen);

        x->sign = rand() % 2;
        y->sign = rand() % 2;
        ////rd1 = __rdtsc();
        add_bigint(&z, x, y);
        bi_refine(x);
        bi_refine(y);
        bi_refine(z);
        ////rd2 = __rdtsc();
        //total += (rd2 - rd1);
        printf("\n"); verfication_add(z, x, y);


        bi_delete(&x);
        bi_delete(&y);
        bi_delete(&z);
    }
    //printf("total clock cycle for 10000 random case : %llu", total);

  }
void check_sub() {
    uint8_t buffer[32]; // 예를 들어 32 바이트의 난수를 생성하려면 버퍼 크기를 조절하세요.
    randombytes(buffer, sizeof(buffer));
    // AES seed expander 초기화
    AES_XOF_struct ctx;
    uint8_t seed[32];
    uint8_t diversifier[8];  // 다이버시파이어는 필요에 따라 무작위로 생성할 수 있습니다.
    size_t maxlen = 1024 * 1024;    // 생성할 난수의 최대 길이 (예: 1024 * 1024 바이트)

    memcpy(seed, buffer, 32);
    seedexpander_init(&ctx, seed, diversifier, maxlen);

    srand(time(NULL));

    bigint* x = NULL;
    bigint* y = NULL;
    bigint* z = NULL;

    word xarr[20];
    word yarr[20];

    uint64_t rd1=0;
    uint64_t rd2=0;
    uint64_t total = 0;
    for (int iter = 0; iter < 10000; iter++) {
        int ylen = 8;//rand() % 9 + 1;
        int xlen = 8;//rand() % 9 + 1;

        bi_new(&x, xlen);
        bi_new(&y, ylen);
        bi_new(&z, ylen + xlen);

        seedexpander(&ctx, (uint8_t*)xarr, xlen * sizeof(word));
        seedexpander(&ctx, (uint8_t*)yarr, ylen * sizeof(word));

        bi_set_by_array(&x, NON_NEGATIVE, xarr, xlen);
        bi_set_by_array(&y, NON_NEGATIVE, yarr, ylen);

        //x->sign = rand() % 2;
        //y->sign = rand() % 2;
        
        ////rd1 = __rdtsc();
        sub_bigint(&z, x, y);
        ////rd2 = __rdtsc();
        total += (rd2 - rd1);
        //printf("\n"); verfication_sub(z, x, y);
        bi_delete(&x);
        bi_delete(&y);
        bi_delete(&z);
    }
    printf("total clock cycle for 10000 random case : %lu", total);
}
void check_schoolbook_mul() {
    uint8_t buffer[32]; // 예를 들어 32 바이트의 난수를 생성하려면 버퍼 크기를 조절하세요.
    randombytes(buffer, sizeof(buffer));
    // AES seed expander 초기화
    AES_XOF_struct ctx;
    uint8_t seed[32];
    uint8_t diversifier[8];  // 다이버시파이어는 필요에 따라 무작위로 생성할 수 있습니다.
    size_t maxlen = 1024 * 1024;    // 생성할 난수의 최대 길이 (예: 1024 * 1024 바이트)

    memcpy(seed, buffer, 32);
    seedexpander_init(&ctx, seed, diversifier, maxlen);
    srand(time(NULL));

    bigint* x = NULL;
    bigint* y = NULL;
    bigint* z = NULL;

    word xarr[20];
    word yarr[20];
    uint64_t rd1 = 0;
    uint64_t rd2 = 0;
    uint64_t total = 0;
    
    for (int iter = 0; iter < 10000; iter++) {
        int ylen = 8;//rand() % 9 + 1;
        int xlen = 8;//rand() % 9 + 1;

        bi_new(&x, xlen);
        bi_new(&y, ylen);
        bi_new(&z, ylen + xlen);

        seedexpander(&ctx, (uint8_t*)xarr, xlen * sizeof(word));
        seedexpander(&ctx, (uint8_t*)yarr, ylen * sizeof(word));

        bi_set_by_array(&x, NON_NEGATIVE, xarr, xlen);
        bi_set_by_array(&y, NON_NEGATIVE, yarr, ylen);

        x->sign = rand() % 2;
        y->sign = rand() % 2;
        ////rd1 = __rdtsc();
        mul_schoolBook(&z, x, y);
        ////rd2 = __rdtsc();
        total += (rd2 - rd1);

        //printf("\n"); verfication_mul(z, x, y);

        bi_delete(&x);
        bi_delete(&y);
        bi_delete(&z);
    }
    printf("total clock cycle for 10000 random case : %lu", total);

}
void check_improved_mul() {
    uint8_t buffer[32]; // 예를 들어 32 바이트의 난수를 생성하려면 버퍼 크기를 조절하세요.
    randombytes(buffer, sizeof(buffer));
    // AES seed expander 초기화
    AES_XOF_struct ctx;
    uint8_t seed[32];
    uint8_t diversifier[8];  // 다이버시파이어는 필요에 따라 무작위로 생성할 수 있습니다.
    size_t maxlen = 1024 * 1024;    // 생성할 난수의 최대 길이 (예: 1024 * 1024 바이트)

    memcpy(seed, buffer, 32);
    seedexpander_init(&ctx, seed, diversifier, maxlen);
    srand(time(NULL));

    bigint* x = NULL;
    bigint* y = NULL;
    bigint* z = NULL;

    word xarr[20];
    word yarr[20];
    uint64_t rd1 = 0;
    uint64_t rd2 = 0;
    uint64_t total = 0;

    for (int iter = 0; iter < 10000; iter++) {
        int ylen = 8;//rand() % 9 + 1;
        int xlen = 8;//rand() % 9 + 1;

        bi_new(&x, xlen);
        bi_new(&y, ylen);

        seedexpander(&ctx, (uint8_t*)xarr, xlen * sizeof(word));
        seedexpander(&ctx, (uint8_t*)yarr, ylen * sizeof(word));

        bi_set_by_array(&x, NON_NEGATIVE, xarr, xlen);
        bi_set_by_array(&y, NON_NEGATIVE, yarr, ylen);

        x->sign = rand() % 2;
        y->sign = rand() % 2;

        ////rd1 = __rdtsc();

        improved_text_mul_bigint(&z, x, y);
        ////rd2 = __rdtsc();
        total += (rd2 - rd1);
        //printf("\n"); verfication_mul(z, x, y);


        bi_delete(&x);
        bi_delete(&y);
        bi_delete(&z);
    }    printf("total clock cycle for 10000 random case : %lu", total);

}
void check_SQR() {
    uint8_t buffer[32]; // 예를 들어 32 바이트의 난수를 생성하려면 버퍼 크기를 조절하세요.
    randombytes(buffer, sizeof(buffer));
    // AES seed expander 초기화
    AES_XOF_struct ctx;
    uint8_t seed[32];
    uint8_t diversifier[8];  // 다이버시파이어는 필요에 따라 무작위로 생성할 수 있습니다.
    size_t maxlen = 1024 * 1024;    // 생성할 난수의 최대 길이 (예: 1024 * 1024 바이트)

    memcpy(seed, buffer, 32);
    seedexpander_init(&ctx, seed, diversifier, maxlen);
    srand(time(NULL));

    bigint* x = NULL;
    bigint* z = NULL;

    word xarr[20];
    word yarr[20];
    uint64_t rd1 = 0;
    uint64_t rd2 = 0;
    uint64_t total = 0;

    for (int iter = 0; iter < 10000; iter++) {
        int xlen = 8;// rand() % 9 + 1;

        bi_new(&x, xlen);

        seedexpander(&ctx, (uint8_t*)xarr, xlen * sizeof(word));
        bi_set_by_array(&x, NON_NEGATIVE, xarr, xlen);

        x->sign = rand() % 2;

        ////rd1 = __rdtsc();
        SQR_schoolBook(&z, x);
        ////rd2 = __rdtsc();
        total += (rd2 - rd1);
        //printf("\n"); verfication_mul(z, x, x);


        bi_delete(&x);
        bi_delete(&z);
    }    printf("total clock cycle for 10000 random case : %lu", total);

}
void check_karatsuba() {

    uint8_t buffer[32]; // 예를 들어 32 바이트의 난수를 생성하려면 버퍼 크기를 조절하세요.
    randombytes(buffer, sizeof(buffer));
    AES_XOF_struct ctx;
    uint8_t seed[32];
    uint8_t diversifier[8];  // 다이버시파이어는 필요에 따라 무작위로 생성할 수 있습니다.
    size_t maxlen = 1024 * 1024;    // 생성할 난수의 최대 길이 (예: 1024 * 1024 바이트)

    memcpy(seed, buffer, 32);
    seedexpander_init(&ctx, seed, diversifier, maxlen);
    srand(time(NULL));

    bigint* x = NULL;
    bigint* y = NULL;
    bigint* z = NULL;

    word xarr[20];
    word yarr[20];
    uint64_t rd1 = 0;
    uint64_t rd2 = 0;
    uint64_t total = 0;

    for (int iter = 0; iter < 10000; iter++) {
        int ylen = 8;//rand() % 9 + 1;
        int xlen = 8;//rand() % 9 + 1;

        bi_new(&x, xlen);
        bi_new(&y, ylen);
        bi_new(&z, ylen + xlen);

        seedexpander(&ctx, (uint8_t*)xarr, xlen * sizeof(word));
        seedexpander(&ctx, (uint8_t*)yarr, ylen * sizeof(word));
        bi_set_by_array(&x, NON_NEGATIVE, xarr, xlen);
        bi_set_by_array(&y, NON_NEGATIVE, yarr, ylen);

        x->sign = rand() % 2;
        y->sign = rand() % 2;

        //rd1 = __rdtsc();
        karatsuba_mul(&z, x, y);
        //rd2 = __rdtsc();
        total += (rd2 - rd1);
        //printf("\n"); verfication_mul(z, x, y);


        bi_delete(&x);
        bi_delete(&y);
        bi_delete(&z);
    }    printf("total clock cycle for 10000 random case : %lu", total);

}
void check_DIVISION() {
    uint8_t buffer[32]; // 예를 들어 32 바이트의 난수를 생성하려면 버퍼 크기를 조절하세요.
    randombytes(buffer, sizeof(buffer));
    AES_XOF_struct ctx;
    uint8_t seed[32];
    uint8_t diversifier[8];  // 다이버시파이어는 필요에 따라 무작위로 생성할 수 있습니다.
    size_t maxlen = 1024 * 1024;    // 생성할 난수의 최대 길이 (예: 1024 * 1024 바이트)

    memcpy(seed, buffer, 32);
    seedexpander_init(&ctx, seed, diversifier, maxlen);
    srand(time(NULL));

    bigint* x = NULL;
    bigint* y = NULL;
    bigint* R = NULL;
    bigint* Q = NULL;

    word xarr[20];
    word yarr[20];

    uint64_t rd1 = 0;
    uint64_t rd2 = 0;
    uint64_t total = 0;

    for (int iter = 0; iter < 10000; iter++) {
        int ylen = 8;//rand() % 9 + 1;
        int xlen = 8;//rand() % 9 + 1;

        bi_new(&x, xlen);
        bi_new(&y, ylen);
        seedexpander(&ctx, (uint8_t*)xarr, xlen * sizeof(word));
        seedexpander(&ctx, (uint8_t*)yarr, ylen * sizeof(word));

        bi_set_by_array(&x, NON_NEGATIVE, xarr, xlen);
        bi_set_by_array(&y, NON_NEGATIVE, yarr, ylen);

        //rd1 = __rdtsc();
        DIVISION(&Q, &R, x, y);
        //rd2 = __rdtsc();
        total += (rd2 - rd1);
        //printf("\n"); verfication_DIVISION(Q, R, x, y);

        bi_delete(&x);
        bi_delete(&y);
        bi_delete(&Q);
        bi_delete(&R);
    }    printf("total clock cycle for 10000 random case : %lu", total);

}
void check_barret() {
    uint8_t buffer[32]; // 예를 들어 32 바이트의 난수를 생성하려면 버퍼 크기를 조절하세요.
    randombytes(buffer, sizeof(buffer));
    // AES seed expander 초기화
    AES_XOF_struct ctx;
    uint8_t seed[32];
    uint8_t diversifier[8];  // 다이버시파이어는 필요에 따라 무작위로 생성할 수 있습니다.
    size_t maxlen = 1024 * 1024;    // 생성할 난수의 최대 길이 (예: 1024 * 1024 바이트)

    memcpy(seed, buffer, 32);
    seedexpander_init(&ctx, seed, diversifier, maxlen);

    srand(time(NULL));

    bigint* x = NULL;
    bigint* y = NULL;
    bigint* z = NULL;

    word xarr[20];
    word yarr[20];

    for (int iter = 0; iter < 1000; iter++) {
        int ylen = 4;// rand() % 9 + 1;
        int xlen = ylen * 2;

        bi_new(&x, xlen);
        bi_new(&y, ylen);
        bi_new(&z, ylen + xlen);

        seedexpander(&ctx, (uint8_t*)xarr, xlen * sizeof(word));
        seedexpander(&ctx, (uint8_t*)yarr, ylen * sizeof(word));

        bi_set_by_array(&x, NON_NEGATIVE, xarr, xlen);
        bi_set_by_array(&y, NON_NEGATIVE, yarr, ylen);

        bi_barrett_reduction(&z, x, y);

        printf("\n"); verfication_barret(z, x, y);

        bi_delete(&x);
        bi_delete(&y);
        bi_delete(&z);
    }
}
void check_L2R() {
    uint8_t buffer[32]; // 예를 들어 32 바이트의 난수를 생성하려면 버퍼 크기를 조절하세요.
    randombytes(buffer, sizeof(buffer));
    // AES seed expander 초기화
    AES_XOF_struct ctx;
    uint8_t seed[32];
    uint8_t diversifier[8];  // 다이버시파이어는 필요에 따라 무작위로 생성할 수 있습니다.
    size_t maxlen = 1024 * 1024;    // 생성할 난수의 최대 길이 (예: 1024 * 1024 바이트)

    memcpy(seed, buffer, 32);
    seedexpander_init(&ctx, seed, diversifier, maxlen);


    srand(time(NULL));

    bigint* x = NULL;
    bigint* y = NULL;
    bigint* z = NULL;
    bigint* n = NULL;

    word xarr[20];
    word yarr[20];
    word narr[10];
    uint64_t rd1 = 0;
    uint64_t rd2 = 0;
    uint64_t total = 0;

    for (int iter = 0; iter < 1000; iter++) {
        int ylen = 1;
        int xlen = 8;// rand() % 9 + 1;
        int nlen = rand() % 1 + 1;

        bi_new(&x, xlen);
        bi_new(&y, ylen);
        bi_new(&z, ylen + xlen);
        bi_new(&n, nlen);

        seedexpander(&ctx, (uint8_t*)xarr, xlen * sizeof(word));
        seedexpander(&ctx, (uint8_t*)yarr, ylen * sizeof(word));
        seedexpander(&ctx, (uint8_t*)narr, nlen * sizeof(word));
        yarr[0] = 0x00000003;
        bi_set_by_array(&x, NON_NEGATIVE, xarr, xlen);
        bi_set_by_array(&y, NON_NEGATIVE, yarr, ylen);
        bi_set_by_array(&n, NON_NEGATIVE, narr, nlen);
        //rd1 = __rdtsc();
        bi_LtR(&z, x, y, n);
        //rd2 = __rdtsc();
        total += (rd2 - rd1);
  
        //printf("\n"); verfication_l2r(z, x, y, n);

        bi_delete(&x);
        bi_delete(&y);
        bi_delete(&z);
        bi_delete(&n);
    }    printf("total clock cycle for 10000 random case : %lu", total);

}
int main() {
    //check_add();
    //check_sub();
    //check_schoolbook_mul();
    //check_improved_mul();
    //check_karatsuba();
    //check_SQR();
    //check_DIVISION();
    check_barret();
    //check_L2R();
}
