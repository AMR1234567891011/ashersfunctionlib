#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include "sha256.h"
#include "X25519.h"
#include "X3DH.h"
#include "double_ratchet.h"

#define MAX_SESSIONS 10
#define MAX_MESSAGE_LEN 1024

typedef struct {
    unsigned char root_key[32];
    unsigned char remote_identity[32];
    uint32_t message_count;
    uint8_t active;
    Double_Ratchet ratchet;
} Session;

typedef struct {
    Session sessions[MAX_SESSIONS];
    uint8_t session_count;
} SessionManager;
void randombytes(unsigned char *out, size_t len);
int session_manager_init(SessionManager *sm);
int session_manager_create_session(SessionManager *sm, const unsigned char *shared_secret, const unsigned char *remote_identity);
int session_send_message(SessionManager *sm, int session_id, const unsigned char *plaintext, uint32_t len, unsigned char *ciphertext, unsigned char *new_ratchet_pub_out);
int session_receive_message(SessionManager *sm, int session_id, const unsigned char *ciphertext, uint32_t len, unsigned char *plaintext, const unsigned char *remote_ratchet_pub);

#endif