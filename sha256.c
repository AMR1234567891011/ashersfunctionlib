#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "sha256.h"

uint8_t salt[] = { 0xde,0x9e,0xdb,0x7d,0x7b,0x7d,0xc1,0xb4,0xd3,0x5b,0x61,0xc2,0xec,0xe4,0x35,0x37,0x3f,0x83,0x43,0xc8,0x5b,0x78,0x67,0x4d,0xad,0xfc,0x7e,0x14,0x6f,0x88,0x2b,0x4f};
//0xde9edb7d7b7dc1b4d35b61c2ece435373f8343c85b78674dadfc7e146f882b4f
#define HKDF_SALT salt
#define HKDF_SALT_LEN 32
void printkey_len(unsigned char *buf, uint32_t len)
{
  for (int i = 0;i < len;++i) {
    printf("%02x",(unsigned int) buf[i]);
  }
}
/*
TRY TO REPLACE MEMMOVE WITH SOME CONSTANT TIME THING
*/
//SHA-256 internal functions
static uint32_t ROTR(uint32_t x, int n){//right rotate
    return ((x >> n) | (x << (32- n )));
}
static uint32_t Ch(uint32_t x, uint32_t y, uint32_t z){//choose
    return ((x & y) ^((~x) & z));
}
static uint32_t Maj(uint32_t x, uint32_t y, uint32_t z){//majority
    return ((x & y) ^ (x & z) ^ (y & z));
}
static uint32_t S0(uint32_t x){//Big Sigma function 0 32 bit word
    return ((ROTR(x, 2) ^ ROTR(x,13) ^ ROTR(x, 22)));
}
static uint32_t S1(uint32_t x){//Big Sigma function 1 32 bit word
    return (ROTR(x, 6)  ^ ROTR(x, 11) ^ ROTR(x, 25));
}
static uint32_t s0(uint32_t x){//little sigma function 0 32 bit word
    return (ROTR(x, 7) ^ ROTR(x, 18) ^ (x >> 3));
}
static uint32_t s1(uint32_t x){//little sigma function 1 32 bit word
    return (ROTR(x,17) ^ ROTR(x, 19) ^ (x >> 10));
}
//SHA256 function defined in https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf
void sha256(const uint8_t *in, uint32_t input_length, uint8_t *out){
    uint32_t bit_length = input_length * 8;
    uint32_t pad_length  = 56 - ((input_length + 1) % 64);//add one for the 0x80 byte and find the padding length of last packet
    pad_length = pad_length + 64 * (pad_length > 63);
    uint32_t total_length = input_length + 1 + pad_length + 8;
    //printf("input_length: %d, bit_length: %d, pad_length: %d, total_length: %d", input_length, bit_length, pad_length,total_length);
    uint8_t padded_buffer[1000] = {0x00};
    memcpy(padded_buffer, in, (size_t) input_length);
    padded_buffer[input_length] = 0x80;
    int i;
    for (i = 4; i < 8 ; i++ ){//appends the bitlength of the messaage to the final 8 bytes
        padded_buffer[ total_length - 8 + i ] = (bit_length >> (8 * (7 - i)));
    }
    uint32_t H[8] = {
        0x6a09e667,
        0xbb67ae85,
        0x3c6ef372,
        0xa54ff53a,
        0x510e527f,
        0x9b05688c,
        0x1f83d9ab,
        0x5be0cd19
    };
    const uint32_t K[64] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };
    uint32_t W[64] = {0x00000000};//schedule
    uint32_t a,b,c,d,e,f,g,h,T1,T2 = 0;
    int j, t;
    uint8_t block[64] = {0x00};
    //MAIN LOOP
    for (j = 0 ; j < total_length; j += 64) { //main loop
       memmove(block, (padded_buffer + j ), 64);
       for ( t = 0; t < 64; t++ ) { //create schedule
        if (t < 16){
            W[t] = (block[ 4* t] << 24) | (block[4 * t + 1] << 16) | (block[4 * t + 2] << 8) | block[4 * t + 3]; 
        } else {
            W[t] = (s0(W[t - 15]) + W[t - 7] + s1(W[t - 2]) + W[t - 16]);
        }
       }
       a = H[0];
       b = H[1];
       c = H[2];
       d = H[3];
       e = H[4];
       f = H[5];
       g = H[6];
       h = H[7];
       T1 = 0;
       T2 = 0;
       for ( t = 0 ; t < 64 ; t++ ) {
        T1 = (h + S1(e) + Ch(e, f, g) + K[t] + W[t]);
        T2 = (S0(a) + Maj(a,b,c));
        h = g;
        g = f;
        f = e;
        e = (d + T1);
        d = c;
        c = b;
        b = a;
        a = (T1 + T2);
       }
       H[0] = (H[0] + a);
       H[1] = (H[1] + b);
       H[2] = (H[2] + c);
       H[3] = (H[3] + d);
       H[4] = (H[4] + e);
       H[5] = (H[5] + f);
       H[6] = (H[6] + g);
       H[7] = (H[7] + h);
    }//main loop ends
    for(i = 0; i < 8; i++) {
        out[4 * i] = H[i] >> 24;
        out[4 * i + 1] = H[i] >> 16;
        out[4 * i + 2] = H[i] >> 8;
        out[4 * i + 3] = H[i];
    }  
    return;
}

void hmac_sha256(const uint8_t *in, const uint8_t *key, uint32_t key_length, uint32_t input_length, uint8_t *out){
    //This HMAC hashes the message before
    int i;
    uint8_t K[64] = {0x00};
    if (key_length > 64) {
        sha256(key, key_length, K);
    }else {
        memmove(K, key, key_length);
    }
    uint8_t KI[64] = {0x00};
    uint8_t KO[64] = {0x00};
    for (i  = 0; i < 64 ; i++){
        KI[i] = K[i] ^ 0x36;
        KO[i] = K[i] ^ 0x5c;
    }
    uint8_t *inner = calloc(64 + input_length, 1);
    memmove(inner + 64, in, input_length);
    memmove(inner, KI, 64);
    uint8_t outer[96] = {0x00};
    memmove(outer, KO, 64);
    sha256(inner, (64 + input_length), (outer + 64));
    free(inner);
    sha256(outer, 96, out);
    return;
}
//https://datatracker.ietf.org/doc/html/rfc5869
void hkdf_extract(uint8_t *IKM, uint32_t IKM_length, uint8_t *out){
    //get a salt, and IKM or input keying material
    //uses HKDF_SALT from header
    hmac_sha256(IKM, HKDF_SALT, HKDF_SALT_LEN, IKM_length, out);
    return;
}
void hkdf_extract_salt(uint8_t *IKM, uint32_t IKM_length, uint8_t *salt, uint32_t salt_length, uint8_t *out) {
    hmac_sha256(IKM, salt, IKM_length, salt_length, out);
    return;
}
//https://datatracker.ietf.org/doc/html/rfc5869
void hkdf_expand(uint32_t L, uint8_t *info, uint32_t info_length, uint8_t *PRK, uint32_t PRK_length, uint8_t *out){
    //L is the desired length of keying material
    uint32_t i, T_length;
    int N = (L + 31) / 32; //celing function
    T_length = 33 + info_length;
    uint8_t *T = calloc(T_length, 1);
    if (info_length > 0){
        memmove((T+32), info, info_length);
    }
    i = 0;
    T[T_length - 1] = 0x01;
    hmac_sha256((T + 32), PRK, PRK_length, (info_length + 1), T);
    uint32_t copy_length = (L - (i * 32) > 32) ? 32 : (L - (i * 32));
    memmove((out + (i * 32)), T, copy_length);
    for (i = 1 ; i < N; i++){
        T[T_length - 1] = (uint8_t) i + 1;
        hmac_sha256(T, PRK, PRK_length, T_length, T);
        copy_length = (L - (i * 32) > 32) ? 32 : (L - (i * 32));
        memmove((out + (i * 32)), T, copy_length);
    }
    free(T);
    return;
}
