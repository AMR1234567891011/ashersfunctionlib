#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include "sha256.h"
#include "X25519.h"
#include "X3DH.h"
#include "double_ratchet.h"

#define MAX_SESSIONS 10
#define MAX_MESSAGE_LEN 1024

typedef struct {
    unsigned char remote_identity[32];
    uint8_t active;
    Double_Ratchet ratchet;
} Session;

typedef struct {
    Session sessions[MAX_SESSIONS];
    uint8_t session_count;
} SessionManager;
void randombytes(unsigned char *out, size_t len);
int session_manager_init(SessionManager *sm);
int session_manager_find_session(SessionManager *sm, const unsigned char *remote_id);
int session_manager_accept_session(SessionManager *sm, const unsigned char *shared_secret, const unsigned char *remote_identity, const unsigned char *dh_public, const unsigned char *prekey_private);
int session_manager_create_session(SessionManager *sm ,const unsigned char *shared_secret, const unsigned char *remote_identity, unsigned char *prekey_public);
int session_send_message(SessionManager *sm, const unsigned char *remote_identity, const unsigned char *plaintext, uint32_t len, unsigned char *ciphertext);
int session_receive_message(SessionManager *sm, const unsigned char *remote_identity, const unsigned char *ciphertext, uint32_t len, unsigned char *plaintext);

#endif