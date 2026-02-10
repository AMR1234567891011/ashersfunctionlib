#ifndef CBC_H
#define CBC_H
#include <stdint.h>

void cbc_encrypt(uint8_t *IV, uint8_t *plaintext, uint32_t plaintext_len, uint8_t *key, uint8_t *ciphertext, uint32_t ciphertext_len);
void cbc_decrypt(uint8_t *IV, uint8_t *plaintext, uint32_t plaintext_len, uint8_t *key, uint8_t *ciphertext, uint32_t ciphertext_len);
void pcks7_pad(uint8_t *buffer, uint32_t current_len, uint32_t *new_len);
#endif