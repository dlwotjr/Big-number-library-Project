#include "bignum.h"

typedef struct _ECP_affine {
    uint32_t x[SIZE] = {0x00,};
    uint32_t y[SIZE] = {0x00,};
    uint32_t point_at_infinity = 0;
}ECP_affine;

typedef struct _ECP_jaco {
    uint32_t x[SIZE] = {0x00,};
    uint32_t y[SIZE] = {0x00,};
    uint32_t z[SIZE] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000001};
}ECP_jaco;

uint32_t coef_a[SIZE] = {0xffffffff, 0x00000001, 0x00000000, 0x00000000, 0x00000000, 0xffffffff, 0xffffffff, 0xfffffffc}; // -3
uint32_t one[SIZE] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000001};
uint32_t zero[SIZE] = {0x00,};

void jacobian_to_affine(ECP_jaco a, ECP_affine *b){
    if(compare_bigint(a.z, zero) == 0){
        printf("This point is point at infinity");
        b->point_at_infinity = 1;
        return;
    }
    uint32_t tmp1[SIZE] = {0x00,};
    uint32_t tmp2[SIZE] = {0x00,};

    inv_bigint_EEA(a.z, tmp1); // z^-1
    sqr_and_redn_bigint(tmp1, tmp2); // z^-2
    mul_and_redn_bigint(tmp1, tmp2, tmp1); // z^-3
    mul_and_redn_bigint(a.x, tmp2, b->x); // affine x
    mul_and_redn_bigint(a.y, tmp1, b->y); // affine y
}

void ECADD_affine(ECP_affine P, ECP_affine Q, ECP_affine *R){
    uint32_t ramda[SIZE] = {0x00,};
    uint32_t tmp1[SIZE] = {0x00,};
    uint32_t tmp2[SIZE] = {0x00,};

    sub_bigint(Q.x, P.x, tmp1);
    inv_bigint_EEA(tmp1, tmp2);
    sub_bigint(Q.y, P.y, tmp1);
    mul_and_redn_bigint(tmp1, tmp2, ramda); // ramda

    sqr_and_redn_bigint(ramda, tmp1);
    sub_bigint(tmp1, P.x, tmp2);
    sub_bigint(tmp2, Q.x, R->x); // x3

    sub_bigint(P.x, R->x, tmp1);
    mul_and_redn_bigint(tmp1, ramda, tmp1);
    sub_bigint(tmp1, P.y, R->y); // y3
}

void ECDBL_affine(ECP_affine P, ECP_affine *R){
    uint32_t ramda[SIZE] = {0x00,};
    uint32_t tmp1[SIZE] = {0x00,};
    uint32_t tmp2[SIZE] = {0x00,};
    uint32_t tmp3[SIZE] = {0x00,};
    uint32_t tmp_constant[SIZE] = {0x00,};

    mul_bigint_by_bitshift(P.y, tmp1, 1); // 곱셈을 bitshift로 대체 (x2)
    inv_bigint_EEA(tmp1, tmp2);
    sqr_and_redn_bigint(P.x, tmp1);
    // tmp_constant[7] = 0x03;
    // mul_and_redn_bigint(tmp_constant, tmp1, tmp1);
    mul_bigint_by_bitshift(tmp1, tmp3, 1); // 곱셈을 bitshift와 add로 대체 (x3)
    add_bigint(tmp3, tmp1, tmp1);          // 곱셈을 bitshift와 add로 대체 (x3)
    add_bigint(tmp1, coef_a, tmp1);
    mul_and_redn_bigint(tmp1, tmp2, ramda); // ramda

    sqr_and_redn_bigint(ramda, tmp2);
    mul_bigint_by_bitshift(P.x, tmp1, 1); // 곱셈을 bitshift로 대체 (x2)
    sub_bigint(tmp2, tmp1, R->x); // x3

    sub_bigint(P.x, R->x, tmp1);
    mul_and_redn_bigint(ramda, tmp1, tmp1);
    sub_bigint(tmp1, P.y, R->y); // y3
}

void ECDBL_jaco(ECP_jaco P, ECP_jaco *R){
    if(compare_bigint(P.z, zero) == 0){ // 무한원점일때 반환해주는 코드
        memcpy(R->z, zero, sizeof(uint32_t) * SIZE*2);
        return;
    } 
    uint32_t tmp1[SIZE] = {0x00,};
    uint32_t tmp2[SIZE] = {0x00,};
    uint32_t tmp3[SIZE] = {0x00,};
    uint32_t tmp4[SIZE] = {0x00,};

    sqr_and_redn_bigint(P.z, tmp1);
    sub_bigint(P.x, tmp1, tmp2);
    add_bigint(P.x, tmp1, tmp1);
    mul_and_redn_bigint(tmp2, tmp1, tmp2);
    mul_bigint_by_bitshift(tmp2, tmp4, 1); // 곱셈을 bitshift와 add로 대체 (x3)
    add_bigint(tmp2, tmp4, tmp2);          // 곱셈을 bitshift와 add로 대체 (x3)
    mul_bigint_by_bitshift(P.y, R->y, 1);  // 곱셈을 bitshift로 대체 (x2)
    mul_and_redn_bigint(R->y, P.z, R->z);
    sqr_and_redn_bigint(R->y, R->y);
    mul_and_redn_bigint(R->y, P.x, tmp3);
    sqr_and_redn_bigint(R->y, R->y);
    div_bigint_by_bitshift(R->y, R->y, 1); // 나눗셈을 bitshift로 대체 (/2)
    sqr_and_redn_bigint(tmp2, R->x);
    mul_bigint_by_bitshift(tmp3, tmp1, 1); // 곱셈을 bitshift로 대체 (x2)
    sub_bigint(R->x, tmp1, R->x);
    sub_bigint(tmp3, R->x, tmp1);
    mul_and_redn_bigint(tmp1, tmp2, tmp1);
    sub_bigint(tmp1, R->y, R->y);
}

void ECADD_jaco(ECP_jaco P, ECP_affine Q, ECP_jaco *R){
    if(Q.point_at_infinity == 1){
        memcpy(R, &P, sizeof(uint32_t) * SIZE*2);
        return;
    }
    if(compare_bigint(P.z, zero) == 0){
        memcpy(R, &Q, sizeof(uint32_t) * SIZE*2);
        memcpy(R->z, one, sizeof(uint32_t) * SIZE);
        return;
    }
    uint32_t tmp1[SIZE] = {0x00,};
    uint32_t tmp2[SIZE] = {0x00,};
    uint32_t tmp3[SIZE] = {0x00,};
    uint32_t tmp4[SIZE] = {0x00,};

    sqr_and_redn_bigint(P.z, tmp1);
    mul_and_redn_bigint(tmp1, P.z, tmp2);
    mul_and_redn_bigint(tmp1, Q.x, tmp1);
    mul_and_redn_bigint(tmp2, Q.y, tmp2);
    sub_bigint(tmp1, P.x, tmp1);
    sub_bigint(tmp2, P.y, tmp2);
    if(compare_bigint(tmp1, zero) == 0){
        if(compare_bigint(tmp2, zero) == 0){
            ECP_jaco Q_jaco;
            memcpy(&Q_jaco, &Q, sizeof(uint32_t) * SIZE*2);
            ECP_jaco *tmp_R = R;
            ECDBL_jaco(Q_jaco, tmp_R);
            return;
        }
        else{ // 무한원점 리턴하기
            memcpy(R->z, zero, sizeof(uint32_t) * SIZE);
            return;
        }
    }
    mul_and_redn_bigint(P.z, tmp1, R->z);
    sqr_and_redn_bigint(tmp1, tmp3);
    mul_and_redn_bigint(tmp3, tmp1, tmp4);
    mul_and_redn_bigint(tmp3, P.x, tmp3);
    mul_bigint_by_bitshift(tmp3, tmp1, 1); // 곱셈을 bitshift로 대체 (x2)
    sqr_and_redn_bigint(tmp2, R->x);
    sub_bigint(R->x, tmp1, R->x);
    sub_bigint(R->x, tmp4, R->x);
    sub_bigint(tmp3, R->x, tmp3);
    mul_and_redn_bigint(tmp3, tmp2, tmp3);
    mul_and_redn_bigint(tmp4, P.y, tmp4);
    sub_bigint(tmp3, tmp4, R->y);
}

void ECSM_LtR(ECP_affine P, const uint32_t k[SIZE], ECP_jaco *Q){
    ECP_jaco tmp_Q; // x=1, y=1, z=0 으로 만들어주기
    memcpy(tmp_Q.x, one, sizeof(uint32_t) * SIZE);
    memcpy(tmp_Q.y, one, sizeof(uint32_t) * SIZE);
    memcpy(tmp_Q.z, zero, sizeof(uint32_t) * SIZE);
    uint32_t tmp = 0x80000000;

    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < 32; j++){
            ECDBL_jaco(tmp_Q, &tmp_Q);
            if(k[i] & tmp)
                ECADD_jaco(tmp_Q, P, &tmp_Q);
            tmp >>= 1;
        }
        tmp = 0x80000000;
    }

    memcpy(Q, &tmp_Q, sizeof(ECP_jaco));
}

void ECSM_RtL(ECP_affine P, const uint32_t k[SIZE], ECP_jaco *Q){
    ECP_jaco tmp_Q; // x=1, y=1, z=0 으로 만들어주기
    ECP_jaco tmp_P;
    memcpy(tmp_Q.x, one, sizeof(uint32_t) * SIZE);
    memcpy(tmp_Q.y, one, sizeof(uint32_t) * SIZE);
    memcpy(tmp_Q.z, zero, sizeof(uint32_t) * SIZE);
    memcpy(&tmp_P, &P, sizeof(uint32_t) * SIZE*2);
    uint32_t tmp = 0x01;

    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < 32; j++){
            if ((k[SIZE-1 - i] & tmp) != 0x00){
                jacobian_to_affine(tmp_P, &P); // 이걸해주면 연산 느리지 않나?
                ECADD_jaco(tmp_Q, P, &tmp_Q);
            }
            ECDBL_jaco(tmp_P, &tmp_P);
            tmp <<= 1;
        }
        tmp = 0x01;
    }

    memcpy(Q, &tmp_Q, sizeof(ECP_jaco));
}

uint32_t check__SM(const char *Scalar_file_, const char *my_SM_TV_file_){
    FILE *Scalar_file;
    FILE *my_SM_TV_file;

    Scalar_file = fopen(Scalar_file_, "rb");
    my_SM_TV_file = fopen(my_SM_TV_file_, "wb");
    if (Scalar_file == NULL || my_SM_TV_file == NULL) { //파일을 못읽어 왔을때
		printf("ERROR");
		return 0;
	}

    fseek(Scalar_file, 0, SEEK_END);
    unsigned long long int file_byte = ftell(Scalar_file);
    fseek(Scalar_file, 0, SEEK_SET);

    while (ftell(Scalar_file) != file_byte){
        uint32_t BigInt_a[SIZE] = {0x00,};
        uint32_t result[SIZE] = {0x00,};
        uint8_t BigInt_a_byte[16] = {0x00,};
        uint8_t buffer[16] = {0x00,};
        uint8_t tmp[32] = {0x00,};
        uint8_t c1, c2, sum;
        for (int size_i = 0; size_i < 2; size_i++){
            //a값 받아오기
            for (int i = 0; i < 16; i++) {
                tmp[i * 2] = fgetc(Scalar_file);
                tmp[i * 2 + 1] = fgetc(Scalar_file);
                c1 = ascii_to_hex(tmp[i * 2]);
                c2 = ascii_to_hex(tmp[i * 2 + 1]);
                sum = c1 << 4 | c2;
                BigInt_a_byte[i] = sum;
            }

            byte2state(BigInt_a_byte, BigInt_a + size_i * 4);
        }

        ECP_affine P = {{0x6b17d1f2, 0xe12c4247, 0xf8bce6e5, 0x63a440f2, 0x77037d81, 0x2deb33a0, 0xf4a13945, 0xd898c296}, {0x4fe342e2, 0xfe1a7f9b, 0x8ee7eb4a, 0x7c0f9e16, 0x2bce3357, 0x6b315ece, 0xcbb64068, 0x37bf51f5}};
        ECP_jaco R = {0x00,};
        ECP_affine R_result = {0x00,};
        ECSM_LtR(P, BigInt_a, &R);
        // ECSM_RtL(P, k, &R);
        jacobian_to_affine(R, &R_result);

        for (int size_i = 0; size_i < 2; size_i++){
            state2byte(R.x + size_i * 4, buffer);

            //16진수 형태로 넣기
            for (int i = 0; i < 16; i++) {
                c1 = buffer[i] & 0xf0;
                c1 = c1 >> 4;
                c2 = buffer[i] & 0x0f;
                tmp[i * 2] = hex_to_ascii(c1);
                tmp[i * 2 + 1] = hex_to_ascii(c2);
                fputc(tmp[i * 2], my_SM_TV_file);
                fputc(tmp[i * 2 + 1], my_SM_TV_file);
            }
        }
        fputc('\n', my_SM_TV_file);
        for (int size_i = 0; size_i < 2; size_i++){
            state2byte(R.y + size_i * 4, buffer);

            //16진수 형태로 넣기
            for (int i = 0; i < 16; i++) {
                c1 = buffer[i] & 0xf0;
                c1 = c1 >> 4;
                c2 = buffer[i] & 0x0f;
                tmp[i * 2] = hex_to_ascii(c1);
                tmp[i * 2 + 1] = hex_to_ascii(c2);
                fputc(tmp[i * 2], my_SM_TV_file);
                fputc(tmp[i * 2 + 1], my_SM_TV_file);
            }
        }

        fseek(Scalar_file, 4, SEEK_CUR);
        fputc('\n', my_SM_TV_file);
        fputc('\n', my_SM_TV_file);
    }

    fclose(Scalar_file);
    fclose(my_SM_TV_file);

    return 0;
}

void ECDH_alice(ECP_affine bob, const uint32_t* alice_sk, ECP_affine* dest) {
    ECP_jaco tmp = {{0x00, }, {0x00, }, {0x00, }};
    ECSM_RtL(bob, alice_sk, &tmp);
    jacobian_to_affine(tmp, dest);
}
void ECDH_bob(ECP_affine alice, const uint32_t* bob_sk, ECP_affine* dest) {
    ECP_jaco tmp = {{0x00, }, {0x00, }, {0x00, }};
    ECSM_RtL(alice, bob_sk, &tmp);
    jacobian_to_affine(tmp, dest);
}

int main() {
    //alice secret key
    uint32_t alice_sk[SIZE] = { 0x2d524407,0x3e432d4a,0x3a2b196e,0xc95eba75,0xaa6f0425,0xc7806f67,0x8e3055b9,0x899adee5 };
    ECP_affine alice = { {0xd986de55, 0xe7fdf4da, 0xcdad6f9e, 0xa8d4e11b, 0xb3b54ef4, 0xd4ff3054, 0x992cf9fa, 0x6f374f1b}, { 0xd50647e6, 0x606a1c52, 0x6503fe59, 0x3c542336, 0xe6d6accd, 0xd5f50917, 0x11e203d6, 0xf05288f7 }};

    //bob secret key
    uint32_t bob_sk[SIZE] = { 0x08c82d6a,0xd6e55cb6,0xad7b8572,0xc071486d,0x455fbf1e,0xa7b2dcf4,0x9ef9fafd,0x5fbde7a5 };
    ECP_affine bob = { { 0x8234ff7f,0xa54856a5,0xd9a14e02,0x8efb6946,0x93bd487c,0x48bf37a6,0xdf501b28,0xead2f785 }, { 0xdf6dae4b,0x90c29fa1,0x9f3ec62a,0xe4fa3eb3,0xce999f7d,0x467144f2,0x78d6c904,0xb6ef8abc } };

    ECP_affine shared_key_alice = {0x00,};
    ECP_affine shared_key_bob = {0x00,};

    ECDH_alice(bob, alice_sk, &shared_key_alice);
    ECDH_bob(alice, bob_sk, &shared_key_bob);



    printf("[Alice's secret key]\n");
    for (int i = 0; i < 8; i++)
        printf("%08x", alice_sk[i]);
    printf("\n");

    printf("[Alice's public key]\n");
    for (int i = 0; i < 8; i++)
        printf("%08x", alice.x[i]);
    printf("\n");
    for (int i = 0; i < 8; i++)
        printf("%08x", alice.y[i]);
    printf("\n\n");

    printf("[Bob's secret key]\n");
    for (int i = 0; i < 8; i++)
        printf("%08x", bob_sk[i]);
    printf("\n");

    printf("[Bob's public key]\n");
    for (int i = 0; i < 8; i++)
        printf("%08x", bob.x[i]);
    printf("\n");
    for (int i = 0; i < 8; i++)
        printf("%08x", bob.y[i]);
    printf("\n\n");

    printf("[Alice's shared key]\n");
    for (int i = 0; i < 8; i++)
        printf("%08x", shared_key_alice.x[i]);
    printf("\n");
    for (int i = 0; i < 8; i++)
        printf("%08x", shared_key_alice.y[i]);
    printf("\n\n");

    printf("[Bob's shared key]\n");
    for (int i = 0; i < 8; i++)
        printf("%08x", shared_key_bob.x[i]);
    printf("\n");
    for (int i = 0; i < 8; i++)
        printf("%08x", shared_key_bob.y[i]);
    printf("\n");
}