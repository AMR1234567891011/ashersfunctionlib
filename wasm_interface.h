// wasm_interface.h
#ifndef WASM_INTERFACE_H
#define WASM_INTERFACE_H

#include "session_manager.h"

// WASM-specific exports for JS
#ifdef __EMSCRIPTEN__
#include <emscripten.h>

// Initialize session system for WASM
EMSCRIPTEN_KEEPALIVE
int wasm_init_sessions();

EMSCRIPTEN_KEEPALIVE  
int wasm_new_session(const unsigned char *shared_secret, const unsigned char *remote_identity);

EMSCRIPTEN_KEEPALIVE
int wasm_send_message(int session_idx, const unsigned char *plaintext, uint32_t len, unsigned char *ciphertext);

EMSCRIPTEN_KEEPALIVE
int wasm_receive_message(int session_idx, const unsigned char *ciphertext, uint32_t len, unsigned char *plaintext, const unsigned char *remote_ratchet_pub);

#else
//native comp
#define wasm_init_sessions() 0
#define wasm_new_session(a, b) -1
#define wasm_send_message(a, b, c, d) -1
#define wasm_receive_message(a, b, c, d, e) -1
#endif

#endif
