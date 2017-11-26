#include <memory.h>
#include <cstdint>
#include <cstdio>
#include "sha256.h"


constexpr uint32_t ROTLEFT(uint32_t a, uint32_t b)          { return (a << b) | (a >> (32 - b)); }
constexpr uint32_t ROTRIGHT(uint32_t a, uint32_t b)         { return (a >> b) | (a << (32 - b)); }
constexpr uint32_t MIN(uint32_t a, uint32_t b)              { return a > b ? b : a; }

constexpr uint32_t CH(uint32_t x, uint32_t y, uint32_t z)   { return (x & y) ^ (~x & z); }
constexpr uint32_t MAJ(uint32_t x, uint32_t y, uint32_t z)  { return (x & y) ^ (x & z) ^ (y & z); }
constexpr uint32_t EP0(uint32_t x)                          { return ROTRIGHT(x, 2) ^ ROTRIGHT(x, 13) ^ ROTRIGHT(x, 22); }
constexpr uint32_t EP1(uint32_t x)                          { return ROTRIGHT(x, 6) ^ ROTRIGHT(x, 11) ^ ROTRIGHT(x, 25); }
constexpr uint32_t SIG0(uint32_t x)                         { return ROTRIGHT(x, 7) ^ ROTRIGHT(x, 18) ^ (x >> 3); }
constexpr uint32_t SIG1(uint32_t x)                         { return ROTRIGHT(x, 17) ^ ROTRIGHT(x, 19) ^ (x >> 10); }


/**************************** VARIABLES *****************************/
static const uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

/*********************** FUNCTION DEFINITIONS ***********************/
inline void sha256_transform(SHA256_CTX *ctx, const BYTE* data)
{
    uint32_t m[64];

#pragma unroll
    for (uint32_t i = 0, j = 0; i < 16; ++i, j += 4) {
        m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
    }
#pragma unroll
    for (uint32_t i = 16; i < 64; ++i) {
        m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];
    }

    uint32_t a = ctx->state[0];
    uint32_t b = ctx->state[1];
    uint32_t c = ctx->state[2];
    uint32_t d = ctx->state[3];
    uint32_t e = ctx->state[4];
    uint32_t f = ctx->state[5];
    uint32_t g = ctx->state[6];
    uint32_t h = ctx->state[7];

#pragma unroll
    for (uint32_t i = 0; i < 64; ++i) {
        const uint32_t t1 = h + EP1(e) + CH(e, f, g) + k[i] + m[i];
        const uint32_t t2 = EP0(a) + MAJ(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;
}

void sha256_init(SHA256_CTX *ctx)
{
    ctx->datalen = 0;
    ctx->bitlen = 0;
    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
}

void sha256_update(SHA256_CTX *ctx, const BYTE* data, uint32_t len)
{
    // assert(ctx->datalen >= 0 && ctx->datalen < 64);
    if (len < 64 - ctx->datalen) {
        memcpy(ctx->data + ctx->datalen, data, len);
        ctx->datalen += len;
        return;
    }

//    for (uint32_t i = 0; i < len; ++i) {
//        ctx->data[ctx->datalen] = data[i];
//        ctx->datalen++;
//        if (ctx->datalen == 64) {
//            sha256_transform(ctx, ctx->data);
//            ctx->bitlen += 512;
//            ctx->datalen = 0;
//        }
//    }


    // now: len >= 64 - ctx->datalen
    memcpy(ctx->data + ctx->datalen, data, 64 - ctx->datalen);
    sha256_transform(ctx, ctx->data);
    ctx->bitlen += 512;

    uint32_t offset = 64 - ctx->datalen;
    for (; offset + 64 <= len; offset += 64) {
        memcpy(ctx->data, data + offset, 64);
        sha256_transform(ctx, ctx->data);
        ctx->bitlen += 512;
    }

    // now: 0 <= len - offset < 64
    memcpy(ctx->data, data + offset, len - offset);
    ctx->datalen = len - offset;
}

void sha256_final(SHA256_CTX *ctx, BYTE hash[])
{
    uint32_t i = ctx->datalen;

    // Pad whatever data is left in the buffer.
    if (ctx->datalen < 56) {
        ctx->data[i++] = 0x80;
        while (i < 56)
            ctx->data[i++] = 0x00;
    }
    else {
        ctx->data[i++] = 0x80;
        while (i < 64)
            ctx->data[i++] = 0x00;
        sha256_transform(ctx, ctx->data);
        memset(ctx->data, 0, 56);
    }

    // Append to the padding the total message's length in bits and transform.
    ctx->bitlen += ctx->datalen * 8;
    ctx->data[63] = (BYTE)(ctx->bitlen);
    ctx->data[62] = (BYTE)(ctx->bitlen >> 8);
    ctx->data[61] = (BYTE)(ctx->bitlen >> 16);
    ctx->data[60] = (BYTE)(ctx->bitlen >> 24);
    ctx->data[59] = (BYTE)(ctx->bitlen >> 32);
    ctx->data[58] = (BYTE)(ctx->bitlen >> 40);
    ctx->data[57] = (BYTE)(ctx->bitlen >> 48);
    ctx->data[56] = (BYTE)(ctx->bitlen >> 56);
    sha256_transform(ctx, ctx->data);

    // Since this implementation uses little endian byte ordering and SHA uses big endian,
    // reverse all the bytes when copying the final state to the output hash.
    for (i = 0; i < 4; ++i) {
        hash[i]      = (BYTE)((ctx->state[0] >> (24 - i * 8)));
        hash[i + 4]  = (BYTE)((ctx->state[1] >> (24 - i * 8)));
        hash[i + 8]  = (BYTE)((ctx->state[2] >> (24 - i * 8)));
        hash[i + 12] = (BYTE)((ctx->state[3] >> (24 - i * 8)));
        hash[i + 16] = (BYTE)((ctx->state[4] >> (24 - i * 8)));
        hash[i + 20] = (BYTE)((ctx->state[5] >> (24 - i * 8)));
        hash[i + 24] = (BYTE)((ctx->state[6] >> (24 - i * 8)));
        hash[i + 28] = (BYTE)((ctx->state[7] >> (24 - i * 8)));
    }
}





void compute_sha256(const void* data, const uint32_t length, char* sha256)
{
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (const BYTE*)data, length);

    BYTE hash[32];
    sha256_final(&ctx, hash);

    for (int i = 0; i < 32; ++i) {
        sprintf(sha256 + i * 2, "%02x", hash[i]);
    }
    sha256[64] = '\0';
}
