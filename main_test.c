#include "stdio.h"
#include "sha256.h"
#include "X25519.h"
#include "X3DH.h"
#include "session_test.h"
#include "double_ratchet_test.h"
/* hash test msg
I was working in the lab late one night When my eyes beheld an eerie sight For my monster, from his slab, began to rise And suddenly, to my surprise (He did the Mash) he did the Monster Mash (The Monster Mash) it was a graveyard smash (He did the Mash) it caught on in a flash (He did the Mash) he did the Monster Mash From my laboratory in the Castle east To the master bedroom, where the vampires feast The ghouls all came from their humble abodes To get a jolt from my electrodes
*/
void hash_test(){
    const uint8_t *msg = "I was working in the lab late one night When my eyes beheld an eerie sight For my monster, from his slab, began to rise And suddenly, to my surprise (He did the Mash) he did the Monster Mash (The Monster Mash) it was a graveyard smash (He did the Mash) it caught on in a flash (He did the Mash) he did the Monster Mash From my laboratory in the Castle east To the master bedroom, where the vampires feast The ghouls all came from their humble abodes To get a jolt from my electrodes";
    //const uint8_t *msg = "THIS IS A TEST";

    uint32_t len = strlen(msg);
    uint8_t out[32] = {0x00};
    sha256(msg, len, out);

    printf("\nhash: ");
    for (int i = 0; i < 32; i++) {
        printf("%02x", out[i]);
    }
    printf("\n");
}
void hmac_test(){
    const uint8_t *msg = "I was working in the lab late one night When my eyes beheld an eerie sight For my monster, from his slab, began to rise And suddenly, to my surprise (He did the Mash) he did the Monster Mash (The Monster Mash) it was a graveyard smash (He did the Mash) it caught on in a flash (He did the Mash) he did the Monster Mash From my laboratory in the Castle east To the master bedroom, where the vampires feast The ghouls all came from their humble abodes To get a jolt from my electrodes";
    uint32_t msg_len = strlen(msg);
    uint8_t *key = "ITS THE MONSTER MASH";
    uint32_t key_len = strlen(key);
    uint8_t out[32] = {0x00};
    hmac_sha256(msg, key, key_len, msg_len, out);
    printf("\nhmac: ");
    for (int i = 0; i < 32; i++) {
        printf("%02x", out[i]);
    }
    uint8_t key2[32] = {0xc0, 0x69, 0xd0, 0x22, 0x98, 0x46,  0x3e, 0x3d,0x6e, 0x75, 0xa0, 0x53, 0x56, 0x2b, 0xbe, 0xf5, 0x94, 0x05, 0xe2, 0x03, 0xa6, 0x58, 0x0d, 0x9b, 0x8d, 0xd9, 0x10, 0x1e, 0xaa, 0x60, 0xfb, 0xfe };
    key_len = 32;
    uint8_t msg2[3] = {0xff, 0xff, 0x01};
    msg_len = 3;
    hmac_sha256(msg2, key2, key_len, msg_len, out);
    printf("\nhmac TEST: ");
    for (int i = 0; i < 32; i++) {
        printf("%02x", out[i]);
    }
    printf("\n");
}
void hkdf_test(){
    uint8_t *msg = "testing my hkdf function I hope it works.";
    uint32_t msg_length = strlen(msg);
    uint8_t PRK[32] = {0x00};
    hkdf_extract(msg, msg_length, PRK);//gets pseduo random key from message
    printf("PRK: \n");
    printkey(PRK);

    uint8_t info[2] = {0x00};
    info[0] = 0xFF;
    info[1] = 0xFF;
    uint32_t info_length = 2;
    uint8_t OKM[64] = {0x00};
    hkdf_expand(64, info, info_length, PRK, 32, OKM);//gets 64 bytes of ouput keying material
    printf("\nOKM: \n");
    printkey(OKM);
    printkey((OKM + 32));
}
void X25519_test(){
    unsigned char alicesk[32] = {//alice secret key
        0x77,0x07,0x6d,0x0a,0x73,0x18,0xa5,0x7d
       ,0x3c,0x16,0xc1,0x72,0x51,0xb2,0x66,0x45
       ,0xdf,0x4c,0x2f,0x87,0xeb,0xc0,0x99,0x2a
       ,0xb1,0x77,0xfb,0xa5,0x1d,0xb9,0x2c,0x2a
       };
       
    unsigned char alicepk[32] = {//alice public key
        0x85,0x20,0xf0,0x09,0x89,0x30,0xa7,0x54
       ,0x74,0x8b,0x7d,0xdc,0xb4,0x3e,0xf7,0x5a
       ,0x0d,0xbf,0x3a,0x0d,0x26,0x38,0x1a,0xf4
       ,0xeb,0xa4,0xa9,0x8e,0xaa,0x9b,0x4e,0x6a
       } ;
       
    unsigned char bobsk[32] = {//bobs secret key
        0x5d,0xab,0x08,0x7e,0x62,0x4a,0x8a,0x4b
       ,0x79,0xe1,0x7f,0x8b,0x83,0x80,0x0e,0xe6
       ,0x6f,0x3b,0xb1,0x29,0x26,0x18,0xb6,0xfd
       ,0x1c,0x2f,0x8b,0x27,0xff,0x88,0xe0,0xeb
       } ;
       
    unsigned char bobpk[32] = {//bobs public key
        0xde,0x9e,0xdb,0x7d,0x7b,0x7d,0xc1,0xb4
       ,0xd3,0x5b,0x61,0xc2,0xec,0xe4,0x35,0x37
       ,0x3f,0x83,0x43,0xc8,0x5b,0x78,0x67,0x4d
       ,0xad,0xfc,0x7e,0x14,0x6f,0x88,0x2b,0x4f
       } ;
    int i;
    unsigned char key[32];
    scalar_mult(key, alicesk, _9);
    printf("Alice public key: \n");
    printkey(key);
    printf("\n");

    scalar_mult(key, bobsk, _9);
    printf("Bob public key: \n");
    printkey(key);
    printf("\n");

    scalar_mult(key, alicesk, bobpk);
    printf("Alice shared secret: \n");
    printkey(key);
    printf("\n");

    scalar_mult(key, bobsk, alicepk);
    printf("Bob shared secret: \n");
    printkey(key);
}
void X3DH_test(){
    unsigned char alice_ik_sk[32] = { // Alice's Identity Key (Private)
        0x77, 0x07, 0x6d, 0x0a, 0x73, 0x18, 0xa5, 0x7d,
        0x3c, 0x16, 0xc1, 0x72, 0x51, 0xb2, 0x66, 0x45,
        0xdf, 0x4c, 0x2f, 0x87, 0xeb, 0xc0, 0x99, 0x2a,
        0xb1, 0x77, 0xfb, 0xa5, 0x1d, 0xb9, 0x2c, 0x2a
    };
    unsigned char alice_ik_pk[32] = {0x00};//Alice's Ident key (Public)
    scalar_mult(alice_ik_pk, alice_ik_sk, _9);

    unsigned char bob_ik_sk[32] = { // Bob's Identity Key (Private)
        0x5d, 0xab, 0x08, 0x7e, 0x62, 0x4a, 0x8a, 0x4b,
        0x79, 0xe1, 0x7f, 0x8b, 0x83, 0x80, 0x0e, 0xe6,
        0x6f, 0x3b, 0xb1, 0x29, 0x26, 0x18, 0xb6, 0xfd,
        0x1c, 0x2f, 0x8b, 0x27, 0xff, 0x88, 0xe0, 0xeb
    };
    unsigned char bob_ik_pk[32] = {0x00};
    scalar_mult(bob_ik_pk, bob_ik_sk, _9);

    unsigned char bob_spk_sk[32] = { // Bob's Signed Pre-Key (Private)
        0x4a, 0x5d, 0x9d, 0x5b, 0xa4, 0xce, 0x2d, 0xe1,
        0x72, 0x8e, 0x3b, 0xf4, 0x80, 0x35, 0x0f, 0x25,
        0xe0, 0x7e, 0x21, 0xc9, 0x47, 0xd1, 0x9e, 0x33,
        0x76, 0xf0, 0x9b, 0x3c, 0x1e, 0x16, 0x17, 0x42
    };
    unsigned char bob_spk_pk[32] = {0x00};
    scalar_mult(bob_spk_pk, bob_spk_sk, _9);

    unsigned char alice_ek_sk[32] = { // Alice's Ephemeral Key (Private)
        0x24, 0xb2, 0x7b, 0x25, 0x40, 0x9d, 0xeb, 0x30,
        0x0b, 0x1d, 0xfb, 0xf9, 0x55, 0xeb, 0x75, 0x75,
        0xe0, 0x39, 0xff, 0x1e, 0xb1, 0xf5, 0xe7, 0x8c,
        0xb6, 0xc6, 0xb6, 0x49, 0x33, 0xc8, 0x7f, 0x7a
    };
    unsigned char alice_ek_pk[32] = {0x00};
    scalar_mult(alice_ek_pk, alice_ek_sk, _9);

    //ALICE FIRST COMPUTES
    unsigned char alice_sk[32] = {0x00};
    unsigned char tempA[96] = {0x00};

    
    x3dh_woS(alice_sk, alice_ik_sk, bob_ik_pk, alice_ek_sk, bob_spk_pk);

    //BOB THEN COMPUTES
    unsigned char bob_sk[32] = {0x00};
    unsigned char tempB[96] = {0x00};
    x3dh_woR(bob_sk, alice_ik_pk, bob_ik_sk, alice_ek_pk, bob_spk_sk);

    printf("\nALICE:\n");
    printkey(alice_sk);
    printf("\nBOB:\n");
    printkey(bob_sk);
    
}
void main_tests() {
    printf("\nHASH TEST\n");
    hash_test();

    printf("\nHMAC TEST\n");
    hmac_test();

    printf("\nHKDF TEST\n");
    hkdf_test();
    
    printf("\nECC TEST\n");
    X25519_test();
    
    printf("\nX3DH TEST\n");
    X3DH_test();

    //session_demo_test();

    printf("\nDOUBLE RATCHET TEST");
    double_ratchet_init_test();
}
int main(){
    main_tests();
    return 0;
}