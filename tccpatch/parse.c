#include "parse.h"

uint64_t r64word(char ** b, char le) {
    if (le) {
        return r32word(b, le) + ((uint64_t)r32word(b, le)<<32);
    } else {
        return ((uint64_t)r32word(b, le)<<32) + r32word(b, le);
    }
}

uint32_t r32word(char ** b, char le) {
    if (le) {
        return r16word(b, le) + ((uint32_t)r16word(b, le)<<16);
    } else {
        return ((uint32_t)r16word(b, le)<<16) + r16word(b, le);
    }
}

uint16_t r16word(char ** b, char le) {
    if (le) {
        return *b[0] + ((uint16_t)(*b[1])<<8);
    } else {
        return ((uint16_t)(*b[0])<<8) + *b[1];
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
    if (le) {
        w16word(w >> 16, b, le);
        w16word((uint16_t)w, b, le);
    } else {
        w16word((uint16_t)w, b, le);
        w16word(w >> 16, b, le);
    }
}

void w16word(uint16_t w, char ** b, char le) {
    if (le) {
        (*b)[0] = w >> 8;
        (*b)[1] = (uint8_t)w;
    } else {
        (*b)[0] = (uint8_t)w;
        (*b)[1] = w >> 8;
    }
    *b += 2;
}

