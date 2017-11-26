#pragma once

/**************************** DATA TYPES ****************************/
typedef unsigned char BYTE;  // 8-bit byte

typedef struct {
    BYTE data[64];
    uint32_t datalen;
    unsigned long long bitlen;
    uint32_t state[8];
} SHA256_CTX;


/*********************** FUNCTION DECLARATIONS **********************/
void sha256_init(SHA256_CTX *ctx);
void sha256_update(SHA256_CTX *ctx, const BYTE* data, uint32_t len);
void sha256_final(SHA256_CTX *ctx, BYTE* hash);

void compute_sha256(const void* data, const uint32_t length, char* sha256);
