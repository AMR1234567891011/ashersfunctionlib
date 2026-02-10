#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../include/ed25519.h"
elem G_Y = {46603310, 25264513, 32001971, 4068508, 54135510, 4838230, 24653603, 19057531, 33554431, 33554431};
elem d = {
    -10913610, 13857413, -15372611, 6949391, 114729, -8787816, -6275908, -3247719, -18696448, -12055116
};
void point_zero(point *p) {
    fe_zero((elem*)p->X);
    fe_zero((elem*)p->Y);
    fe_zero((elem*)p->Z);
    fe_zero((elem*)p->T);
}
static void carry25519(elem h) {
    int i;
    for (i = 0; i < 9; ++i) {
        h[i+1] += h[i] >> 26;
        h[i] &= 0x3FFFFFF;//mask 26 bits
    }
    h[0] += (h[9] >> 25) * 19;
    h[9] &= 0x1FFFFFF;//mask 25 bits or 21 btis? not sure here
}
static void fe_sub(elem out, elem *a, elem *b) {
    for(int i =0;i<10;i++) out[i] = (int64_t)((*a)[i] - (*b)[i]);
}
static void fe_add(elem out, elem *a, elem *b) {
    for(int i =0;i<10;i++) out[i] = (int64_t)((*a)[i] + (*b)[i]);
}
static void fmul(elem out, elem *a, elem *b) {
    int64_t t[19] = {0};//intermediate values
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            t[i+j] += (int64_t)(*a)[i] * (*b)[j];
        }
    }
    for (int i = 0; i < 9; i++) t[i] += t[i+10] * 19;//mult by 19 bc 2^i where i >= 255, is congruent to 19 mod(p) on the field
    
    memcpy(out, t, sizeof(int64_t) * 10);
    carry25519(out);
    carry25519(out); // Second pass to handle the wrap-around carry
}
void ed_point_add(point *out, point* p,  point* q) { 
    elem A, B, C, D, E, F, G, H;
    
    // A = (Y1-X1)*(Y2-X2)
    fe_sub(A, &p->Y, &p->X);
    fe_sub(B, &q->Y, &q->X);
    fmul(A, &A, &B);
    
    // B = (Y1+X1)*(Y2+X2)
    fe_add(B, &p->Y, &p->X);
    fe_add(C, &q->Y, &q->X);
    fmul(B, &B, &C);
    
    // C = T1*2*d*T2
    fmul(C, &p->T, &q->T);
    fe_add(C, &C, &C);
    fmul(C, &C, &d);
    
    // D = Z1*2*Z2
    fmul(D, &p->Z, &q->Z);
    fe_add(D, &D, &D);
    
    // E = B-A, F = D-C, G = D+C, H = B+A
    fe_sub(E, &B, &A);
    fe_sub(F, &D, &C);
    fe_add(G, &D, &C);
    fe_add(H, &B, &A);
    
    // Output
    fmul(out->X, &E, &F);
    fmul(out->Y, &G, &H);
    fmul(out->Z, &F, &G);
    fmul(out->T, &E, &H);    
}
//mod l not p
static void scalar_add();
static void scalar_mul();
static void scalar_reduce();

static void point_mult(point *out, const point* a, const point* b) {
    //mult needs 512 bit result,
    //use q as quotient and return a*b - ql where l is the 25519 
}
// Adjusted print function for 26-bit limbs
void print_elem(const char* label, elem e) {
    uint8_t bytes[32];
    // This is a simplified hex export for 26-bit limbs
    printf("%s: ", label);
    // For debugging, let's just print the raw limbs to see if they change
    for (int i = 9; i >= 0; i--) printf("%07llx ", (long long)e[i]);
    printf("\n");
}
// int main() {
//     elem a, b, out;

//     // Test 1: Identity Multiplication (G_Y * 1)
//     // Initialize 'a' as G_Y and 'b' as 1
//     memcpy(a, G_Y, sizeof(elem));
//     memset(b, 0, sizeof(elem));
//     b[0] = 1; 

//     fmul(out, a, b);

//     printf("Test 1: G_Y * 1\n");
//     print_elem("Expected", G_Y);
//     print_elem("Result  ", out);

//     // Test 2: Squaring a value (G_Y * G_Y)
//     fmul(out, &G_Y, &G_Y);
//     printf("\nTest 2: G_Y^2\n");
//     print_elem("Result  ", out);

//     // Test 3: Carry check (Large value reduction)
//     // Set a value slightly larger than 2^255-19
//     // 2^255 is roughly bit 25 in the 10th limb (index 9)
//     memset(a, 0, sizeof(elem));
//     a[9] = (1LL << 25); // This represents 2^255
    
//     // carry25519 should convert 2^255 into 19 (since 2^255 mod p = 19)
//     carry25519(a);
//     printf("\nTest 3: Modular Reduction (2^255 mod p)\n");
//     printf("Expected: ...0000000 0000013\n");
//     print_elem("Result  ", a);

//     return 0;
// }