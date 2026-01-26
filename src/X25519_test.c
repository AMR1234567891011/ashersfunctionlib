#include <stdio.h>

// Examples taken from the NaCl source code downloaded from https://nacl.cr.yp.to/install.html
// Files: tests/scalarmult*

unsigned char alicesk[32] = {
 0x77,0x07,0x6d,0x0a,0x73,0x18,0xa5,0x7d
,0x3c,0x16,0xc1,0x72,0x51,0xb2,0x66,0x45
,0xdf,0x4c,0x2f,0x87,0xeb,0xc0,0x99,0x2a
,0xb1,0x77,0xfb,0xa5,0x1d,0xb9,0x2c,0x2a
};

unsigned char alicepk[32] = {
 0x85,0x20,0xf0,0x09,0x89,0x30,0xa7,0x54
,0x74,0x8b,0x7d,0xdc,0xb4,0x3e,0xf7,0x5a
,0x0d,0xbf,0x3a,0x0d,0x26,0x38,0x1a,0xf4
,0xeb,0xa4,0xa9,0x8e,0xaa,0x9b,0x4e,0x6a
} ;

unsigned char bobsk[32] = {
 0x5d,0xab,0x08,0x7e,0x62,0x4a,0x8a,0x4b
,0x79,0xe1,0x7f,0x8b,0x83,0x80,0x0e,0xe6
,0x6f,0x3b,0xb1,0x29,0x26,0x18,0xb6,0xfd
,0x1c,0x2f,0x8b,0x27,0xff,0x88,0xe0,0xeb
} ;

unsigned char bobpk[32] = {
 0xde,0x9e,0xdb,0x7d,0x7b,0x7d,0xc1,0xb4
,0xd3,0x5b,0x61,0xc2,0xec,0xe4,0x35,0x37
,0x3f,0x83,0x43,0xc8,0x5b,0x78,0x67,0x4d
,0xad,0xfc,0x7e,0x14,0x6f,0x88,0x2b,0x4f
} ;

typedef unsigned char u8;
typedef long long i64;
typedef i64 field_elem[16];
static const u8 _9[32] = {9};
static const field_elem _121665 = {0xDB41, 1};

static void unpack25519(field_elem out, const u8 *in)
{
  int i;
  for (i = 0; i < 16; ++i) out[i] = in[2*i] + ((i64) in[2*i + 1] << 8);
  out[15] &= 0x7fff;
}

static void carry25519(field_elem elem)
{
  int i;
  i64 carry;
  for (i = 0; i < 16; ++i) {
    carry = elem[i] >> 16;
    elem[i] -= carry << 16;
    if (i < 15) elem[i + 1] += carry; else elem[0] += 38 * carry;
  }
}

static void fadd(field_elem out, const field_elem a, const field_elem b) /* out = a + b */
{
  int i;
  for (i = 0; i < 16; ++i) out[i] = a[i] + b[i];
}

static void fsub(field_elem out, const field_elem a, const field_elem b) /* out = a - b */
{
  int i;
  for (i = 0; i < 16; ++i) out[i] = a[i] - b[i];
}

static void fmul(field_elem out, const field_elem a, const field_elem b) /* out = a * b */
{
  i64 i, j, product[31];
  for (i = 0; i < 31; ++i) product[i] = 0;
  for (i = 0; i < 16; ++i) {
    for (j = 0; j < 16; ++j) product[i+j] += a[i] * b[j];
  }
  for (i = 0; i < 15; ++i) product[i] += 38 * product[i+16];
  for (i = 0; i < 16; ++i) out[i] = product[i];
  carry25519(out);
  carry25519(out);
}

static void finverse(field_elem out, const field_elem in)
{
  field_elem c;
  int i;
  for (i = 0; i < 16; ++i) c[i] = in[i];
  for (i = 253; i >= 0; i--) {
    fmul(c,c,c);
    if (i != 2 && i != 4) fmul(c,c,in);
  }
  for (i = 0; i < 16; ++i) out[i] = c[i];
}

static void swap25519(field_elem p, field_elem q, int bit)
{
  i64 t, i, c = ~(bit - 1);
  for (i = 0; i < 16; ++i) {
    t = c & (p[i] ^ q[i]);
    p[i] ^= t;
    q[i] ^= t;
  }
}

static void pack25519(u8 *out, const field_elem in)
{
  int i, j, carry;
  field_elem m, t;
  for (i = 0; i < 16; ++i) t[i] = in[i];
  carry25519(t); carry25519(t); carry25519(t);
  for (j = 0; j < 2; ++j) {
    m[0] = t[0] - 0xffed;
    for(i = 1; i < 15; i++) {
      m[i] = t[i] - 0xffff - ((m[i-1] >> 16) & 1);
      m[i-1] &= 0xffff;
    }
    m[15] = t[15] - 0x7fff - ((m[14] >> 16) & 1);
    carry = (m[15] >> 16) & 1;
    m[14] &= 0xffff;
    swap25519(t, m, 1 - carry);
  }
  for (i = 0; i < 16; ++i) {
    out[2*i] = t[i] & 0xff;
    out[2*i + 1] = t[i] >> 8;
  }
}

void scalarmult(u8 *out, const u8 *scalar, const u8 *point)
{
  u8 clamped[32];
  i64 bit, i;
  field_elem a, b, c, d, e, f, x;
  for (i = 0; i < 32; ++i) clamped[i] = scalar[i];
  clamped[0] &= 0xf8;
  clamped[31] = (clamped[31] & 0x7f) | 0x40;
  unpack25519(x, point);
  for (i = 0; i < 16; ++i) {
    b[i] = x[i];
    d[i] = a[i] = c[i] = 0;
  }
  a[0] = d[0] = 1;
  for (i = 254; i >= 0; --i) {
    bit = (clamped[i >> 3] >> (i & 7)) & 1;
    swap25519(a, b, bit);
    swap25519(c, d, bit);
    fadd(e, a, c);
    fsub(a, a, c);
    fadd(c, b, d);
    fsub(b, b, d);
    fmul(d, e, e);
    fmul(f, a, a);
    fmul(a, c, a);
    fmul(c, b, e);
    fadd(e, a, c);
    fsub(a, a, c);
    fmul(b, a, a);
    fsub(c, d, f);
    fmul(a, c, _121665);
    fadd(a, a, d);
    fmul(c, c, a);
    fmul(a, d, f);
    fmul(d, b, x);
    fmul(b, e, e);
    swap25519(a, b, bit);
    swap25519(c, d, bit);
  }
  finverse(c, c);
  fmul(a, a, c);
  pack25519(out, a);
}

void scalarmult_base(u8 *out, const u8 *scalar)
{ 
  scalarmult(out, scalar, _9);
}

void generate_keypair(u8 *pk, u8 *sk)
{
  randombytes(sk, 32);
  scalarmult_base(pk, sk);
}

void x25519(u8 *out, const u8 *pk, const u8 *sk)
{
  scalarmult(out, sk, pk);
}

// int main()
// {
//           //Test_Operations();
//           u8 S1_big_endian[32] = { // Big-endian test vector
//             0xa5, 0x46, 0xe3, 0x6b, 0xf0, 0x52, 0x7c, 0x9d,
//             0x3b, 0x16, 0x15, 0x4b, 0x82, 0x46, 0x5e, 0xdd,
//             0x62, 0x14, 0x4c, 0x0a, 0xc1, 0xfc, 0x5a, 0x18,
//             0x50, 0x6a, 0x22, 0x44, 0xba, 0x44, 0x9a, 0xc4
//         };
    
//         u8 U1_big_endian[32] = { // Big-endian test vector
//             0xe6, 0xdb, 0x68, 0x67, 0x58, 0x30, 0x30, 0xdb,
//             0x35, 0x94, 0xc1, 0xa4, 0x24, 0xb1, 0x5f, 0x7c,
//             0x72, 0x66, 0x24, 0xec, 0x26, 0xb3, 0x35, 0x3b,
//             0x10, 0xa9, 0x03, 0xa6, 0xd0, 0xab, 0x1c, 0x4c
//         };
    
//         u8 S1[32];
//         u8 U1[32];
    
//         //reverse the input.
//         for (int i = 0; i < 32; i++){
//             S1[i] = S1_big_endian[31-i];
//             U1[i] = U1_big_endian[31-i];
//         }
//         u8 out[32];
    
//         scalarmult(out, U1, S1);
    
//         printf("\nout: ");
//         for (int i = 0; i < 32; i++) {
//             printf("%02x", out[i]);
//         }
//         printf("\n");
//         fflush(stdout);
//   int i;
//   unsigned char key[32];
//   scalarmult_base(key, alicesk);
//   printkey(key);
//   printf("\n");

//   scalarmult_base(key, bobsk);
//   printkey(key);
//   printf("\n");

//   scalarmult(key, alicesk, bobpk);
//   printkey(key);
//   printf("\n");

//   scalarmult(key, bobsk, alicepk);
//   printkey(key);
//   return 0;
// }