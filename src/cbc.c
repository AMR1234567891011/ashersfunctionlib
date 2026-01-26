#include <stdint.h>
#include <stdio.h>
#include "../include/aes.h"
void print_state(uint8_t *state) {
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            printf("%02X", state[4*row + col]); // column-major order
        }
        printf(" ");
    }
    printf("\n");
}
void print_state_block(uint8_t *state) {
    for (int i = 0; i < 16; i++) {
        printf("%02X", state[i]);
    }
    printf("\n");
}
void pcks7_pad(uint8_t *buffer, uint32_t current_len, uint32_t *new_len) {//adds the number of x padded bytes as the last x bytes
    uint8_t padding_val = 16 - (current_len % 16);

    for (uint32_t i = 0; i < padding_val; i++) {
        buffer[current_len + i] = padding_val;
    }
    *new_len = current_len + padding_val;
}
void pcks7_unpad(uint8_t *buffer, uint32_t current_len, uint32_t *new_len) {
    uint32_t padding_val = buffer[current_len - 1];
    for(uint32_t i = 0; i < padding_val; i++) {
        buffer[current_len - 1 - i] = 0x00;
    }
    *new_len = (current_len - padding_val);
}
void block_xor(uint8_t *out, const uint8_t* a, const uint8_t *b) { // out = a xor b
    uint64_t *a64 = (uint64_t *)a;
    uint64_t *b64 = (uint64_t *)b;
    uint64_t *res = (uint64_t *)out;
    res[0] = a64[0] ^ b64[0];
    res[1] = a64[1] ^ b64[1];
}
void block_xor_i(uint8_t *a, uint8_t * b) {// a = a xor b
    uint64_t *a64 = (uint64_t *)a;
    uint64_t *b64 = (uint64_t *)b;
    a64[0] = a64[0] ^ b64[0];
    a64[1] = a64[1] ^ b64[1];
}
void cbc_encrypt(uint8_t *IV, uint8_t* plaintext, uint32_t plaintext_len, uint8_t *key, uint8_t *ciphertext, uint32_t *ciphertext_len) {
    uint32_t round_keys[60];
    uint8_t *prev_block = IV;
    ciphertext_len = &plaintext_len;
    key_expansion(key, round_keys);
    for(int i = 0; i < (plaintext_len / 16); i++) {//assuming pcks7 padding
        uint8_t *curr_pt = &plaintext[i * 16];
        uint8_t *curr_ct = &ciphertext[i * 16];
        block_xor(curr_ct, curr_pt, prev_block);
        cipher(curr_ct, round_keys);
        prev_block = curr_ct;
    }
}
void cbc_decrypt(uint8_t *IV, uint8_t *ciphertext,uint32_t ciphertext_len, uint8_t *key, uint8_t *plaintext, uint32_t *plaintext_len) {
    uint8_t block[16] = {0x00};
    uint8_t z[16] = {0x00};
    uint32_t round_keys[60];
    uint8_t *prev_block = IV;
    uint8_t *curr_pt;
    uint8_t *curr_ct;
    key_expansion(key, round_keys);
    for(int i = 0; i < (ciphertext_len / 16); i++) {//assuming pcks7 padding
        curr_pt = &plaintext[i * 16];
        curr_ct = &ciphertext[i * 16];
        block_xor(block, curr_ct, z);
        inv_cipher(block, round_keys);
        block_xor(curr_pt, block, prev_block);
        prev_block = curr_ct;
    }
    pcks7_unpad(plaintext, ciphertext_len, plaintext_len);
}
void cbc_tests(){
    uint8_t msg[496] = "I was working in the lab late one night When my eyes beheld an eerie sight For my monster, from his slab, began to rise And suddenly, to my surprise (He did the Mash) he did the Monster Mash (The Monster Mash) it was a graveyard smash (He did the Mash) it caught on in a flash (He did the Mash) he did the Monster Mash From my laboratory in the Castle east To the master bedroom, where the vampires feast The ghouls all came from their humble abodes To get a jolt from my electrodes";
    uint32_t len = 482;//real length pre pad
    uint32_t *new_len = &len;
    pcks7_pad(msg, len, new_len);
    uint8_t ciphertext[496] = {0x00};
    uint32_t ciphertext_len = 0;
    uint8_t IV[16] = {0x00};
    uint8_t key[32] = {
        0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
        0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
        0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
        0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F
    };
    //key: 0x000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F
    cbc_encrypt(IV, msg, *new_len, key, ciphertext, &ciphertext_len);
    printf("\nencrypted: len: %d\n", ciphertext_len);
    for(int i = 0; i < ciphertext_len; i++) {
        printf("%02X", ciphertext[i]);
    }
    uint8_t output[512];
    uint32_t output_len = 0;
    cbc_decrypt(IV, ciphertext, ciphertext_len, key, output, &output_len);
    printf("\ndecrypted: len: %d\n", output_len);
    for(int i  = 0; i < output_len; i++) {
        printf("%c", output[i]);
    }

    uint8_t plaintext[32] = {0x00};
    uint8_t ciphertext_2[32] = {0x00};
    uint32_t ciphertext_len_2 = 0;
    uint32_t new_plaintext_2 = 16;
    uint8_t out[32];
    uint32_t out_len = 0;
    pcks7_pad(plaintext, 16, &new_plaintext_2);
    cbc_encrypt(IV, plaintext, new_plaintext_2, key, ciphertext_2, &ciphertext_len_2);
    printf("\n\n\nencrypted");
    printf("\npadded_len: %d\n", ciphertext_len_2);
    for(int i = 0; i < ciphertext_len_2; i++) {
        printf("%02X", ciphertext_2[i]);
    }
    printf("\ndecrypted\n");
    cbc_decrypt(IV, ciphertext_2, ciphertext_len_2, key, out, &out_len);
        for(int i = 0; i < out_len; i++) {
        printf("%02X", out[i]);
    }
}
// int main() {
//     cbc_tests();
//     return 0;
// }