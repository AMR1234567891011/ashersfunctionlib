#include "double_ratchet.h"
#include "session_manager.h"
#include "X25519.h"
#include "sha256.h"
int init_double_ratchet(Double_Ratchet *dr, unsigned char dh_public) {
    for(int i = 0; i < 32; i++) {
        dr->dhs[i] = 0x00;
        dr->ckr[i] = 0x00;
        dr->cks[i] = 0x00;
    }
    dr->ns = 0;
    dr->nr = 0;
    randombytes(dr->dhs, 32);
    scalar_mult(dh_public, dr->dhs, _9);//public point
    return 0;
}
int start_double_ratchet(Double_Ratchet *dr, unsigned char *remote_public) {
    scalar_mult(dr->dh_priv, remote_public, dr->dhr);
    hkdf_extract_salt(dr->dh_priv, 32, dr->root_key, 32, dr->prk);
    hkdf_expand(64, 0xAA, 1, dr->dh_priv, 32, dr->root_key);//expand the key to root key and send chain
    return 0;
}