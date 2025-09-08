#if defined(_WIN32)
#include <stdint.h>
#include<stdio.h>
#include <windows.h>
#include <wincrypt.h>
#include "randombytes.h"


//int main() {
//    uint8_t buffer[32]; // 예를 들어 32 바이트의 난수를 생성하려면 버퍼 크기를 조절하세요.
//    int result = randombytes_win32_randombytes(buffer, sizeof(buffer));
//    if (result == 0) {
//        for (int i = 0; i < sizeof(buffer); i++) {
//            printf("%02x", buffer[i]);
//        }
//    }
//    else {
//        // 난수 생성에 실패한 경우 처리할 코드를 여기에 추가하세요.
//    }
//
//    return 0;
//}
#endif

int randombytes(uint8_t* output, size_t n)
{
    HCRYPTPROV ctx;
    BOOL tmp;

    tmp = CryptAcquireContext(&ctx, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
    if (tmp == FALSE) {
        return -1;
    }

    tmp = CryptGenRandom(ctx, (unsigned long)n, (uint8_t*)output);
    if (tmp == FALSE) {
        return -1;
    }

    tmp = CryptReleaseContext(ctx, 0);
    if (tmp == FALSE) {
        return -1;
    }

    return 0;
}