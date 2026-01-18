#ifndef AES_H
#define AES_H
void key_expansion(uint8_t *Key, uint32_t *out);
void cipher(uint8_t *in, uint32_t *w);
void inv_cipher(uint8_t *in, uint32_t *w);
#endif