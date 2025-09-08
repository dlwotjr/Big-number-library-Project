//
//  rand.h
//
//  Created by Bassham, Lawrence E (Fed) on 8/29/17.
//  Copyright © 2017 Bassham, Lawrence E (Fed). All rights reserved.
//

#ifndef RAND_H
#define RAND_H

#include <stdio.h>

#ifndef RNG_SUCCESS
#define RNG_SUCCESS 0
#endif

#ifndef RNG_FAILURE
#define RNG_FAILURE 1
#endif

#ifndef RNG_BAD_MAXLEN
#define RNG_BAD_MAXLEN -1
#endif

#ifndef RNG_BAD_OUTBUF
#define RNG_BAD_OUTBUF -2
#endif

#ifndef RNG_BAD_REQ_LEN
#define RNG_BAD_REQ_LEN -3
#endif

#ifdef USE_OPENSSL
typedef struct {
    unsigned char buffer[16];
    int buffer_pos;
    unsigned long length_remaining;
    unsigned char key[32];
    unsigned char ctr[16];
} AES_XOF_struct;

typedef struct {
    unsigned char Key[32];
    unsigned char V[16];
    int reseed_counter;
    int init;
} AES256_CTR_DRBG_struct;

void randombytes_init(unsigned char *entropy_input,
                      unsigned char *personalization_string,
                      int security_strength);
#endif // USE_OPENSSL

int randombytes(unsigned char *out, size_t outlen);

#endif // RAND_H