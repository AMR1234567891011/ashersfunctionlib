#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

typedef uint8_t key[32];
void hash_test();
void hmac_test();
void hkdf_test();
void X25519_test();
void X3DH_test();

#endif