#include "wasm_interface.h"
#include "session_manager.h"
#include <string.h>

#ifdef __EMSCRIPTEN__

// Global session manager for WASM
SessionManager global_sm;

EMSCRIPTEN_KEEPALIVE
int wasm_init_sessions() {
    return session_manager_init(&global_sm);
}

EMSCRIPTEN_KEEPALIVE
int wasm_new_session(const unsigned char *shared_secret, const unsigned char *remote_identity) {
    return session_manager_create_session(&global_sm, shared_secret, remote_identity);
}

EMSCRIPTEN_KEEPALIVE
int wasm_send_message(int session_idx, const unsigned char *plaintext, uint32_t len, unsigned char *ciphertext) {
    return session_send_message(&global_sm, session_idx, plaintext, len, ciphertext);
}

EMSCRIPTEN_KEEPALIVE
int wasm_receive_message(int session_idx, const unsigned char *ciphertext, uint32_t len, unsigned char *plaintext, const unsigned char *remote_ratchet_pub) { 
    return session_receive_message(&global_sm, session_idx, ciphertext, len, plaintext, remote_ratchet_pub);
}

EMSCRIPTEN_KEEPALIVE
void wasm_run_all_tests() {
    run_all_tests();
}

#endif