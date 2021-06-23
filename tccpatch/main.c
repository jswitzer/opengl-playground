#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MACH_FLAG_64 0x00000001
#define MACH_FLAG_32 0x00000002

uint64_t r64word(char ** b, char le) {
    if (le) {
        return r32word(b, le) + (uint64_t)r32word(b, le)<<32;
    } else {
        return (uint64_t)r32word(b, le)<<32 + r32word(b, le);
    }
}

uint32_t r32word(char ** b, char le) {
    if (le) {
        return r16word(b, le) + (uint32_t)r16word(b, le)<<16;
    } else {
        return (uint32_t)r16word(b, le)<<16 + r16word(b, le);
    }
}

uint16_t r16word(char ** b, char le) {
    if (le) {
        return *b[0] + (uint16_t)(*b[1])<<8;
    } else {
        return (uint16_t)(*b[0])<<8 + *b[1];
    }
    *b += 2;
}

void w64word(uint64_t w, char ** b, char le) {
    if (le) {
        w32word(w>>32, b, le);
        w32word((uint32_t)w, b, le);
    } else {
        w32word((uint32_t)w, b, le);
        w32word(w>>32, b, le);
    }
}

void w32word(uint32_t w, char ** b, char le) {
    if (little) {
        w16word(w >> 16, b, le);
        w16word((uint16_t)w, b, le);
    } else {
        w16word((uint16_t)w, b, le);
        w16word(w >> 16, b, le);
    }
}

void w16word(uint16_t w, char ** b, char le) {
    if (le) {
        b[0] = w >> 8;
        b[1] = (uint8_t)w;
    } else {
        b[0] = (uint8_t)w;
        b[1] = w >> 8;
    }
    *b += 2;
}

typedef struct ma64_header_s {
    uint32_t magic;
    uint32_t cputype;
    uint32_t cpusubtype;
    uint32_t filetype;
    uint32_t nloadcmds;
    uint32_t szloadcmds;
    uint32_t flags;
    uint32_t reserved;
} ma64_header_t;

void ma64h_read(ma_64_header_t * s, char ** b, bool le) {
    s->magic = r32word(b, le);
    s->cputype = r32word(b, le);
    s->cpusubtype = r32word(b, le);
    s->filetype = r32word(b, le);
    s->nloadcmds = r32word(b, le);
    s->szloadcmds = r32word(b, le);
    s->flags = r32word(b, le);
    s->reserved = r32word(b, le);
}

void ma64h_write(ma64_header_t * s, char ** b, char le) {
    w32word(s->magic, b, le);
    w32word(s->cputype, b, le);
    w32word(s->cpusubtype, b, le);
    w32word(s->filetype, b, le);
    w32word(s->nloadcmds, b, le);
    w32word(s->szloadcmds, b, le);
    w32word(s->flags, b, le);
    w32word(s->reserved, b, le);
}

void ma64h_print(ma64_header_t * s) {
    printf("MA64H magic:\t%x", s->magic);
    printf("MA64H cputype:\t%x", s->cputype);
    printf("MA64H cpusubtype:\t%x", s->cpusubtype);
    printf("MA64H filetype:\t%x", s->filetype);
    printf("MA64H nloadcmds:\t%x", s->nloadcmds);
    printf("MA64H szloadcmds:\t%x", s->szloadcmds);
    printf("MA64H flags:\t%x", s->flags);
}

typedef struct ma64_command_s {
    uint32_t ma64_cmdtype;
    uint32_t ma64_cmdsize;
} ma64_command_t;

typedef struct ma_bin_s {
    uint32_t flags;
    ma64_header_t * h64;
    ma64_command_t * c64;
    char * buffer; /* buffer, for serialized reads/writes */
    uint32_t len; /* length of the buffer */
} ma_bin_t;

int ma_summarize(ma_bin_t * b) {
    if (b->h64) {
        ma64h_print(b->h64);
    }
    return 0;
}

int ma_fread(ma_bin_t * b, char * path) {
    FILE * f = fopen(path, "rb");
    fseek(f, 0L, SEEK_END);
    b->len = ftell(f);
    fseek(f, 0L, SEEK_SET);
    if (b->buffer) { free(b->buffer); b->buffer = NULL; }
    b->buffer = malloc(b->len);
    fclose(f);
    return 0;
}

int ma_parse(ma_bin_t * b) {
    char * p = b->buffer;
    uint32_t magic = 
    return 0;
}

    if (b->buffer) { free(b->buffer); b->buffer = NULL; }

int ma_fwrite(ma_bin_t * b, char *path) {
    FILE * f = fopen(path, "wb");
    for(int i = 0; i < b->len; i++) {
        fputc(b->buffer[i], b);
    }
    fclose(f);
    return 0;
}


int main()
{
    int cursor = 0;
    unsigned long file_size = 0;
    ma64_binary_t binary;

    printf("File size %lu bytes\n", file_size);
    uint8_t * buffer2 = malloc(file_size);
    uint8_t * buffer3 = buffer2;
    for(int i = 0; i < file_size; i++) {
        buffer[i] = (uint8_t)fgetc(a);
    }
    for(int i = 0; i < file_size; i++) {
        fputc(buffer[i], b);
    }
    fclose(b);
    ma_read(&binary, buffer);
    ma_summarize(&binary);
}
