#include <stdio.h>
#include "main_test.h"
#include "X25519.h"
#include "sha256.h"

void x3dh_w(key sk, key IKA, key IKB, key EKA, key SPKB, key OPKB)
{
    uint8_t temp[128] = {0x00};
    scalar_mult(temp, IKA, SPKB);
    scalar_mult((temp + 32), EKA, IKB);
    scalar_mult((temp + 64), EKA, SPKB);
    scalar_mult((temp + 96), EKA, OPKB);
    sha256(temp, 128, sk);
}

void x3dh_woS(key sk, key IKA, key IKB, key EKA, key SPKB)//Alice first
{
    uint8_t temp[96] = {0x00};
    scalar_mult(temp, IKA, SPKB);
    scalar_mult((temp + 32), EKA, IKB);
    scalar_mult((temp + 64), EKA, SPKB);
    sha256(temp, 96, sk);
}

void x3dh_woR(key sk, key IKA, key IKB, key EKA, key SPKB)//Bob second
{
    uint8_t temp[96] = {0x00};
    scalar_mult(temp, SPKB, IKA);
    scalar_mult((temp + 32), IKB, EKA);
    scalar_mult((temp + 64), SPKB, EKA);
    sha256(temp, 96, sk);
}


/*
IKA: ALICE Identity Key
IKB: BOB Identity Key
EKA: ALICE EPHEMRAL Key
SPKB: BOB signed pre Key
OPKB: BOB One time Keys

**NOTE**
All keys from Bob are supplied by the server and there is a case where
the server does not send Bobs prekey. These two cases split the execution
of x3dh into two paths

1: without Bob one time key
DH1 = DH(IKA, SPKB)
DH2 = DH(EKA, IKB)
DH3 = DH(EKA, SPKB)
Secret Key = KDF(DH1 || DH2 || DH3)

2: with Bob one time key

*/