#include <stdio.h>
#include "../include/ed25519.h"
#include "../include/ed25519_tests.h"
static void print_group_point(point result) {
    printf("Result X: ");
    for (int i = 0; i < 10; i++) printf("%lld ", (long long)result.X[i]);
    printf("\n");
    
    printf("Result Y: ");
    for (int i = 0; i < 10; i++) printf("%lld ", (long long)result.Y[i]);
    printf("\n");
    
    printf("Result Z: ");
    for (int i = 0; i < 10; i++) printf("%lld ", (long long)result.Z[i]);
    printf("\n");
    
    printf("Result T: ");
    for (int i = 0; i < 10; i++) printf("%lld ", (long long)result.T[i]);
    printf("\n\n");
}
void test_ed_add_identity() {
    point X1, X2, result;
    fe_zero(&result.X);
    fe_zero(&result.Y);
    fe_zero(&result.Z);
    fe_zero(&result.T);
    fe_zero(&X1.X);
    fe_zero(&X2.X);
    fe_one(&X1.Y);
    fe_one(&X2.Y);
    fe_one(&X1.Z);
    fe_one(&X2.Z);
    fe_zero(&X1.T);
    fe_zero(&X2.T);
    ed_point_add(&result, &X1, &X2);
    print_group_point(result);
}
void test_ed_add_g() {
    
// G = (Gx, Gy, 1, Gx*Gy mod p)
// G2 = add(G, G)
// # Compare with known 2G from standard test vectors
// # (affine):
// # 2Gx = 24727413235106541002554574571675588834622768167397638456726423682521233608206
// # 2Gy = 15549675580280190176352668710449542251549572066445060580507079593062643049417
//affine to extended
//known_2G_proj = (known_2G_x, known_2G_y, 1, known_2G_x * known_2G_y mod p)
    elem G_Y = {46603310, 25264513, 32001971, 4068508, 54135510, 4838230, 24653603, 19057531, 33554431, 33554431};
    point calc_2G;
    // point_zero(&calc_2G);
    // ed_point_add(&calc_2G, (elem*)&G_Y, (elem*)&G_Y);


}