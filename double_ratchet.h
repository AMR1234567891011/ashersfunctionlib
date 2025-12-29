#ifndef DOUBLE_RATCHET_H
#define DOUBLE_RATCHET_H

typedef struct  {
    unsigned char root_key[32];//shared secret from the X3DH initial exchange
    unsigned char cks[32];//chain sending key
    unsigned char ckr[32];//chain key
    unsigned char dhs[32];//diffiehellman send
    unsigned char dhr[32];//diifie hellman recv
    uint32_t ns;//number or send messages
    uint32_t nr;//number or recieved messages
    uint32_t pn;//previous chain len
    //STATIC: maybe remove later
    unsigned char dh_priv[32];
    unsigned char prk[32];
    unsigned char kdf_out[64];
} Double_Ratchet;


void init_double_ratchet(Double_Ratchet *dr, unsigned char *prekey);
void comp_double_ratchet(Double_Ratchet *dr, unsigned char *dh_public, unsigned char *prekey_private);

#endif