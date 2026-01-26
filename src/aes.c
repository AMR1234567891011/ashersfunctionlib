#include <stdio.h>
#include <stdint.h>
#define Nr 14
#define Nk 8
static const uint32_t round_constants[] = {
    0x01000000, 0x02000000, 0x04000000, 0x08000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000,
    0x1B000000, 0x36000000
};

static uint8_t Sbox[256] = {
    // 0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,  // 0
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,  // 1
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,  // 2
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,  // 3
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,  // 4
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,  // 5
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,  // 6
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,  // 7
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,  // 8
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,  // 9
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,  // A
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,  // B
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,  // C
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,  // D
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,  // E
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16   // F
};
static uint8_t InvSbox[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};
uint8_t xtime(uint8_t x) {//double in GF(2^8)
    return (x << 1) ^ ((x & 0x80) ? 0x1B : 0x00);
}
// Multiply by 9 (8 + 1)
static inline uint8_t mul_09(uint8_t x) {
    uint8_t x2 = xtime(x);
    uint8_t x4 = xtime(x2);
    uint8_t x8 = xtime(x4);
    return x8 ^ x;
}

// Multiply by 11 (8 + 2 + 1)
static inline uint8_t mul_0b(uint8_t x) {
    uint8_t x2 = xtime(x);
    uint8_t x4 = xtime(x2);
    uint8_t x8 = xtime(x4);
    return x8 ^ x2 ^ x;
}

// Multiply by 13 (8 + 4 + 1)
static inline uint8_t mul_0d(uint8_t x) {
    uint8_t x2 = xtime(x);
    uint8_t x4 = xtime(x2);
    uint8_t x8 = xtime(x4);
    return x8 ^ x4 ^ x;
}

// Multiply by 14 (8 + 4 + 2)
static inline uint8_t mul_0e(uint8_t x) {
    uint8_t x2 = xtime(x);
    uint8_t x4 = xtime(x2);
    uint8_t x8 = xtime(x4);
    return x8 ^ x4 ^ x2;
}
void inv_mix_columns(uint8_t *state) {
    uint8_t col[4];
    for (int c = 0; c < 4; c++) {
        col[0] = state[0 + 4*c];
        col[1] = state[1 + 4*c];
        col[2] = state[2 + 4*c];
        col[3] = state[3 + 4*c];

        state[0 + 4*c] = mul_0e(col[0]) ^ mul_0b(col[1]) ^ mul_0d(col[2]) ^ mul_09(col[3]);
        state[1 + 4*c] = mul_09(col[0]) ^ mul_0e(col[1]) ^ mul_0b(col[2]) ^ mul_0d(col[3]);
        state[2 + 4*c] = mul_0d(col[0]) ^ mul_09(col[1]) ^ mul_0e(col[2]) ^ mul_0b(col[3]);
        state[3 + 4*c] = mul_0b(col[0]) ^ mul_0d(col[1]) ^ mul_09(col[2]) ^ mul_0e(col[3]);
    }
}
void inv_sub_bytes(uint8_t *state) {
    for (int i = 0; i < 16; i++) {
        state[i] = InvSbox[state[i]];
    }
}
void inv_shift_rows(uint8_t *state) {
    uint8_t temp[4];
    temp[0] = state[1+0*4]; 
    temp[1] = state[1+1*4]; 
    temp[2] = state[1+2*4]; 
    temp[3] = state[1+3*4];
    state[1+0*4] = temp[3]; 
    state[1+1*4] = temp[0]; 
    state[1+2*4] = temp[1]; 
    state[1+3*4] = temp[2];

    temp[0] = state[2+0*4]; 
    temp[1] = state[2+1*4]; 
    temp[2] = state[2+2*4]; 
    temp[3] = state[2+3*4];
    state[2+0*4] = temp[2]; 
    state[2+1*4] = temp[3]; 
    state[2+2*4] = temp[0]; 
    state[2+3*4] = temp[1];

    temp[0] = state[3+0*4]; 
    temp[1] = state[3+1*4]; 
    temp[2] = state[3+2*4]; 
    temp[3] = state[3+3*4];
    state[3+0*4] = temp[1]; 
    state[3+1*4] = temp[2]; 
    state[3+2*4] = temp[3]; 
    state[3+3*4] = temp[0];
}

void add_round_key(uint8_t *state, uint32_t *round_key) { //column major in fips actually means row major here
    for (int c = 0; c < 4; c++) {       // 4 columns
        state[0 + 4*c] ^= (round_key[c] >> 24) & 0xFF;
        state[1 + 4*c] ^= (round_key[c] >> 16) & 0xFF;
        state[2 + 4*c] ^= (round_key[c] >> 8) & 0xFF;
        state[3 + 4*c] ^= round_key[c] & 0xFF;
    }
}
void mix_columns(uint8_t *state) { //column major in fips actually means row major here
    uint8_t col[4];
    for (int c = 0; c < 4; c++) {
        col[0] = state[0 + 4*c];
        col[1] = state[1 + 4*c];
        col[2] = state[2 + 4*c];
        col[3] = state[3 + 4*c];
        state[0 + 4*c] = xtime(col[0]) ^ (xtime(col[1]) ^ col[1]) ^ col[2] ^ col[3]; // 2*col0 + 3*col1 + 1*col2 + 1*col3
        state[1 + 4*c] = col[0] ^ xtime(col[1]) ^ (xtime(col[2]) ^ col[2]) ^ col[3]; // 1*col0 + 2*col1 + 3*col2 + 1*col3
        state[2 + 4*c] = col[0] ^ col[1] ^ xtime(col[2]) ^ (xtime(col[3]) ^ col[3]); // 1*col0 + 1*col1 + 2*col2 + 3*col3
        state[3 + 4*c] = (xtime(col[0]) ^ col[0]) ^ col[1] ^ col[2] ^ xtime(col[3]); // 3*col0 + 1*col1 + 1*col2 + 2*col3
    }
}

void shift_rows(uint8_t *state) { //column major in fips actually means row major here
    uint8_t temp[4];
    int row = 1;
    temp[0] = state[row + 0*4];
    temp[1] = state[row + 1*4];
    temp[2] = state[row + 2*4];
    temp[3] = state[row + 3*4];
    state[row + 0*4] = temp[1];
    state[row + 1*4] = temp[2];
    state[row + 2*4] = temp[3];
    state[row + 3*4] = temp[0];
    row = 2;
    temp[0] = state[row + 0*4];
    temp[1] = state[row + 1*4];
    temp[2] = state[row + 2*4];
    temp[3] = state[row + 3*4];
    state[row + 0*4] = temp[2];
    state[row + 1*4] = temp[3];
    state[row + 2*4] = temp[0];
    state[row + 3*4] = temp[1];
    row = 3;
    temp[0] = state[row + 0*4];
    temp[1] = state[row + 1*4];
    temp[2] = state[row + 2*4];
    temp[3] = state[row + 3*4];
    state[row + 0*4] = temp[3];
    state[row + 1*4] = temp[0];
    state[row + 2*4] = temp[1];
    state[row + 3*4] = temp[2];
}
void key_expansion(uint8_t *Key, uint32_t *out){//key is 4 words and output is 60
    int i = 0;

    uint32_t temp;
    while (i < Nk){
        out[i] = (uint32_t) Key[4 * i] << 24 |
                 (uint32_t) Key[4 * i + 1] << 16 |
                 (uint32_t) Key[4 * i + 2] << 8 |
                 (uint32_t) Key[4 * i + 3];
        i++;
    }
    //i = NK here
    while(i < (4 * (Nr + 1))){
        temp = out[i - 1];
        if (i % 8 == 0){
            rot_word(&temp);
            sub_word(&temp);
            temp ^= round_constants[i / 8 - 1];
        } else if (i % 8 == 4) {
            sub_word(&temp);
        }
        out[i] = out[i - 8] ^ temp;
        i++;
    }
    return;
}
void rot_word(uint32_t *W){//takes Word[a0,a1,a2,a3] into word[a1,a2,a3,a0]
    uint32_t temp = *W;
    uint32_t rot = (temp << 8) | (temp >> 24);
    *W = rot;
}
void sub_bytes(uint8_t *b, uint32_t b_length){
    int i;
    for (i = 0; i < b_length; i++){
        b[i] = Sbox[b[i]];
        //b[i] = Sbox[(b[i] & 0xF0) << 4 | b[i] & 0x0F];
    }
}
void sub_word(uint32_t *W){//pass &mem to this!
    //sub_bytes((uint8_t *)W, 4); AES is big endian
    uint32_t x = *W;

    *W =
        ((uint32_t)Sbox[(x >> 24) & 0xFF] << 24) |
        ((uint32_t)Sbox[(x >> 16) & 0xFF] << 16) |
        ((uint32_t)Sbox[(x >>  8) & 0xFF] <<  8) |
        ((uint32_t)Sbox[x & 0xFF]);
}

void cipher(uint8_t *in, uint32_t *w) {
    add_round_key(in, &w[0]);
    for(int r = 1; r < Nr; r++) {
        sub_bytes(in, 16);
        shift_rows(in);
        mix_columns(in);
        add_round_key(in, &w[r*4]);
    }
    sub_bytes(in, 16);
    shift_rows(in);
    add_round_key(in, &w[4 * Nr]);
}
void inv_cipher(uint8_t *in, uint32_t *w) {
    add_round_key(in, &w[Nr * 4]);

    for (int r = Nr - 1; r > 0; r--) {
        inv_shift_rows(in);
        inv_sub_bytes(in);
        add_round_key(in, &w[r * 4]);
        inv_mix_columns(in);
    }
    inv_shift_rows(in);
    inv_sub_bytes(in);
    add_round_key(in, &w[0]);
}

void test_aes256_key_expansion(void)
{
    /* FIPS-197 AES-256 test key */
    uint8_t key[32] = {
        0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
        0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
        0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
        0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F
    };

    uint32_t expanded[60];

    key_expansion(key, expanded);

    /* Known correct words from FIPS-197 */
    uint32_t expected[] = {
        0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F,
        0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F,
        0xA573C29F, 0xA176C498, 0xA97FCE93, 0xA572C09C
    };


    int pass = 1;

    for (int i = 0; i < 12; i++) {
        if (expanded[i] != expected[i]) {
            printf("Mismatch at w[%d]: got %08X, expected %08X\n",
                   i, expanded[i], expected[i]);
            pass = 0;
        }
    }

    if (pass) {
        printf("AES-256 key expansion test  1 PASSED\n");
    }
    uint8_t key_2[32] = {
        0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
        0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
        0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 
        0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
    };

    uint32_t expanded_2[60];

    key_expansion(key_2, expanded_2);

    uint32_t expected_2[8] = {
        0x603deb10, 0x15ca71be, 0x2b73aef0, 0x857d7781,
        0x1f352c07, 0x3b6108d7, 0x2d9810a3, 0x0914dff4
    };

    for (int i = 0; i < 8; i++) {
        if (expanded_2[i] != expected_2[i]) {
            printf("Mismatch at w[%d]: got %08X, expected %08X\n",
                   i, expanded[i], expected[i]);
            pass = 0;
        }
    }

    if (pass) {
        printf("AES-256 key expansion test 2 PASSED\n");
    }

}
void test_aes256_cipher(void) {//https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/AES_Core256.pdf
    uint32_t plaintext[16] = { 
        0x6BC1BEE2, 0x2E409F96, 0xE93D7E11, 0x7393172A, 
        0xAE2D8A57, 0x1E03AC9C, 0x9EB76FAC, 0x45AF8E51, 
        0x30C81C46, 0xA35CE411, 0xE5FBC119, 0x1A0A52EF, 
        0xF69F2445, 0xDF4F9B17, 0xAD2B417B, 0xE66C3710 
    }; 
    uint32_t key[8] = { 
        0x603DEB10, 0x15CA71BE, 0x2B73AEF0, 0x857D7781, 
        0x1F352C07, 0x3B6108D7, 0x2D9810A3, 0x0914DFF4 
    }; 
    uint32_t ciphertext[16] = { 
        0xF3EED1BD, 0xB5D2A03C, 0x064B5A7E, 0x3DB181F8, 
        0x591CCB10, 0xD410ED26, 0xDC5BA74A, 0x31362870, 
        0xB6ED21B9, 0x9CA6F4F9, 0xF153E7B1, 0xBEAFED1D, 
        0x23304B7A, 0x39F9F3FF, 0x067D8D8F, 0x9E24ECC7 
    };
    uint8_t state[64];   // flatten 16 uint32_t to bytes
    uint8_t key_bytes[32];

    // flatten plaintext
    for (int i = 0; i < 16; i++) {
        state[4*i + 0] = (plaintext[i] >> 24) & 0xFF;
        state[4*i + 1] = (plaintext[i] >> 16) & 0xFF;
        state[4*i + 2] = (plaintext[i] >> 8) & 0xFF;
        state[4*i + 3] = (plaintext[i] >> 0) & 0xFF;
    }

    // flatten key
    for (int i = 0; i < 8; i++) {
        key_bytes[4*i + 0] = (key[i] >> 24) & 0xFF;
        key_bytes[4*i + 1] = (key[i] >> 16) & 0xFF;
        key_bytes[4*i + 2] = (key[i] >> 8) & 0xFF;
        key_bytes[4*i + 3] = (key[i] >> 0) & 0xFF;
    }

    uint32_t round_keys[60]; // AES-256 has 60 words
    key_expansion(key_bytes, round_keys);

    // encrypt each block
    for (int b = 0; b < 4; b++) { // 16 uint32_t = 64 bytes, 4 blocks of 16 bytes
        for(int i = 0; i < 16; i++) {
            printf("%02X", state[b*16 + i]);
        }
        printf("\n");
        cipher(&state[b*16], round_keys);
        for(int i = 0; i < 16; i++) {
            printf("%02X", state[b*16 + i]);
        }
        printf("\n");
        inv_cipher(&state[b*16], round_keys);
        for(int i = 0; i < 16; i++) {
            printf("%02X", state[b*16 + i]);
        }
        printf("\n - \n");
    }

}
// int main() {
//     //test_aes256_key_expansion();
//     test_aes256_cipher();
// }
void aes_cbc_256_encrypt(){
    return;
}
void aes_cbc_256_decrypt(){
    return;
}