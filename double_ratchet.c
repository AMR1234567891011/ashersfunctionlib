#include <stdint.h>
#include "double_ratchet.h"
#include "session_manager.h"
#include "X25519.h"
#include "sha256.h"

uint8_t info = 0xAA;
void clear_double_ratchet(Double_Ratchet *dr) {
    for(int i = 0; i < 32; i++) {
        dr->dhs[i] = 0x00;
        dr->dhr[i] = 0x00;
        dr->ckr[i] = 0x00;
        dr->cks[i] = 0x00;
    }
    dr->ns = 0;
    dr->nr = 0;
}
void init_double_ratchet(Double_Ratchet *dr, unsigned char *prekey_public) {//initiator -> generates key pair adn computes its sending chain
    randombytes(dr->dhr, 32);
    scalar_mult(dr->dhs, dr->dhr, _9);//public point to send
    scalar_mult(dr->dh_priv, dr->dhr, prekey_public);//do DH on private and remote public *note dhr is used to store intermediate state*
    hkdf_extract_salt(dr->dh_priv, 32, dr->root_key, 32, dr->prk);//generate keying material
    hkdf_expand(64, &info, 1, dr->prk, 32, dr->kdf_out);//write 64 to fill rootkey and sending chain assuming no struct padding
    for(int i =  0; i <32; i++) {
        dr->root_key[i] = dr->kdf_out[i];
        dr->cks[i] = dr->kdf_out[i + 32];
    }
}
void resp_double_ratchet(Double_Ratchet *dr, unsigned char *dh_public, unsigned char *prekey_private) {//responder -> receives inititors public key so computes it receving chain and sending chain.
    scalar_mult(dr->dh_priv, prekey_private, dh_public);
    hkdf_extract_salt(dr->dh_priv, 32, dr->root_key, 32, dr->prk);//generate keying material
    hkdf_expand(64, &info, 1, dr->prk, 32, dr->kdf_out);
        for(int i =  0; i <32; i++) {
        dr->root_key[i] = dr->kdf_out[i];
        dr->ckr[i] = dr->kdf_out[i + 32];
    }
}
void send_message(Double_Ratchet *dr, unsigned char *message, uint32_t message_len) {
    
}

void receive_message(Double_Ratchet *dr, unsigned char *buffer, uint32_t *message_len) {

}

