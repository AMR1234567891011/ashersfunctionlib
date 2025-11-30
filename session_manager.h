#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include "sha256.h"
#include "X25519.h"
#include "X3DH.h"

#define MAX_SESSIONS 10
#define MAX_MESSAGE_LEN 1024

typedef struct {
    unsigned char root_key[32];
    unsigned char chain_key[32];
    unsigned char ratchet_priv[32];
    unsigned char ratchet_pub[32];
    unsigned char remote_pub[32];
    unsigned char remote_identity[32];
    uint32_t message_count;
    uint8_t active;
} Session;

typedef struct {
    Session sessions[MAX_SESSIONS];
    uint8_t session_count;
} SessionManager;

int session_manager_init(SessionManager *sm);
int session_manager_create_session(SessionManager *sm, const unsigned char *shared_secret, const unsigned char *remote_identity);
int session_send_message(SessionManager *sm, int session_id, const unsigned char *plaintext, uint32_t len, unsigned char *ciphertext);
int session_receive_message(SessionManager *sm, int session_id, const unsigned char *ciphertext, uint32_t len, unsigned char *plaintext, const unsigned char *remote_ratchet_pub);

#endif