#ifndef DOUBLE_RATCHET_H
#define DOUBLE_RATCHET_H
#include <stdint.h>
typedef struct {
    unsigned char root_key[32];//shared secret from the X3DH initial exchange
    unsigned char cks[32];//chain sending key
    unsigned char ckr[32];//chain key
    unsigned char dhs[32];//diffiehellman send
    unsigned char dhr[32];//diifie hellman recv
    uint32_t ns;//number or send messages
    uint32_t nr;//number or recieved messages
    uint32_t pn;//previous chain len
    //STATIC: maybe remove later
    unsigned char dh_pub[32];
    unsigned char dh_priv[32];
    unsigned char prk[32];
    unsigned char kdf_out[64];
} Double_Ratchet;
typedef struct {
    uint32_t pn;
    uint32_t n;
    uint32_t ciphertext_len;
    uint8_t pub_key[32];
    uint8_t IV[16];
    uint8_t ciphertext[256];
    uint8_t tag[32];
} Message;

void init_double_ratchet(Double_Ratchet *dr, unsigned char *prekey_public);
void resp_double_ratchet(Double_Ratchet *dr, unsigned char *dh_public, unsigned char *prekey_private);
void send_message_dr(Double_Ratchet *dr, unsigned char *message, uint32_t message_len, Message *msg);
void recv_message_dr(Double_Ratchet *dr, unsigned char *ciphertext, uint32_t ciphertext_len);
#endif