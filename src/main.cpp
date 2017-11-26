#include <cstdio>
#include <cassert>
#include <iostream>
#include <lzma.h>
#include <cstring>
#include "sha256.h"


void sha256_file(const char* path)
{
    SHA256_CTX ctx;
    sha256_init(&ctx);

    FILE *file = fopen(path, "rb");
    assert(file);

    while (true) {
        const int BUF_SIZE = 1024 * 1024;
        char buffer[BUF_SIZE];
        const size_t readCnt = fread(buffer, 1, BUF_SIZE, file);
        if (readCnt == 0) break;

        sha256_update(&ctx, (const BYTE*)buffer, (uint32_t)readCnt);
    }

    fclose(file);

    BYTE hash[32];
    sha256_final(&ctx, hash);

    char sha256[64];
    for (int i = 0; i < 32; ++i) {
        sprintf(sha256 + i * 2, "%02x", hash[i]);
    }
    sha256[64] = '\0';

    printf("%s  %s\n", sha256, path);
}


int main(int argc, char** argv)
{
    lzma_options_lzma opt_lzma2;
    lzma_bool fail = lzma_lzma_preset(&opt_lzma2, LZMA_PRESET_DEFAULT);
    assert(!fail);

    lzma_stream strm = LZMA_STREAM_INIT;

    const lzma_filter filters[] = {
        { .id = LZMA_FILTER_X86,    .options = nullptr },
        { .id = LZMA_FILTER_LZMA2,  .options = &opt_lzma2 },
        { .id = LZMA_VLI_UNKNOWN,   .options = nullptr },
    };
    lzma_ret ret = lzma_stream_encoder(&strm, filters, LZMA_CHECK_CRC64);
    assert(ret == LZMA_OK);

    if (argc == 1) {
        sha256_file("/usr/bin/docker");
    }
    else {
        sha256_file(argv[1]);
    }
    return 0;
}
