#include "session_manager.h"
#include <stdio.h>
#include <string.h>

// Simple random bytes function for demo
void randombytes(unsigned char *out, size_t len) {
    for(size_t i = 0; i < len; i++) {
        out[i] = (i * 17 + 53) % 256;
    }
}

// Simple XOR encryption for demo
// shit is terrible
//TODO: replace with AES 
static void simple_crypt(unsigned char *output, const unsigned char *input, uint32_t len, const unsigned char *key) {
    unsigned char stream[MAX_MESSAGE_LEN];
    unsigned char info[1] = {0x00};
    
    hkdf_expand(len, info, 1, (uint8_t*)key, 32, stream);
    for(uint32_t i = 0; i < len; i++) {
        output[i] = input[i] ^ stream[i];
    }
}
int session_manager_init(SessionManager *sm) {
    sm->session_count = 0;
    for(int i = 0; i < MAX_SESSIONS; i++) {
        sm->sessions[i].active = 0;
    }
    return 0;
}
int session_manager_find_session(SessionManager *sm, const unsigned char *remote_identity) {
    for(int i = 0; i < MAX_SESSIONS; i++) {
        if(sm->sessions[i].active && memcmp(sm->sessions[i].remote_identity, remote_identity, 32) == 0) {
            return i;
        }
    }
    return -1;
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

}


//TODO: replace
int session_send_message(SessionManager *sm, int session_id, const unsigned char *plaintext, uint32_t len, unsigned char *ciphertext, unsigned char *new_ratchet_pub_out) {
    if(session_id < 0 || session_id >= MAX_SESSIONS || !sm->sessions[session_id].active) return -1;
    if(len > MAX_MESSAGE_LEN) return -1;
    Session *session = &sm->sessions[session_id];

    return len;
}
//TODO: replace
int session_receive_message(SessionManager *sm, int session_id, const unsigned char *ciphertext, uint32_t len, unsigned char *plaintext, const unsigned char *remote_ratchet_pub) {
    if(session_id < 0 || session_id >= MAX_SESSIONS || !sm->sessions[session_id].active) return -1;
    if(len > MAX_MESSAGE_LEN) return -1;
    
    Session *session = &sm->sessions[session_id];
    
    return 0;
}