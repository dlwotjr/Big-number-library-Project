#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <x86intrin.h>

#define SIZE 8

uint32_t ascii_to_hex(const uint8_t c);
uint32_t hex_to_ascii(const uint8_t c);
void byte2state(const uint8_t b[16], uint32_t state[4]);
void state2byte(const uint32_t state[4], uint8_t b[16]);
void add_int(const uint32_t a, const uint32_t b, uint32_t *result, int *carry);
void add_bigint(uint32_t a[SIZE], uint32_t b[SIZE], uint32_t result[SIZE]);
void add_bigint_16(uint32_t a[SIZE*2], uint32_t b[SIZE*2], uint32_t result[SIZE*2]);
void sub_int(const uint32_t a, const uint32_t b, uint32_t *result, int *borrow);
void sub_bigint(uint32_t a[SIZE], uint32_t b[SIZE], uint32_t result[SIZE]);
void mul_bigint_os(const uint32_t a[SIZE], const uint32_t b[SIZE], uint32_t result[SIZE*2]);
void mul_bigint_ps(const uint32_t a[SIZE], const uint32_t b[SIZE], uint32_t result[SIZE*2]);
void sqr_bigint(const uint32_t a[SIZE], uint32_t result[SIZE*2]);
void redn_bigint_fast(const uint32_t a[SIZE*2], uint32_t result[SIZE]);
int compare_bigint(const uint32_t a[SIZE], const uint32_t b[SIZE]);
void redn_bigint_mont(const uint32_t a[SIZE*2], uint32_t result[SIZE]);
void div_bigint_by_bitshift(const uint32_t a[SIZE], uint32_t b[SIZE], const uint32_t loop_cnt);
void mul_bigint_by_bitshift(const uint32_t a[SIZE], uint32_t b[SIZE], const uint32_t loop_cnt);
void inv_bigint_EEA(const uint32_t a[SIZE], uint32_t result[SIZE]);
void sqr_bigint_loop(const uint32_t a[SIZE], uint32_t result[SIZE], const uint32_t loop_cnt);
void inv_bigint_FLT(const uint32_t a[SIZE], uint32_t result[SIZE]);
void mul_and_redn_bigint(uint32_t a[SIZE], uint32_t b[SIZE], uint32_t result[SIZE]);
void sqr_and_redn_bigint(uint32_t a[SIZE], uint32_t result[SIZE]);
void mul_bigint_LtR(uint32_t a[SIZE], const uint32_t b[SIZE], uint32_t result[SIZE]);
void mul_bigint_os_4(const uint32_t a[SIZE/2], const uint32_t b[SIZE/2], uint32_t result[SIZE*2]);
void mul_bigint_os_5(const uint32_t a[SIZE/2+1], const uint32_t b[SIZE/2+1], uint32_t result[SIZE*2]);
void add_bigint_4(const uint32_t a[SIZE/2], const uint32_t b[SIZE/2], uint32_t result[SIZE/2+1]);
int compare_bigint_16(const uint32_t a[SIZE*2], const uint32_t b[SIZE*2]);
void sub_bigint_16(uint32_t a[SIZE*2], uint32_t b[SIZE*2], uint32_t result[SIZE*2]);
void mul_bigint_karatsuba(const uint32_t a[SIZE], const uint32_t b[SIZE], uint32_t result[SIZE*2]);
uint32_t check_add_bigint(const char *BigInt_a_file_, const char *BigInt_b_file_, const char *my_add_bigint_TV_file_);
uint32_t check_sub_bigint(const char *BigInt_a_file_, const char *BigInt_b_file_, const char *my_sub_bigint_TV_file_);
uint32_t check_mul_bigint_os(const char *BigInt_a_file_, const char *BigInt_b_file_, const char *my_mul_bigint_TV_file_);
uint32_t check_mul_bigint_ps(const char *BigInt_a_file_, const char *BigInt_b_file_, const char *my_mul_bigint_TV_file_);
uint32_t check_sqr_bigint(const char *BigInt_a_file_, const char *my_sqr_bigint_TV_file_);
uint32_t check_redn_bigint_fast(const char *BigInt_a_file_, const char *my_sqr_bigint_TV_file_);
uint32_t check_redn_bigint_mont(const char *BigInt_a_file_, const char *my_sqr_bigint_TV_file_);
uint32_t check_inv_bigint_EEA(const char *BigInt_a_file_, const char *my_inv_bigint_TV_file_);
uint32_t check_inv_bigint_FLT(const char *BigInt_a_file_, const char *my_inv_bigint_TV_file_);
