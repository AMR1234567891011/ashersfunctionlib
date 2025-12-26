#ifndef DOUBLE_RATCHET_H
#define DOUBLE_RATCHET_H

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
    unsigned char dh_priv[32];
    unsigned char prk[32];
} Double_Ratchet;


int init_double_ratchet();
//intialize DH keypair
int start_double_ratchet();
//initialize send and recv ratchet steps
//Takes remote public key and root key from X3DH
int advance_dh_chain();
//generate new DH keypair after remote_pub has changed
//generate send and receive chain keys
int advance_send_chain();
//run current send chain key through KDF
int advance_recv_chain();
//run current receive chain key through KDF

#endif