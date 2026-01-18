#include <stdio.h>
#include <string.h>
#include "double_ratchet.h"
#include "cbc.h"
#include "session_manager.h"
// Simple random bytes function for demo
void randombytes(unsigned char *out, size_t len) {
    for(size_t i = 0; i < len; i++) {
        out[i] = (i * 17 + 53) % 256;
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
        if((memcmp(sm->sessions[i].remote_identity, remote_identity, 32) == 0)) {
            return i;
        }
    }
    return -1;
}
static int sm_get_session_idx(SessionManager *sm) {
    if(sm->session_count >= MAX_SESSIONS) return -1;
    int slot = -1;
    for(int i = 0; i < MAX_SESSIONS; i++) {
        if(sm->sessions[i].active == 0) {
            slot = i;
            break;
        }
    }
    return slot;
}
int session_manager_create_session(
    SessionManager *sm, 
    const unsigned char *shared_secret, 
    const unsigned char *remote_identity, 
    unsigned char *prekey_public) {
    //initiator
    int idx = session_manager_find_session(sm, remote_identity);
    if(idx < 0) {
        idx = sm_get_session_idx(sm);
        if(idx < 0) return -1;
    }
    
    Session *session = &sm->sessions[idx];
    if(session->active == 0) {
        for(int i = 0; i < 32; i++) {
            session->remote_identity[i] = remote_identity[i];
            session->ratchet.root_key[i] = shared_secret[i];
        }
        session->active = 1;
    }
    printf("\nSession idx: %d remote_id\n", idx);
    init_double_ratchet(&session->ratchet, prekey_public);
    printf("\nSend Key:\n");
    for(int i = 0; i< 32; i++) {
        printf("%02x", session->ratchet.cks[i]);
    }
    return 1;
}
int session_manager_accept_session(
    SessionManager *sm, 
    const unsigned char *shared_secret,  
    const unsigned char *remote_identity, 
    const unsigned char *dh_public, 
    const unsigned char *prekey_private) {
    //responder
    int idx = session_manager_find_session(sm, remote_identity);
    if(idx < 0) {
        idx = sm_get_session_idx(sm);
        if(idx  < 0) return -1;
    }
    Session *session = &sm->sessions[idx];
    if(session->active == 0) {
        for(int i = 0; i < 32; i++) {
            session->remote_identity[i] = remote_identity[i];
            session->ratchet.root_key[i] = shared_secret[i];
        }
        session->active = 1;
    }
    resp_double_ratchet(&session->ratchet, dh_public, prekey_private);
    printf("\nRecv Key:\n");
    for(int i = 0; i< 32; i++) {
        printf("%02x", session->ratchet.ckr[i]);
    }
    return 1;
}

