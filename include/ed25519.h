#ifndef ED_25519_H
#define ED_25519_H
#include <stdint.h>
typedef int64_t elem[10]; 
struct {
    elem X;
    elem Y;
    elem Z;
    elem T;
}typedef point;

static void fe_one(elem *e) {
    (*e)[0] = 1;
    for(int i = 1;i<10;i++) (*e)[i] = 0;
}
static void fe_zero(elem *e) {
    for(int i = 0;i<10;i++) (*e)[i] = 0;
}
void point_zero(point *p);
void ed_point_add(point *out, point* p, point* q);
void print_elem(const char* label, elem e);
void compress(char *out, point* p);
void decompress(point* out, char *in);
#endif