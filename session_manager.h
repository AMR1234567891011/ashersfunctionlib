// session_manager.h
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

// Session management
int session_manager_init(SessionManager *sm);
int session_manager_create_session(SessionManager *sm, const unsigned char *shared_secret, const unsigned char *remote_identity);
int session_manager_find_session(SessionManager *sm, const unsigned char *remote_identity);

// Messaging
int session_send_message(SessionManager *sm, int session_id, const unsigned char *plaintext, uint32_t len, unsigned char *ciphertext);
int session_receive_message(SessionManager *sm, int session_id, const unsigned char *ciphertext, uint32_t len, unsigned char *plaintext, const unsigned char *remote_ratchet_pub);

// Ratchet operations
void session_perform_ratchet(Session *session, const unsigned char *new_remote_pub);
void session_get_message_key(Session *session, unsigned char *message_key);

#endif