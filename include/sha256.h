#ifndef SHA256_H
#define SHA256_H

#include <stdint.h>
#include <string.h>

void sha256(const uint8_t *in, uint32_t input_length, uint8_t *out);
void hmac_sha256(const uint8_t *in, const uint8_t *key, uint32_t key_length, uint32_t input_length, uint8_t *out);
void hkdf_extract(uint8_t *IKM, uint32_t IKM_length, uint8_t *out);
void hkdf_extract_salt(uint8_t *IKM, uint32_t IKM_length, uint8_t *salt, uint32_t salt_length, uint8_t *out);
void hkdf_expand(uint32_t L, uint8_t *info, uint32_t info_length, uint8_t *PRK, uint32_t PRK_length, uint8_t *out);

#endif