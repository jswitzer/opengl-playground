#include "bmp.h"
#include "parse.h"
#include <stdio.h>

int test_bmp() {
    uint8_t * pixels = NULL;
    uint16_t h;
    uint16_t w;
    int r = load_bmp("../test/green.bmp", &h, &w, (void**)(&pixels));
    printf("GOT %d, %d, %d %p\n", r, h, w, pixels);
    printf("2CP %p", pixels);
    if (pixels) {
        int i = 0;
        for (int t = 0; t< h; t++) {
            for (int s = 0; s<w; s++) {
                printf("idx %u, s:%u, t:%u", i, s, t);
                printf("r:%u g:%u b:%u a:%u", pixels[y*w+x], pixels[y*w+x+1], pixels[y*w + x+2], pixels[y*w+x+3]);
                i += 4;
            }
        }
    }
/*
    FILE * f = fopen("../test/green.bmp", "rb");
    fseek(f, 0L, SEEK_END);
    uint32_t file_size = ftell(f);
    fseek(f, 0L, SEEK_SET);
    uint8_t * buffer = malloc(file_size);
    printf("file size %lu\n", file_size); 
    for(int i = 0; i < file_size; i++) {
        buffer[i] = (uint8_t)fgetc(f);
    }
    uint8_t * buffer2 = buffer;
    bmp_header_t h;
    bmph_read(&h, &buffer2, 1);
    bmph_print(&h);
    bmp_header_core_t hc;
    bmphc_read(&hc, &buffer2, 1);

    hc.rbitmask = hc.rbitmask >> (32-hc.bits);
    hc.gbitmask = hc.gbitmask >> (32-hc.bits);
    hc.bbitmask = hc.bbitmask >> (32-hc.bits);
    hc.abitmask = hc.abitmask >> (32-hc.bits);

    uint8_t rshift = bmask2bshift(hc.rbitmask);
    uint8_t gshift = bmask2bshift(hc.gbitmask);
    uint8_t bshift = bmask2bshift(hc.bbitmask);
    uint8_t ashift = bmask2bshift(hc.abitmask);
    printf("rs %i, gs %i bs %i as %i", rshift, gshift, bshift, ashift);

    bmphc_print(&hc);
    if (hc.bits == 16) {
        int x = 0;
        int y = 0;
        int off = h.offset;
        int idx = 0;
        while (y < hc.height) {
            while (x < hc.width) {
                uint16_t raw = ((uint16_t)(buffer[off])) << 8 + ((uint16_t)buffer[off+1]);
                printf("pixel x:%d, y:%d, raw:%x\n",x, y, raw);
                x++;
                off += 2;
            }
            x =0;
            y ++;
        }
    }
    if (hc.bits == 32) {
        int x = 0;
        int y = 0;
        int off = h.offset;
        int idx = 0;
        while (y < hc.height) {
            while (x < hc.width) {
                uint32_t r1 = ((uint32_t)(buffer[off])) << 24;
                uint32_t r2 = ((uint32_t)(buffer[off+1])) << 16;
                uint32_t r3 = ((uint32_t)(buffer[off+2])) << 8;
                uint32_t r4 = ((uint32_t)(buffer[off+3]));
                uint32_t raw = r1+r2+r3+r4;
                uint8_t r = (raw & hc.rbitmask) >> rshift;
                uint8_t g = (raw & hc.gbitmask) >> gshift;
                uint8_t b = (raw & hc.bbitmask) >> bshift;
                uint8_t a = (raw & hc.abitmask) >> ashift; 
                //printf("pixel raw %x, x:%d, y:%d, r1:%x r2:%x r3:%x r4:%x", raw, x,y, r1, r2, r3, r4);
                printf("pixel x:%d, y:%d, raw:%x r:%x g:%x b:%x a:%x\n",x, y, raw, r,g,b,a );

                x++;
                off += 4;
            }
            x =0;
            y ++;
        }
    }
    */
}

int main() {
    char * a= "\xFF\x00\x00\x00";
    char * b = a;
    printf("Testing bitmap parsing\n");
    test_bmp();
    printf("BE parsing of FF000000: %x\n", r32word(&b, 0));
    b = a;
    printf("LE parsing of FF000000: %x\n", r32word(&b, 1));
}
