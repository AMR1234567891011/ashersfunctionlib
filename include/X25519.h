#ifndef X25519_H
#define X25519_H

#include <stdint.h>
// typedef long long int64_t;
typedef int64_t elem[16];
static const elem _121665 = {0xDB41,1};
static const uint8_t _9[32] = {9};

void scalar_mult(uint8_t *out, const uint8_t *scalar, const uint8_t *point);
void printkey(unsigned char *buf);
void printlongkey(unsigned char *buf);

#endif // X25519_H
