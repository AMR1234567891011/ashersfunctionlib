#ifndef X3DH_H
#define X3DH_H

#include "main_test.h"
#include "X25519.h"
#include "sha256.h"
//REPLACE SHA256 WITH HKDF FUNCTION!!!

void x3dh_w(key sk, key IKA, key IKB, key EKA, key SPKB, key OPKB);//implement later with one time pre keys,
void x3dh_woS(key sk, key IKA, key IKB, key EKA, key SPKB);//send/initialize first
void x3dh_woR(key sk, key IKA, key IKB, key EKA, key SPKB);//receive/verify second


#endif