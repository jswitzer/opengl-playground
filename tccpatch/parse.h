#ifndef _PARSE_H
#define _PARSE_H

#include <stdint.h>

uint64_t r64word(char ** b, char le);
uint32_t r32word(char ** b, char le);
uint16_t r16word(char ** b, char le);

void w64word(uint64_t w, char ** b, char le);
void w32word(uint32_t w, char ** b, char le);
void w16word(uint16_t w, char ** b, char le);
#endif
