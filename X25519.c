#include <stdio.h>
#include <stdint.h>


//Little Endian !!!
// typedef long long int64_t;
typedef int64_t elem[16];//16 x 16 bytes, use uint64 to prevent overflows.
static const elem _121665={0xDB41,1};
static const uint8_t _9[32] = {9};


static void carry25519(elem elt) {
    int64_t carry;
    for (int i = 0; i < 16; i++) {
        carry = elt[i] >> 16;
        elt[i] -= carry << 16;
        //printf("CARRY state before i = %d: elt[0]:%x elt[15]: %x\n", i, elt[0], elt[15]);
        if (i < 15) {
            elt[i + 1] += carry;
        } else {
            //printf("CARRY state mult i = %d: 38 * carry = dec: %d hex: %x\n", i, 38 * carry, 38 * carry);
            elt[0] += (38 * carry);
        }
        //printf("CARRY state after i = %d: elt[0]:%x elt[15]: %x\n", i, elt[0], elt[15]);
    }

}

static void unpack25519(elem out, const unsigned char *in) {
    for (int i = 0; i < 16; i++) out[i] = in[2 * i] + ((int64_t)in[2 * i + 1] << 8);
    out[15] &= 0x7fff;
}

static void swap25519(elem p, elem q, int bit) {
    uint64_t t, i, c = ~(bit - 1);
    for (i = 0; i < 16; i++) {
        t = c & (p[i] ^ q[i]);
        p[i] ^= t;
        q[i] ^= t;
    }
}

static void pack25519(uint8_t *out, const elem in) {
    int i, j, carry;
    elem m, t;
    for (i = 0; i < 16; i++) {
        t[i] = in[i];
    }
    carry25519(t);
    carry25519(t);
    for (j = 0; j < 2; j++) {
        m[0] = t[0] - 0xffed;
        for (i = 1; i < 15; i++) {
            m[i] = t[i] - 0xffff - ((m[i - 1] >> 16) & 1);
            m[i - 1] &= 0xffff;
        }
        m[15] = t[15] - 0x7fff - ((m[14] >> 16) & 1);
        carry = (m[15] >> 16) & 1;
        m[14] &= 0xffff;
        swap25519(t, m, 1 - carry);
    }
    for (i = 0; i < 16; i++) {
        out[2 * i] = t[i] & 0xff;
        out[2 * i + 1] = t[i] >> 8;
    }
}

static void fadd(elem out, const elem a, const elem b) {//adds together field elements a + b
    int i;
    for(i = 0; i < 16; i++) {
        out[i] = a[i] + b[i];
    }
}
static void fsub(elem out, const elem a, const elem b) {
    int i;
    for(i = 0; i < 16;i ++) {
        out[i] = a[i] - b[i];
    }
}
static void fmul(elem out, const elem a, const elem b) {
    uint64_t i,j,pr[31];
    for (i=0; i < 31; i++){
        pr[i] = 0;
    }
    for (i=0; i < 16; i++){
        for(j = 0; j < 16; j++){
            pr[ i + j] += a[i] * b[j];//changed
        }
    }
    for (i = 0; i < 15; i++) {//reducing 2^255 -19 *modulo*
        //this is derrived from montgomery form
        pr[i] += 38 * pr[i + 16];
    }
    for (i = 0; i < 16; i++) {
        out[i] = pr[i];
    }
    carry25519(out);//figure out overflow
    carry25519(out);//make sure no final overflow
}
static void finv(elem out, const elem input){
    //multiplicative inverse of elements on the X25519 curve.
    elem c;
    int i;
    for (i = 0; i < 16; i++) {
        c[i] = input[i];
    }
    for (i = 253; i >= 0; i--) {
        fmul(c, c, c);//c = c * c
        if (i != 2 && i != 4) {//X25519 optimization
            fmul(c, c, input);//c = c * input
        }
    }
    for (i = 0; i < 16; i++) {
        out[i] = c[i];
    }
}
void scalar_mult(uint8_t* out, const uint8_t* scalar, const uint8_t * input){//by voodoo magic performs scalar mutliplication
    uint8_t clamped[32];
    int64_t i, bit;
    elem a,b,c,d,e,f,x;
    for (i = 0; i < 32; i++){
        clamped[i] = scalar[i];
    }
    clamped[0] = clamped[0] & 0xf8; //0x11111000
    clamped[31] = (clamped[31] & 0x7f) | 0x40; //0x11101111 | 0x01000000
    unpack25519(x, input);
    for (i = 0; i < 16; i++) {
        b[i] = x[i];
        d[i] = a[i] = c[i] = 0;
    }
    a[0] = d[0] = 1;
    for ( i = 254; i >= 0 ; i--) {
        bit  = (clamped[i >> 3] >> (i & 7)) & 1; //boi what the helllll??? 
        swap25519(a, b, bit);
        swap25519(c, d, bit);
        fadd(e, a, c);
        fsub(a, a, c);
        fadd(c, b, d);
        fsub(b, b, d);
        fmul(d, e, e);
        fmul(f, a, a);
        fmul(a,c,a);
        fmul(c,b,e);
        fadd(e,a,c);
        fsub(a,a,c);
        fmul(b,a,a);
        fsub(c,d,f);
        fmul(a,c,_121665);
        fadd(a,a,d);
        fmul(c,c,a);
        fmul(a,d,f);
        fmul(d,b,x);
        fmul(b,e,e);
        swap25519(a,b,bit);
        swap25519(c,d,bit);
    }
    finv(c, c);
    fmul(a,a,c);
    pack25519(out, a);
    return;
}
void printkey(unsigned char *buf)
{
  for (int i = 0;i < 32;++i) {
    if (i > 0) printf(","); else printf(" ");
    printf("0x%02x",(unsigned int) buf[i]);
    if (i % 8 == 7) printf("\n");
  }
}
void printlongkey(unsigned char *buf)
{
  for (int i = 0;i < 96;++i) {
    if (i > 0) printf(","); else printf(" ");
    printf("0x%02x",(unsigned int) buf[i]);
    if (i % 8 == 7) printf("\n");
  }
}
