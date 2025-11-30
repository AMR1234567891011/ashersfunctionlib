#include "session_manager.h"
#include <stdio.h>
#include <string.h>

// Simple random bytes function for demo
static void randombytes(unsigned char *out, size_t len) {
    for(size_t i = 0; i < len; i++) {
        out[i] = (i * 17 + 53) % 256;
    }
}

// Simple XOR encryption for demo
static void simple_crypt(unsigned char *output, const unsigned char *input, uint32_t len, const unsigned char *key) {
    unsigned char stream[MAX_MESSAGE_LEN];
    unsigned char info[1] = {0x00};
    
    hkdf_expand(len, info, 1, (uint8_t*)key, 32, stream);
    for(uint32_t i = 0; i < len; i++) {
        output[i] = input[i] ^ stream[i];
    }
}

static void session_peek_message_key(Session *session, unsigned char *message_key) {
    unsigned char info[1] = {0x03};
    hkdf_expand(32, info, 1, session->chain_key, 32, message_key);
}

static void session_advance_chain(Session *session) {
    unsigned char new_chain[32];
    unsigned char info_chain[1] = {0x04};
    hkdf_expand(32, info_chain, 1, session->chain_key, 32, new_chain);
    memcpy(session->chain_key, new_chain, 32);
    session->message_count++;
}

int session_manager_init(SessionManager *sm) {
    sm->session_count = 0;
    for(int i = 0; i < MAX_SESSIONS; i++) {
        sm->sessions[i].active = 0;
    }
    return 0;
}

int session_manager_create_session(SessionManager *sm, const unsigned char *shared_secret, const unsigned char *remote_identity) {
    if(sm->session_count >= MAX_SESSIONS) return -1;
    int slot = -1;
    for(int i = 0; i < MAX_SESSIONS; i++) {
        if(!sm->sessions[i].active) {
            slot = i;
            break;
        }
    }
    if(slot == -1) return -1;
    
    Session *session = &sm->sessions[slot];
    unsigned char salt[32] = {0};
    unsigned char info[13] = "SignalRatchet";
    
    hkdf_extract((uint8_t*)shared_secret, 32, session->root_key);
    hkdf_expand(32, info, 13, session->root_key, 32, session->chain_key);
    
    randombytes(session->ratchet_priv, 32);
    scalar_mult(session->ratchet_pub, session->ratchet_priv, _9);
    
    memcpy(session->remote_identity, remote_identity, 32);
    session->message_count = 0;
    session->active = 1;
    sm->session_count++;
    
    return slot;
}

int session_manager_find_session(SessionManager *sm, const unsigned char *remote_identity) {
    for(int i = 0; i < MAX_SESSIONS; i++) {
        if(sm->sessions[i].active && memcmp(sm->sessions[i].remote_identity, remote_identity, 32) == 0) {
            return i;
        }
    }
    return -1;
}

void session_perform_ratchet(Session *session, const unsigned char *new_remote_pub) {
    memcpy(session->remote_pub, new_remote_pub, 32);
    
    unsigned char dh_output[32];
    scalar_mult(dh_output, session->ratchet_priv, new_remote_pub);
    
    unsigned char input[64];
    for(int i = 0; i < 32; i++) {
        input[i] = session->root_key[i];
        input[i+32] = dh_output[i];
    }
    
    unsigned char new_root[32], new_chain[32];
    unsigned char info_root[1] = {0x01};
    unsigned char info_chain[1] = {0x02};
    
    hkdf_expand(32, info_root, 1, input, 64, new_root);
    hkdf_expand(32, info_chain, 1, input, 64, new_chain);
    
    memcpy(session->root_key, new_root, 32);
    memcpy(session->chain_key, new_chain, 32);
    
    randombytes(session->ratchet_priv, 32);
    scalar_mult(session->ratchet_pub, session->ratchet_priv, _9);
    session->message_count = 0;
}

int session_send_message(SessionManager *sm, int session_id, const unsigned char *plaintext, uint32_t len, unsigned char *ciphertext) {
    if(session_id < 0 || session_id >= MAX_SESSIONS || !sm->sessions[session_id].active) return -1;
    if(len > MAX_MESSAGE_LEN) return -1;
    Session *session = &sm->sessions[session_id];
    unsigned char message_key[32];
    session_peek_message_key(session, message_key);
    simple_crypt(ciphertext, plaintext, len, message_key);
    session_advance_chain(session);
    
    return 0;
}

int session_receive_message(SessionManager *sm, int session_id, const unsigned char *ciphertext, uint32_t len, unsigned char *plaintext, const unsigned char *remote_ratchet_pub) {
    if(session_id < 0 || session_id >= MAX_SESSIONS || !sm->sessions[session_id].active) return -1;
    if(len > MAX_MESSAGE_LEN) return -1;
    
    Session *session = &sm->sessions[session_id];
    
    if(remote_ratchet_pub && memcmp(remote_ratchet_pub, session->remote_pub, 32) != 0) {
        session_perform_ratchet(session, remote_ratchet_pub);
    }
    
    unsigned char message_key[32];
    session_peek_message_key(session, message_key);
    simple_crypt(plaintext, ciphertext, len, message_key);
    session_advance_chain(session);
    
    return 0;
}