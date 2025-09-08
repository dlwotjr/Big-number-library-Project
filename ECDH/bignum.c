#include "bignum.h"

#define SIZE 8
uint32_t p256[SIZE] = {0xFFFFFFFF, 0x00000001, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
uint32_t p256_i[SIZE] = {0x00000000, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 0x00000001};
uint32_t R_sqr[SIZE] = {0x00000004, 0xfffffffd, 0xffffffff, 0xfffffffe, 0xfffffffb, 0xffffffff, 0x00000000, 0x00000003};
uint32_t p256_prime = 0x00000001;

#define GETU32(pt) (((uint32_t)(pt)[0] << 24) ^ ((uint32_t)(pt)[1] << 16) ^ ((uint32_t)(pt)[2] <<  8) ^ ((uint32_t)(pt)[3]))
#define PUTU32(ct, st) { (ct)[0] = (uint8_t)((st) >> 24); (ct)[1] = (uint8_t)((st) >> 16); (ct)[2] = (uint8_t)((st) >>  8); (ct)[3] = (uint8_t)(st); }

// ASCII 값을 HEX로 변경해주는 함수
uint32_t ascii_to_hex(const uint8_t c) {
    uint32_t num = (uint32_t)c;
    if (num < 58 && num > 47) {
		return num - 48;
	}
	if (num < 103 && num > 96) {
		return num - 87;
	}
    if (num < 71 && num > 64) {
        return num - 55;
	}
	return num;
}

//HEX 값을 ASCII로 변경해주는 함수
uint32_t hex_to_ascii(const uint8_t c) {
    uint32_t num = (uint32_t)c;
    if (num < 10 && num >= 0) {
		return num + 48;
	}
	if (num < 16 && num > 9) {
		return num + 55;
	}
	return num;
}

// 바이트 b[16] --> 워드 state[4]
void byte2state(const uint8_t b[16], uint32_t state[4]) {
	state[0] = GETU32(b);
	state[1] = GETU32(b + 4);
	state[2] = GETU32(b + 8);
	state[3] = GETU32(b + 12);
}

// 워드 state[4] --> 바이트 b[16]
void state2byte(const uint32_t state[4], uint8_t b[16]) {
	PUTU32(b, state[0]);
	PUTU32(b + 4, state[1]);
	PUTU32(b + 8, state[2]);
	PUTU32(b + 12, state[3]);
}

void add_int(const uint32_t a, const uint32_t b, uint32_t *result, int* carry){
    *result = a + b + *carry;
    if (*carry == 1){
        if (*result <= a);
        else *carry = 0;
    }
    else{
        if (*result < a) *carry = 1;
        else;
    }
}

void add_bigint(uint32_t a[SIZE], uint32_t b[SIZE], uint32_t result[SIZE]){
    int carry = 0;

    for (int i = 0; i < SIZE; i++)
        add_int(a[SIZE-1 - i], b[SIZE-1 - i], &result[SIZE-1 - i], &carry);
    if (carry == 1){
        carry = 0;
        for (int i = 0; i < SIZE; i++)
            add_int(result[SIZE-1 - i], p256_i[SIZE-1 - i], &result[SIZE-1 - i], &carry);
    }

    // for (int i = 0; i < SIZE; i++)
    //     printf("%08x", result[i]);
    // printf("\n");
}

void add_bigint_16(uint32_t a[SIZE*2], uint32_t b[SIZE*2], uint32_t result[SIZE*2]){
    int carry = 0;

    for (int i = 0; i < SIZE*2; i++)
        add_int(a[SIZE*2-1 - i], b[SIZE*2-1 - i], &result[SIZE*2-1 - i], &carry);
    // if (carry == 1){
    //     carry = 0;
    //     for (int i = 0; i < SIZE*2; i++)
    //         add_int(result[SIZE*2-1 - i], p256_i[SIZE*2-1 - i], &result[SIZE*2-1 - i], &carry);
    // }

    // for (int i = 0; i < SIZE; i++)
    //     printf("%08x", result[i]);
    // printf("\n");
}

void sub_int(const uint32_t a, const uint32_t b, uint32_t *result, int *borrow){
    *result = a - b - *borrow;
    if (a < b) *borrow = 1;
    else if (a == b && *borrow == 1) *borrow = 1;
    else *borrow = 0;
}

void sub_bigint(uint32_t a[SIZE], uint32_t b[SIZE], uint32_t result[SIZE]){
    int borrow = 0;

    for (int i = 0; i < SIZE; i++)
        sub_int(a[SIZE-1 - i], b[SIZE-1 - i], &result[SIZE-1 - i], &borrow);
    if (borrow == 1){
        borrow = 0;
        for (int i = 0; i < SIZE; i++)
            sub_int(result[SIZE-1 - i], p256_i[SIZE-1 - i], &result[SIZE-1 - i], &borrow);
    }

    // for (int i = 0; i < SIZE; i++)
    //     printf("%08x", result[i]);
    // printf("\n");
}

void mul_bigint_os(const uint32_t a[SIZE], const uint32_t b[SIZE], uint32_t result[SIZE*2]){
    uint64_t mul_value = 0;
    uint64_t mul_value_U = 0;
    uint64_t mul_value_V = 0;
    for (int i = 0; i < SIZE; i++){
        mul_value_U = 0;
        for (int j = 0; j < SIZE; j++){
            mul_value = result[SIZE*2-1 - i - j] + a[SIZE-1 - i] * (uint64_t)b[SIZE-1 - j] + mul_value_U;
            mul_value_U = mul_value >> 32;
            mul_value_V = mul_value;
            result[SIZE*2-1 - i - j] = mul_value_V;
        }
        result[SIZE-1 - i] = mul_value_U;
    }

    // for (int i = 0; i < SIZE * 2; i++)
    //     printf("%08x", result[i]);
    // printf("\n");
}

void mul_bigint_ps(const uint32_t a[SIZE], const uint32_t b[SIZE], uint32_t result[SIZE*2]){
    uint64_t mul_value = 0;
    uint64_t mul_value_U = 0;
    uint64_t mul_value_V = 0;
    uint32_t mul_value_R[3] = {0x00,};
    for (int k = 0; k < SIZE*2-1; k++){
        int carry = 0;
        if (k >= SIZE){
            for (int i = k-(SIZE-1); i < SIZE; i++){
                mul_value = (uint64_t)a[SIZE-1 - i] * (uint64_t)b[SIZE-1 - k + i];
                mul_value_U = mul_value >> 32;
                mul_value_V = mul_value;
                add_int(mul_value_R[0] , mul_value_V, &mul_value_R[0], &carry);
                add_int(mul_value_R[1], mul_value_U, &mul_value_R[1], &carry);
                add_int(mul_value_R[2], 0, &mul_value_R[2], &carry);
            }
        }
        else{
            for (int i = 0; i < k+1; i++){
                mul_value = (uint64_t)a[SIZE-1 - i] * (uint64_t)b[SIZE-1 - k + i];
                mul_value_U = mul_value >> 32;
                mul_value_V = mul_value;
                add_int(mul_value_R[0] , mul_value_V, &mul_value_R[0], &carry);
                add_int(mul_value_R[1], mul_value_U, &mul_value_R[1], &carry);
                add_int(mul_value_R[2], 0, &mul_value_R[2], &carry);
            }
        }
        result[SIZE*2-1 - k] = mul_value_R[0];
        mul_value_R[0] = mul_value_R[1];
        mul_value_R[1] = mul_value_R[2];
        mul_value_R[2] = 0;
    }
    result[0] = mul_value_R[0];
}

void sqr_bigint(const uint32_t a[SIZE], uint32_t result[SIZE*2]){
    uint64_t mul_value = 0;
    uint64_t mul_value_U = 0;
    uint64_t mul_value_V = 0;
    uint32_t mul_value_R[3] = {0x00,};
    for (int k = 0; k < SIZE*2-1; k++){
        int carry = 0;
        if (k >= SIZE){
            for (int i = SIZE-1; i >= (k+1)/2; i--){
                mul_value = (uint64_t)a[SIZE-1 - i] * a[SIZE-1 - k + i];
                if(i > (k - i)){
                    if(mul_value >> 63 == 0) carry = 0;
                    else carry = 1;
                    mul_value <<= 1;
                    add_int(mul_value_R[2], 0, &mul_value_R[2], &carry);
                }
                mul_value_U = mul_value >> 32;
                mul_value_V = mul_value;
                add_int(mul_value_R[0] , mul_value_V, &mul_value_R[0], &carry);
                add_int(mul_value_R[1], mul_value_U, &mul_value_R[1], &carry);
                add_int(mul_value_R[2], 0, &mul_value_R[2], &carry);
            }
        }
        else{
            for (int i = 0; i < k/2 + 1; i++){
                mul_value = (uint64_t)a[SIZE-1 - i] * a[SIZE-1 - k + i];
                if (i < (k - i)){
                    if(mul_value >> 63 == 0) carry = 0;
                    else carry = 1;
                    mul_value <<= 1;
                    add_int(mul_value_R[2], 0, &mul_value_R[2], &carry);
                }
                mul_value_U = mul_value >> 32;
                mul_value_V = mul_value;
                add_int(mul_value_R[0] , mul_value_V, &mul_value_R[0], &carry);
                add_int(mul_value_R[1], mul_value_U, &mul_value_R[1], &carry);
                add_int(mul_value_R[2], 0, &mul_value_R[2], &carry);
            }
        }
        result[SIZE*2-1 - k] = mul_value_R[0];    
        mul_value_R[0] = mul_value_R[1];
        mul_value_R[1] = mul_value_R[2];
        mul_value_R[2] = 0;
    }
    result[0] = mul_value_R[0];

    // for (int i = 0; i < SIZE * 2; i++)
    //     printf("%08x", result[i]);
    // printf("\n");
}

void redn_bigint_fast(const uint32_t a[SIZE*2], uint32_t result[SIZE]){ // SIZE 적용 못함
    uint32_t s1[8] = {a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15]};
    uint32_t s2[8] = {a[0], a[1], a[2], a[3], a[4], 0x00, 0x00, 0x00};
    uint32_t s3[8] = {0x00, a[0], a[1], a[2], a[3] ,0x00, 0x00, 0x00};
    uint32_t s4[8] = {a[0], a[1], 0x00, 0x00, 0x00, a[5], a[6], a[7]};
    uint32_t s5[8] = {a[7], a[2], a[0], a[1], a[2], a[4], a[5], a[6]};
    uint32_t s6[8] = {a[5], a[7], 0x00, 0x00, 0x00, a[2], a[3], a[4]};
    uint32_t s7[8] = {a[4], a[6], 0x00, 0x00, a[0], a[1], a[2], a[3]};
    uint32_t s8[8] = {a[3], 0x00, a[5], a[6], a[7], a[0], a[1], a[2]};
    uint32_t s9[8] = {a[2], 0x00, a[4], a[5], a[6], 0x00, a[0], a[1]};

    add_bigint(s1, s2, result);
    add_bigint(result, s2, result);
    add_bigint(result, s3, result);
    add_bigint(result, s3, result);
    add_bigint(result, s4, result);
    add_bigint(result, s5, result);
    sub_bigint(result, s6, result);
    sub_bigint(result, s7, result);
    sub_bigint(result, s8, result);
    sub_bigint(result, s9, result);

    // for (int i = 0; i < SIZE; i++)
    //     printf("%08x", result[i]);
    // printf("\n");
}

int compare_bigint(const uint32_t a[SIZE], const uint32_t b[SIZE]){
    int compare = 0;
    for(int i = 0; i < SIZE; i++){
        if(a[i] == b[i]) continue;
        else if(a[i] > b[i]){
            compare = 1;
            break;
        }
        else{
            compare = 2;
            break;
        }
    }
    return compare;
}

void redn_bigint_mont(const uint32_t a[SIZE*2], uint32_t result[SIZE]){
    // b = 2^32, R = 2^256
    uint64_t mul_value = 0;
    uint32_t u = 0x00;
    uint32_t ub[SIZE] = {0x00,};
    uint32_t umb[SIZE*2] = {0x00,};
    uint32_t result_16bytes[SIZE*2] = {0x00,};
    for(int i = 0; i < SIZE*2; i++) result_16bytes[i] = a[i];

    // montgomery
    // for(int j = 0; j < 2; j++){
    //     for(int i = 0; i < SIZE; i++){
    //         mul_value = result_16bytes[SIZE*2-1 - i] * p256_prime;
    //         u = mul_value;
    //         ub[SIZE-1 - i] = u;
    //         mul_bigint_ps(ub, p256, umb);
    //         add_bigint_16(result_16bytes, umb, result_16bytes);
    //         ub[SIZE-1 - i] = 0x00;
    //     }
    //     for(int i = 0; i < SIZE; i++) result[i] = result_16bytes[i];
    //     if(compare_bigint(result, p256) != 2) sub_bigint(result, p256, result);
    //     if(j == 0) mul_bigint_ps(result, R_sqr, result_16bytes);
    // }

    // fast montgomery (SIZE 적용 못함)
    for(int j = 0; j < 2; j++){
        for(int i = 0; i < SIZE; i++){
            ub[SIZE-1 - i] = result_16bytes[SIZE*2-1 - i];
            mul_bigint_ps(ub, p256, umb);
            add_bigint_16(result_16bytes, umb, result_16bytes);
            ub[SIZE-1 - i] = 0x00;
        }
        for(int i = 0; i < SIZE; i++) result[i] = result_16bytes[i];
        if(compare_bigint(result, p256) != 2) sub_bigint(result, p256, result);

        if(j == 0) mul_bigint_ps(result, R_sqr, result_16bytes);
    }

    // for (int i = 0; i < SIZE; i++)
    //     printf("%08x", result[i]);
    // printf("\n");
}

void div_bigint_by_bitshift(const uint32_t a[SIZE], uint32_t b[SIZE], const uint32_t loop_cnt){
    memcpy(b, a, sizeof(uint32_t) * SIZE);
    for(int j = 0; j < loop_cnt; j++){
        if(b[7] & 0x01 != 0x00){ // 홀수일때
            int carry = 0;
            for (int i = 0; i < SIZE; i++)
                add_int(b[SIZE - 1 - i], p256[SIZE - 1 - i], &b[SIZE - 1 - i], &carry);
            for (int i = 0; i < SIZE - 1; i++){
                b[SIZE - 1 - i] >>= 1;
                b[SIZE - 1 - i] ^= (b[SIZE - 2 - i] << 31);
            }
            b[0] >>= 1;
            b[0] ^= carry << 31;
        }
        else{ // 짝수일때
            for(int i = 0; i < SIZE-1; i++){
                b[SIZE-1 - i] >>= 1;
                b[SIZE-1 - i] ^= (b[SIZE-2 - i] << 31);
            }
            b[0] >>= 1;
        }
    }
}

void mul_bigint_by_bitshift(const uint32_t a[SIZE], uint32_t b[SIZE], const uint32_t loop_cnt){
    memcpy(b, a, sizeof(uint32_t) * SIZE);
    for(int j = 0; j < loop_cnt; j++){
        uint32_t tmp = b[0] >> 31;
        for(int i = 0; i < SIZE-1; i++){
            b[i] <<= 1;
            b[i] ^= (b[i + 1] >> 31);
        }
        b[7] <<= 1;
        if (tmp)
            add_bigint(b, p256_i, b);
    }
}

void inv_bigint_EEA(const uint32_t a[SIZE], uint32_t result[SIZE]){
    uint32_t u[SIZE] = {0x00,};
    uint32_t v[SIZE] = {0x00,};
    uint32_t q[SIZE] = {0x00,};
    uint32_t r[SIZE] = {0x00,};
    uint32_t x1[SIZE] = {0x00,};
    uint32_t x2[SIZE] = {0x00,};
    uint32_t bigint1[SIZE] = {0x00,};
    int compare = 0;

    memcpy(u, a, sizeof(uint32_t) * SIZE);
    memcpy(v, p256, sizeof(uint32_t) * SIZE);
    x1[SIZE-1] += 0x01;
    bigint1[SIZE-1] += 0x01;

    while(compare_bigint(u, bigint1) != 0 && compare_bigint(v, bigint1) != 0){
        while((u[SIZE-1] & 0x01) == 0){
            div_bigint_by_bitshift(u, u, 1);
            div_bigint_by_bitshift(x1, x1, 1);
        }
        while((v[SIZE-1] & 0x01) == 0){
            div_bigint_by_bitshift(v, v, 1);
            div_bigint_by_bitshift(x2, x2, 1);
        }        
        if(compare_bigint(u, v) != 2){
            sub_bigint(u, v, u);
            sub_bigint(x1, x2, x1);
        }
        else{
            sub_bigint(v, u, v);
            sub_bigint(x2, x1, x2);
        }
    }
    if(compare_bigint(u, bigint1) == 0) memcpy(result, x1, sizeof(uint32_t) * SIZE);
    else memcpy(result, x2, sizeof(uint32_t) * SIZE);

    // for (int i = 0; i < SIZE; i++)
    //     printf("%08x", result[i]);
    // printf("\n");
}

void sqr_bigint_loop(const uint32_t a[SIZE], uint32_t result[SIZE], const uint32_t loop_cnt){
    uint32_t result_16bytes[SIZE*2] = {0x00,};
    sqr_bigint(a, result_16bytes);
    redn_bigint_fast(result_16bytes, result);
    for(int i = 0; i < loop_cnt-1; i++){
        sqr_bigint(result, result_16bytes);
        redn_bigint_fast(result_16bytes, result);
    }
}

void inv_bigint_FLT(const uint32_t a[SIZE], uint32_t result[SIZE]){ // SIZE 적용 못함
    uint32_t result_tmp[SIZE] = {0x00,};
    uint32_t z[SIZE] = {0x00};
    uint32_t z3[SIZE] = {0x00,};
    uint32_t z15[SIZE] = {0x00,};
    uint32_t t0[SIZE] = {0x00,};
    uint32_t t1[SIZE] = {0x00,};
    uint32_t t2[SIZE] = {0x00,};
    uint32_t t3[SIZE] = {0x00,};
    uint32_t t4[SIZE] = {0x00,};
    uint32_t t5[SIZE] = {0x00,};
    uint32_t result_16bytes[SIZE*2] = {0x00,};

    memcpy(z, a, sizeof(uint32_t) * SIZE); // Z

    sqr_bigint_loop(z, result_tmp, 1); // Z^2
    mul_bigint_ps(z, result_tmp, result_16bytes);
    redn_bigint_fast(result_16bytes, z3); // Z3

    sqr_bigint_loop(z3, result_tmp, 2); // Z3^2^2
    mul_bigint_ps(result_tmp, z3, result_16bytes);
    redn_bigint_fast(result_16bytes, z15); // Z15

    sqr_bigint_loop(z15, result_tmp, 2); // Z15^2^2
    mul_bigint_ps(result_tmp, z3, result_16bytes);
    redn_bigint_fast(result_16bytes, t0); // t0

    sqr_bigint_loop(t0, result_tmp, 6);
    mul_bigint_ps(result_tmp, t0, result_16bytes);
    redn_bigint_fast(result_16bytes, t1); // t1

    sqr_bigint_loop(t1, result_tmp, 12);
    mul_bigint_ps(result_tmp, t1, result_16bytes);
    redn_bigint_fast(result_16bytes, result_tmp);
    sqr_bigint_loop(result_tmp, result_tmp, 6);
    mul_bigint_ps(result_tmp, t0, result_16bytes);
    redn_bigint_fast(result_16bytes, t2); // t2

    sqr_bigint_loop(t2, result_tmp, 2);
    mul_bigint_ps(result_tmp, z3, result_16bytes);
    redn_bigint_fast(result_16bytes, t3); // t3

    sqr_bigint_loop(t3, result_tmp, 32);
    mul_bigint_ps(result_tmp, a, result_16bytes);
    redn_bigint_fast(result_16bytes, result_tmp);
    sqr_bigint_loop(result_tmp, t4, 96); // t4

    sqr_bigint_loop(t4, result_tmp, 32);
    mul_bigint_ps(result_tmp, t3, result_16bytes);
    redn_bigint_fast(result_16bytes, result_tmp);
    sqr_bigint_loop(result_tmp, result_tmp, 32);
    mul_bigint_ps(result_tmp, t3, result_16bytes);
    redn_bigint_fast(result_16bytes, t5); // t5

    
    sqr_bigint_loop(t5, result_tmp, 30);
    mul_bigint_ps(result_tmp, t2, result_16bytes);
    redn_bigint_fast(result_16bytes, result_tmp);
    sqr_bigint_loop(result_tmp, result_tmp, 2);
    mul_bigint_ps(result_tmp, a, result_16bytes);
    redn_bigint_fast(result_16bytes, result); // t

    // for (int i = 0; i < SIZE; i++)
    //     printf("%08x", result[i]);
    // printf("\n");
}

void mul_and_redn_bigint(uint32_t a[SIZE], uint32_t b[SIZE], uint32_t result[SIZE]){
    uint32_t result_16bytes[SIZE*2] = {0x00,};

    mul_bigint_ps(a, b, result_16bytes);
    redn_bigint_fast(result_16bytes, result);
}

void sqr_and_redn_bigint(uint32_t a[SIZE], uint32_t result[SIZE]){
    uint32_t result_16bytes[SIZE*2] = {0x00,};

    sqr_bigint(a, result_16bytes);
    redn_bigint_fast(result_16bytes, result);
}

void mul_bigint_LtR(uint32_t a[SIZE], const uint32_t b[SIZE], uint32_t result[SIZE]){
    uint32_t zero[SIZE] = {0x00,};
    memcpy(result, zero, sizeof(uint32_t) * SIZE);
    uint32_t tmp = 0x80000000;

    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < 32; j++){
            mul_bigint_by_bitshift(result, result, 1);
            if(b[i] & tmp)
                add_bigint(result, a, result);
            tmp >>= 1;
        }
        tmp = 0x80000000;
    }
}

void mul_bigint_os_4(const uint32_t a[SIZE/2], const uint32_t b[SIZE/2], uint32_t result[SIZE*2]){
    uint64_t mul_value = 0;
    uint64_t mul_value_U = 0;
    uint64_t mul_value_V = 0;
    for (int i = 0; i < SIZE/2; i++){
        mul_value_U = 0;
        for (int j = 0; j < SIZE/2; j++){
            mul_value = result[SIZE*2-1 - i - j] + a[SIZE/2-1 - i] * (uint64_t)b[SIZE/2-1 - j] + mul_value_U;
            mul_value_U = mul_value >> 32;
            mul_value_V = mul_value;
            result[SIZE*2-1 - i - j] = mul_value_V;
        }
        result[SIZE+3 - i] = mul_value_U;
    }

    // for (int i = 0; i < SIZE * 2; i++)
    //     printf("%08x", result[i]);
    // printf("\n");
}

void mul_bigint_os_5(const uint32_t a[SIZE/2+1], const uint32_t b[SIZE/2+1], uint32_t result[SIZE*2]){
    uint64_t mul_value = 0;
    uint64_t mul_value_U = 0;
    uint64_t mul_value_V = 0;
    for (int i = 0; i < SIZE/2+1; i++){
        mul_value_U = 0;
        for (int j = 0; j < SIZE/2+1; j++){
            mul_value = result[SIZE*2-1 - i - j] + a[SIZE/2 - i] * (uint64_t)b[SIZE/2 - j] + mul_value_U;
            mul_value_U = mul_value >> 32;
            mul_value_V = mul_value;
            result[SIZE*2-1 - i - j] = mul_value_V;
        }
        result[SIZE+2 - i] = mul_value_U;
    }

    // for (int i = 0; i < SIZE * 2; i++)
    //     printf("%08x", result[i]);
    // printf("\n");
}

void add_bigint_4(const uint32_t a[SIZE/2], const uint32_t b[SIZE/2], uint32_t result[SIZE/2+1]){
    int carry = 0;

    for (int i = 0; i < SIZE/2; i++)
        add_int(a[SIZE/2-1 - i], b[SIZE/2-1 - i], &result[SIZE/2 - i], &carry);
    if (carry == 1)
        result[0] = 0x01;

    // for (int i = 0; i < SIZE; i++)
    //     printf("%08x", result[i]);
    // printf("\n");
}

int compare_bigint_16(const uint32_t a[SIZE*2], const uint32_t b[SIZE*2]){
    int compare = 0;
    for(int i = 0; i < SIZE*2; i++){
        if(a[i] == b[i]) continue;
        else if(a[i] > b[i]){
            compare = 1;
            break;
        }
        else{
            compare = 2;
            break;
        }
    }
    return compare;
}

void sub_bigint_16(uint32_t a[SIZE*2], uint32_t b[SIZE*2], uint32_t result[SIZE*2]){
    int borrow = 0;
    if(compare_bigint_16(a, b) != 2){
        for (int i = 0; i < SIZE*2; i++)
            sub_int(a[SIZE*2-1 - i], b[SIZE*2-1 - i], &result[SIZE*2-1 - i], &borrow);
    }
    else{
        for (int i = 0; i < SIZE*2; i++)
            sub_int(b[SIZE*2-1 - i], a[SIZE*2-1 - i], &result[SIZE*2-1 - i], &borrow);
    }
}

void mul_bigint_karatsuba(const uint32_t a[SIZE], const uint32_t b[SIZE], uint32_t result[SIZE*2]){
    uint32_t zero[SIZE] = {0x00,};
    uint32_t z0[SIZE*2] = {0x00,};
    uint32_t z1[SIZE*2] = {0x00,};
    uint32_t z2[SIZE*2] = {0x00,};
    uint32_t z3[SIZE*2] = {0x00,};
    uint32_t a1_a2[SIZE/2+1] = {0x00,};
    uint32_t b1_b2[SIZE/2+1] = {0x00,};
    
    mul_bigint_os_4(a, b, z0);
    mul_bigint_os_4((a+4), (b+4), z1);
    add_bigint_4(a, (a+4), a1_a2);
    add_bigint_4(b, (b+4), b1_b2);
    mul_bigint_os_5(a1_a2, b1_b2, z2);
    add_bigint_16(z0, z1, z3);
    sub_bigint_16(z3, z2, z3);
    memcpy(z0, z0+8, sizeof(uint32_t)*SIZE);
    memcpy(z0+8, zero, sizeof(uint32_t)*SIZE); //shift를 memcpy로 대체
    memcpy(z3+3, z3+7, sizeof(uint32_t)*(SIZE+1));
    memcpy(z3+12, zero, sizeof(uint32_t)*SIZE/2); //shift를 memcpy로 대체
    add_bigint_16(z0, z3, result);
    add_bigint_16(result, z1, result);

    // for (int i = 0; i < 16; i++)
    //     printf("%08x", reuslt[i]);
    // printf("\n");
}

uint32_t check_add_bigint(const char *BigInt_a_file_, const char *BigInt_b_file_, const char *my_add_bigint_TV_file_){
    FILE *BigInt_a_file;
    FILE *BigInt_b_file;
    FILE *my_add_bigint_TV_file;

    BigInt_a_file = fopen(BigInt_a_file_, "rb");
    BigInt_b_file = fopen(BigInt_b_file_, "rb");
    my_add_bigint_TV_file = fopen(my_add_bigint_TV_file_, "wb");
    if (BigInt_a_file == NULL || BigInt_b_file == NULL || my_add_bigint_TV_file == NULL) { //파일을 못읽어 왔을때
		printf("ERROR");
		return 0;
	}

    fseek(BigInt_a_file, 0, SEEK_END);
    unsigned long long int file_byte = ftell(BigInt_a_file);
    fseek(BigInt_a_file, 0, SEEK_SET);

    while (ftell(BigInt_a_file) != file_byte){
        uint32_t BigInt_a[SIZE] = {0x00,};
        uint32_t BigInt_b[SIZE] = {0x00,};
        uint32_t result[SIZE] = {0x00,};
        uint8_t BigInt_a_byte[16] = {0x00,};
        uint8_t BigInt_b_byte[16] = {0x00,};
        uint8_t buffer[16] = {0x00,};
        uint8_t tmp[32] = {0x00,};
        uint8_t c1, c2, sum;
        for (int size_i = 0; size_i < 2; size_i++){
            //a값 받아오기
            for (int i = 0; i < 16; i++) {
                tmp[i * 2] = fgetc(BigInt_a_file);
                tmp[i * 2 + 1] = fgetc(BigInt_a_file);
                c1 = ascii_to_hex(tmp[i * 2]);
                c2 = ascii_to_hex(tmp[i * 2 + 1]);
                sum = c1 << 4 | c2;
                BigInt_a_byte[i] = sum;
            }
            //b값 받아오기
            for (int i = 0; i < 16; i++) {
                tmp[i * 2] = fgetc(BigInt_b_file);
                tmp[i * 2 + 1] = fgetc(BigInt_b_file);
                c1 = ascii_to_hex(tmp[i * 2]);
                c2 = ascii_to_hex(tmp[i * 2 + 1]);
                sum = c1 << 4 | c2;
                BigInt_b_byte[i] = sum;
            }

            byte2state(BigInt_a_byte, BigInt_a + size_i * 4);
            byte2state(BigInt_b_byte, BigInt_b + size_i * 4);
        }

        add_bigint(BigInt_a, BigInt_b, result);

        for (int size_i = 0; size_i < 2; size_i++){
            state2byte(result + size_i * 4, buffer);

            //16진수 형태로 넣기
            for (int i = 0; i < 16; i++) {
                c1 = buffer[i] & 0xf0;
                c1 = c1 >> 4;
                c2 = buffer[i] & 0x0f;
                tmp[i * 2] = hex_to_ascii(c1);
                tmp[i * 2 + 1] = hex_to_ascii(c2);
                fputc(tmp[i * 2], my_add_bigint_TV_file);
                fputc(tmp[i * 2 + 1], my_add_bigint_TV_file);
            }
        }

        fseek(BigInt_a_file, 4, SEEK_CUR);
        fseek(BigInt_b_file, 4, SEEK_CUR);
        fputc('\n', my_add_bigint_TV_file);
        fputc('\n', my_add_bigint_TV_file);
    }
    

    fclose(BigInt_a_file);
    fclose(BigInt_b_file);
    fclose(my_add_bigint_TV_file);

    return 0;
}

uint32_t check_sub_bigint(const char *BigInt_a_file_, const char *BigInt_b_file_, const char *my_sub_bigint_TV_file_){
    FILE *BigInt_a_file;
    FILE *BigInt_b_file;
    FILE *my_sub_bigint_TV_file;

    BigInt_a_file = fopen(BigInt_a_file_, "rb");
    BigInt_b_file = fopen(BigInt_b_file_, "rb");
    my_sub_bigint_TV_file = fopen(my_sub_bigint_TV_file_, "wb");
    if (BigInt_a_file == NULL || BigInt_b_file == NULL || my_sub_bigint_TV_file == NULL) { //파일을 못읽어 왔을때
		printf("ERROR");
		return 0;
	}

    fseek(BigInt_a_file, 0, SEEK_END);
    unsigned long long int file_byte = ftell(BigInt_a_file);
    fseek(BigInt_a_file, 0, SEEK_SET);

    while (ftell(BigInt_a_file) != file_byte){
        uint32_t BigInt_a[SIZE] = {0x00,};
        uint32_t BigInt_b[SIZE] = {0x00,};
        uint32_t result[SIZE] = {0x00,};
        uint8_t BigInt_a_byte[16] = {0x00,};
        uint8_t BigInt_b_byte[16] = {0x00,};
        uint8_t buffer[16] = {0x00,};
        uint8_t tmp[32] = {0x00,};
        uint8_t c1, c2, sum;
        for (int size_i = 0; size_i < 2; size_i++){
            //a값 받아오기
            for (int i = 0; i < 16; i++) {
                tmp[i * 2] = fgetc(BigInt_a_file);
                tmp[i * 2 + 1] = fgetc(BigInt_a_file);
                c1 = ascii_to_hex(tmp[i * 2]);
                c2 = ascii_to_hex(tmp[i * 2 + 1]);
                sum = c1 << 4 | c2;
                BigInt_a_byte[i] = sum;
            }
            //b값 받아오기
            for (int i = 0; i < 16; i++) {
                tmp[i * 2] = fgetc(BigInt_b_file);
                tmp[i * 2 + 1] = fgetc(BigInt_b_file);
                c1 = ascii_to_hex(tmp[i * 2]);
                c2 = ascii_to_hex(tmp[i * 2 + 1]);
                sum = c1 << 4 | c2;
                BigInt_b_byte[i] = sum;
            }

            byte2state(BigInt_a_byte, BigInt_a + size_i * 4);
            byte2state(BigInt_b_byte, BigInt_b + size_i * 4);
        }

        sub_bigint(BigInt_a, BigInt_b, result);

        for (int size_i = 0; size_i < 2; size_i++){
            state2byte(result + size_i * 4, buffer);

            //16진수 형태로 넣기
            for (int i = 0; i < 16; i++) {
                c1 = buffer[i] & 0xf0;
                c1 = c1 >> 4;
                c2 = buffer[i] & 0x0f;
                tmp[i * 2] = hex_to_ascii(c1);
                tmp[i * 2 + 1] = hex_to_ascii(c2);
                fputc(tmp[i * 2], my_sub_bigint_TV_file);
                fputc(tmp[i * 2 + 1], my_sub_bigint_TV_file);
            }
        }

        fseek(BigInt_a_file, 4, SEEK_CUR);
        fseek(BigInt_b_file, 4, SEEK_CUR);
        fputc('\n', my_sub_bigint_TV_file);
        fputc('\n', my_sub_bigint_TV_file);
    }
    

    fclose(BigInt_a_file);
    fclose(BigInt_b_file);
    fclose(my_sub_bigint_TV_file);

    return 0;
}

uint32_t check_mul_bigint_os(const char *BigInt_a_file_, const char *BigInt_b_file_, const char *my_mul_bigint_TV_file_){
    FILE *BigInt_a_file;
    FILE *BigInt_b_file;
    FILE *my_mul_bigint_TV_file;

    BigInt_a_file = fopen(BigInt_a_file_, "rb");
    BigInt_b_file = fopen(BigInt_b_file_, "rb");
    my_mul_bigint_TV_file = fopen(my_mul_bigint_TV_file_, "wb");
    if (BigInt_a_file == NULL || BigInt_b_file == NULL || my_mul_bigint_TV_file == NULL) { //파일을 못읽어 왔을때
		printf("ERROR");
		return 0;
	}

    fseek(BigInt_a_file, 0, SEEK_END);
    unsigned long long int file_byte = ftell(BigInt_a_file);
    fseek(BigInt_a_file, 0, SEEK_SET);

    while (ftell(BigInt_a_file) != file_byte){
    //for(int test_i = 0; test_i < 3; test_i++){
        uint32_t BigInt_a[SIZE] = {0x00,};
        uint32_t BigInt_b[SIZE] = {0x00,};
        uint32_t result[SIZE*2] = {0x00,};
        uint8_t BigInt_a_byte[16] = {0x00,};
        uint8_t BigInt_b_byte[16] = {0x00,};
        uint8_t buffer[16] = {0x00,};
        uint8_t tmp[32] = {0x00,};
        uint8_t c1, c2, sum;
        for (int size_i = 0; size_i < 2; size_i++){
            //a값 받아오기
            for (int i = 0; i < 16; i++) {
                tmp[i * 2] = fgetc(BigInt_a_file);
                tmp[i * 2 + 1] = fgetc(BigInt_a_file);
                c1 = ascii_to_hex(tmp[i * 2]);
                c2 = ascii_to_hex(tmp[i * 2 + 1]);
                sum = c1 << 4 | c2;
                BigInt_a_byte[i] = sum;
            }
            //b값 받아오기
            for (int i = 0; i < 16; i++) {
                tmp[i * 2] = fgetc(BigInt_b_file);
                tmp[i * 2 + 1] = fgetc(BigInt_b_file);
                c1 = ascii_to_hex(tmp[i * 2]);
                c2 = ascii_to_hex(tmp[i * 2 + 1]);
                sum = c1 << 4 | c2;
                BigInt_b_byte[i] = sum;
            }

            byte2state(BigInt_a_byte, BigInt_a + size_i * 4);
            byte2state(BigInt_b_byte, BigInt_b + size_i * 4);
        }

        mul_bigint_os(BigInt_a, BigInt_b, result);

        for (int size_i = 0; size_i < 4; size_i++){
            state2byte(result + size_i * 4, buffer);

            //16진수 형태로 넣기
            for (int i = 0; i < 16; i++) {
                c1 = buffer[i] & 0xf0;
                c1 = c1 >> 4;
                c2 = buffer[i] & 0x0f;
                tmp[i * 2] = hex_to_ascii(c1);
                tmp[i * 2 + 1] = hex_to_ascii(c2);
                fputc(tmp[i * 2], my_mul_bigint_TV_file);
                fputc(tmp[i * 2 + 1], my_mul_bigint_TV_file);
            }
        }

        fseek(BigInt_a_file, 4, SEEK_CUR);
        fseek(BigInt_b_file, 4, SEEK_CUR);
        fputc('\n', my_mul_bigint_TV_file);
        fputc('\n', my_mul_bigint_TV_file);
    }
    

    fclose(BigInt_a_file);
    fclose(BigInt_b_file);
    fclose(my_mul_bigint_TV_file);

    return 0;
}

uint32_t check_mul_bigint_ps(const char *BigInt_a_file_, const char *BigInt_b_file_, const char *my_mul_bigint_TV_file_){
    FILE *BigInt_a_file;
    FILE *BigInt_b_file;
    FILE *my_mul_bigint_TV_file;

    BigInt_a_file = fopen(BigInt_a_file_, "rb");
    BigInt_b_file = fopen(BigInt_b_file_, "rb");
    my_mul_bigint_TV_file = fopen(my_mul_bigint_TV_file_, "wb");
    if (BigInt_a_file == NULL || BigInt_b_file == NULL || my_mul_bigint_TV_file == NULL) { //파일을 못읽어 왔을때
		printf("ERROR");
		return 0;
	}

    fseek(BigInt_a_file, 0, SEEK_END);
    unsigned long long int file_byte = ftell(BigInt_a_file);
    fseek(BigInt_a_file, 0, SEEK_SET);

    while (ftell(BigInt_a_file) != file_byte){
    // for(int test_i = 0; test_i < 3; test_i++){
        uint32_t BigInt_a[SIZE] = {0x00,};
        uint32_t BigInt_b[SIZE] = {0x00,};
        uint32_t result[SIZE*2] = {0x00,};
        uint8_t BigInt_a_byte[16] = {0x00,};
        uint8_t BigInt_b_byte[16] = {0x00,};
        uint8_t buffer[16] = {0x00,};
        uint8_t tmp[32] = {0x00,};
        uint8_t c1, c2, sum;
        for (int size_i = 0; size_i < 2; size_i++){
            //a값 받아오기
            for (int i = 0; i < 16; i++) {
                tmp[i * 2] = fgetc(BigInt_a_file);
                tmp[i * 2 + 1] = fgetc(BigInt_a_file);
                c1 = ascii_to_hex(tmp[i * 2]);
                c2 = ascii_to_hex(tmp[i * 2 + 1]);
                sum = c1 << 4 | c2;
                BigInt_a_byte[i] = sum;
            }
            //b값 받아오기
            for (int i = 0; i < 16; i++) {
                tmp[i * 2] = fgetc(BigInt_b_file);
                tmp[i * 2 + 1] = fgetc(BigInt_b_file);
                c1 = ascii_to_hex(tmp[i * 2]);
                c2 = ascii_to_hex(tmp[i * 2 + 1]);
                sum = c1 << 4 | c2;
                BigInt_b_byte[i] = sum;
            }

            byte2state(BigInt_a_byte, BigInt_a + size_i * 4);
            byte2state(BigInt_b_byte, BigInt_b + size_i * 4);
        }

        mul_bigint_ps(BigInt_a, BigInt_b, result);

        for (int size_i = 0; size_i < 4; size_i++){
            state2byte(result + size_i * 4, buffer);

            //16진수 형태로 넣기
            for (int i = 0; i < 16; i++) {
                c1 = buffer[i] & 0xf0;
                c1 = c1 >> 4;
                c2 = buffer[i] & 0x0f;
                tmp[i * 2] = hex_to_ascii(c1);
                tmp[i * 2 + 1] = hex_to_ascii(c2);
                fputc(tmp[i * 2], my_mul_bigint_TV_file);
                fputc(tmp[i * 2 + 1], my_mul_bigint_TV_file);
            }
        }

        fseek(BigInt_a_file, 4, SEEK_CUR);
        fseek(BigInt_b_file, 4, SEEK_CUR);
        fputc('\n', my_mul_bigint_TV_file);
        fputc('\n', my_mul_bigint_TV_file);
    }
    

    fclose(BigInt_a_file);
    fclose(BigInt_b_file);
    fclose(my_mul_bigint_TV_file);

    return 0;
}

uint32_t check_sqr_bigint(const char *BigInt_a_file_, const char *my_sqr_bigint_TV_file_){
    FILE *BigInt_a_file;
    FILE *my_sqr_bigint_TV_file;

    BigInt_a_file = fopen(BigInt_a_file_, "rb");
    my_sqr_bigint_TV_file = fopen(my_sqr_bigint_TV_file_, "wb");
    if (BigInt_a_file == NULL || my_sqr_bigint_TV_file == NULL) { //파일을 못읽어 왔을때
		printf("ERROR");
		return 0;
	}

    fseek(BigInt_a_file, 0, SEEK_END);
    unsigned long long int file_byte = ftell(BigInt_a_file);
    fseek(BigInt_a_file, 0, SEEK_SET);

    while (ftell(BigInt_a_file) != file_byte){
    // for(int test_i = 0; test_i < 3; test_i++){
        uint32_t BigInt_a[SIZE] = {0x00,};
        uint32_t result[SIZE*2] = {0x00,};
        uint8_t BigInt_a_byte[16] = {0x00,};
        uint8_t buffer[16] = {0x00,};
        uint8_t tmp[32] = {0x00,};
        uint8_t c1, c2, sum;
        for (int size_i = 0; size_i < 2; size_i++){
            //a값 받아오기
            for (int i = 0; i < 16; i++) {
                tmp[i * 2] = fgetc(BigInt_a_file);
                tmp[i * 2 + 1] = fgetc(BigInt_a_file);
                c1 = ascii_to_hex(tmp[i * 2]);
                c2 = ascii_to_hex(tmp[i * 2 + 1]);
                sum = c1 << 4 | c2;
                BigInt_a_byte[i] = sum;
            }

            byte2state(BigInt_a_byte, BigInt_a + size_i * 4);
        }

        sqr_bigint(BigInt_a, result);

        for (int size_i = 0; size_i < 4; size_i++){
            state2byte(result + size_i * 4, buffer);

            //16진수 형태로 넣기
            for (int i = 0; i < 16; i++) {
                c1 = buffer[i] & 0xf0;
                c1 = c1 >> 4;
                c2 = buffer[i] & 0x0f;
                tmp[i * 2] = hex_to_ascii(c1);
                tmp[i * 2 + 1] = hex_to_ascii(c2);
                fputc(tmp[i * 2], my_sqr_bigint_TV_file);
                fputc(tmp[i * 2 + 1], my_sqr_bigint_TV_file);
            }
        }

        fseek(BigInt_a_file, 4, SEEK_CUR);
        fputc('\n', my_sqr_bigint_TV_file);
        fputc('\n', my_sqr_bigint_TV_file);
    }
    

    fclose(BigInt_a_file);
    fclose(my_sqr_bigint_TV_file);

    return 0;
}

uint32_t check_redn_bigint_fast(const char *BigInt_a_file_, const char *my_sqr_bigint_TV_file_){
    FILE *BigInt_a_file;
    FILE *my_sqr_bigint_TV_file;

    BigInt_a_file = fopen(BigInt_a_file_, "rb");
    my_sqr_bigint_TV_file = fopen(my_sqr_bigint_TV_file_, "wb");
    if (BigInt_a_file == NULL || my_sqr_bigint_TV_file == NULL) { //파일을 못읽어 왔을때
		printf("ERROR");
		return 0;
	}

    fseek(BigInt_a_file, 0, SEEK_END);
    unsigned long long int file_byte = ftell(BigInt_a_file);
    fseek(BigInt_a_file, 0, SEEK_SET);

    while (ftell(BigInt_a_file) != file_byte){
    // for(int test_i = 0; test_i < 3; test_i++){
        uint32_t BigInt_a[SIZE*2] = {0x00,};
        uint32_t result[SIZE] = {0x00,};
        uint8_t BigInt_a_byte[16] = {0x00,};
        uint8_t buffer[16] = {0x00,};
        uint8_t tmp[32] = {0x00,};
        uint8_t c1, c2, sum;
        for (int size_i = 0; size_i < 4; size_i++){
            //a값 받아오기
            for (int i = 0; i < 16; i++) {
                tmp[i * 2] = fgetc(BigInt_a_file);
                tmp[i * 2 + 1] = fgetc(BigInt_a_file);
                c1 = ascii_to_hex(tmp[i * 2]);
                c2 = ascii_to_hex(tmp[i * 2 + 1]);
                sum = c1 << 4 | c2;
                BigInt_a_byte[i] = sum;
            }

            byte2state(BigInt_a_byte, BigInt_a + size_i * 4);
        }

        redn_bigint_fast(BigInt_a, result);

        for (int size_i = 0; size_i < 2; size_i++){
            state2byte(result + size_i * 4, buffer);

            //16진수 형태로 넣기
            for (int i = 0; i < 16; i++) {
                c1 = buffer[i] & 0xf0;
                c1 = c1 >> 4;
                c2 = buffer[i] & 0x0f;
                tmp[i * 2] = hex_to_ascii(c1);
                tmp[i * 2 + 1] = hex_to_ascii(c2);
                fputc(tmp[i * 2], my_sqr_bigint_TV_file);
                fputc(tmp[i * 2 + 1], my_sqr_bigint_TV_file);
            }
        }

        fseek(BigInt_a_file, 2, SEEK_CUR);
        fputc('\n', my_sqr_bigint_TV_file);
        fputc('\n', my_sqr_bigint_TV_file);
    }
    

    fclose(BigInt_a_file);
    fclose(my_sqr_bigint_TV_file);

    return 0;
}

uint32_t check_redn_bigint_mont(const char *BigInt_a_file_, const char *my_sqr_bigint_TV_file_){
    FILE *BigInt_a_file;
    FILE *my_sqr_bigint_TV_file;

    BigInt_a_file = fopen(BigInt_a_file_, "rb");
    my_sqr_bigint_TV_file = fopen(my_sqr_bigint_TV_file_, "wb");
    if (BigInt_a_file == NULL || my_sqr_bigint_TV_file == NULL) { //파일을 못읽어 왔을때
		printf("ERROR");
		return 0;
	}

    fseek(BigInt_a_file, 0, SEEK_END);
    unsigned long long int file_byte = ftell(BigInt_a_file);
    fseek(BigInt_a_file, 0, SEEK_SET);

    while (ftell(BigInt_a_file) != file_byte){
    // for(int test_i = 0; test_i < 3; test_i++){
        uint32_t BigInt_a[SIZE*2] = {0x00,};
        uint32_t result[SIZE] = {0x00,};
        uint8_t BigInt_a_byte[16] = {0x00,};
        uint8_t buffer[16] = {0x00,};
        uint8_t tmp[32] = {0x00,};
        uint8_t c1, c2, sum;
        for (int size_i = 0; size_i < 4; size_i++){
            //a값 받아오기
            for (int i = 0; i < 16; i++) {
                tmp[i * 2] = fgetc(BigInt_a_file);
                tmp[i * 2 + 1] = fgetc(BigInt_a_file);
                c1 = ascii_to_hex(tmp[i * 2]);
                c2 = ascii_to_hex(tmp[i * 2 + 1]);
                sum = c1 << 4 | c2;
                BigInt_a_byte[i] = sum;
            }

            byte2state(BigInt_a_byte, BigInt_a + size_i * 4);
        }

        redn_bigint_mont(BigInt_a, result);

        for (int size_i = 0; size_i < 2; size_i++){
            state2byte(result + size_i * 4, buffer);

            //16진수 형태로 넣기
            for (int i = 0; i < 16; i++) {
                c1 = buffer[i] & 0xf0;
                c1 = c1 >> 4;
                c2 = buffer[i] & 0x0f;
                tmp[i * 2] = hex_to_ascii(c1);
                tmp[i * 2 + 1] = hex_to_ascii(c2);
                fputc(tmp[i * 2], my_sqr_bigint_TV_file);
                fputc(tmp[i * 2 + 1], my_sqr_bigint_TV_file);
            }
        }

        fseek(BigInt_a_file, 2, SEEK_CUR);
        fputc('\n', my_sqr_bigint_TV_file);
        fputc('\n', my_sqr_bigint_TV_file);
    }
    

    fclose(BigInt_a_file);
    fclose(my_sqr_bigint_TV_file);

    return 0;
}

uint32_t check_inv_bigint_EEA(const char *BigInt_a_file_, const char *my_inv_bigint_TV_file_){
    FILE *BigInt_a_file;
    FILE *my_inv_bigint_TV_file;

    BigInt_a_file = fopen(BigInt_a_file_, "rb");
    my_inv_bigint_TV_file = fopen(my_inv_bigint_TV_file_, "wb");
    if (BigInt_a_file == NULL || my_inv_bigint_TV_file == NULL) { //파일을 못읽어 왔을때
		printf("ERROR");
		return 0;
	}

    fseek(BigInt_a_file, 0, SEEK_END);
    unsigned long long int file_byte = ftell(BigInt_a_file);
    fseek(BigInt_a_file, 0, SEEK_SET);

    while (ftell(BigInt_a_file) != file_byte){
    // for(int test_i = 0; test_i < 3; test_i++){
        uint32_t BigInt_a[SIZE] = {0x00,};
        uint32_t result[SIZE] = {0x00,};
        uint8_t BigInt_a_byte[16] = {0x00,};
        uint8_t buffer[16] = {0x00,};
        uint8_t tmp[32] = {0x00,};
        uint8_t c1, c2, sum;
        for (int size_i = 0; size_i < 2; size_i++){
            //a값 받아오기
            for (int i = 0; i < 16; i++) {
                tmp[i * 2] = fgetc(BigInt_a_file);
                tmp[i * 2 + 1] = fgetc(BigInt_a_file);
                c1 = ascii_to_hex(tmp[i * 2]);
                c2 = ascii_to_hex(tmp[i * 2 + 1]);
                sum = c1 << 4 | c2;
                BigInt_a_byte[i] = sum;
            }
            byte2state(BigInt_a_byte, BigInt_a + size_i * 4);
        }

        inv_bigint_EEA(BigInt_a, result);

        for (int size_i = 0; size_i < 2; size_i++){
            state2byte(result + size_i * 4, buffer);

            //16진수 형태로 넣기
            for (int i = 0; i < 16; i++) {
                c1 = buffer[i] & 0xf0;
                c1 = c1 >> 4;
                c2 = buffer[i] & 0x0f;
                tmp[i * 2] = hex_to_ascii(c1);
                tmp[i * 2 + 1] = hex_to_ascii(c2);
                fputc(tmp[i * 2], my_inv_bigint_TV_file);
                fputc(tmp[i * 2 + 1], my_inv_bigint_TV_file);
            }
        }

        fseek(BigInt_a_file, 4, SEEK_CUR);
        fputc('\n', my_inv_bigint_TV_file);
        fputc('\n', my_inv_bigint_TV_file);
    }
    
    fclose(BigInt_a_file);
    fclose(my_inv_bigint_TV_file);

    return 0;
}

uint32_t check_inv_bigint_FLT(const char *BigInt_a_file_, const char *my_inv_bigint_TV_file_){
    FILE *BigInt_a_file;
    FILE *my_inv_bigint_TV_file;

    BigInt_a_file = fopen(BigInt_a_file_, "rb");
    my_inv_bigint_TV_file = fopen(my_inv_bigint_TV_file_, "wb");
    if (BigInt_a_file == NULL || my_inv_bigint_TV_file == NULL) { //파일을 못읽어 왔을때
		printf("ERROR");
		return 0;
	}

    fseek(BigInt_a_file, 0, SEEK_END);
    unsigned long long int file_byte = ftell(BigInt_a_file);
    fseek(BigInt_a_file, 0, SEEK_SET);

    while (ftell(BigInt_a_file) != file_byte){
    // for(int test_i = 0; test_i < 3; test_i++){
        uint32_t BigInt_a[SIZE] = {0x00,};
        uint32_t result[SIZE] = {0x00,};
        uint8_t BigInt_a_byte[16] = {0x00,};
        uint8_t buffer[16] = {0x00,};
        uint8_t tmp[32] = {0x00,};
        uint8_t c1, c2, sum;
        for (int size_i = 0; size_i < 2; size_i++){
            //a값 받아오기
            for (int i = 0; i < 16; i++) {
                tmp[i * 2] = fgetc(BigInt_a_file);
                tmp[i * 2 + 1] = fgetc(BigInt_a_file);
                c1 = ascii_to_hex(tmp[i * 2]);
                c2 = ascii_to_hex(tmp[i * 2 + 1]);
                sum = c1 << 4 | c2;
                BigInt_a_byte[i] = sum;
            }
            byte2state(BigInt_a_byte, BigInt_a + size_i * 4);
        }

        inv_bigint_FLT(BigInt_a, result);

        for (int size_i = 0; size_i < 2; size_i++){
            state2byte(result + size_i * 4, buffer);

            //16진수 형태로 넣기
            for (int i = 0; i < 16; i++) {
                c1 = buffer[i] & 0xf0;
                c1 = c1 >> 4;
                c2 = buffer[i] & 0x0f;
                tmp[i * 2] = hex_to_ascii(c1);
                tmp[i * 2 + 1] = hex_to_ascii(c2);
                fputc(tmp[i * 2], my_inv_bigint_TV_file);
                fputc(tmp[i * 2 + 1], my_inv_bigint_TV_file);
            }
        }

        fseek(BigInt_a_file, 4, SEEK_CUR);
        fputc('\n', my_inv_bigint_TV_file);
        fputc('\n', my_inv_bigint_TV_file);
    }
    
    fclose(BigInt_a_file);
    fclose(my_inv_bigint_TV_file);

    return 0;
}
