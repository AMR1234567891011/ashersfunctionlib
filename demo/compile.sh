#!/bin/bash

emcc -O3 \
  -sWASM=1 \
  -sEXPORTED_FUNCTIONS='[_x3dh_woR,_x3dh_woS,_main_tests, scalar_mult, _wasm_init_sessions,_wasm_new_session,_wasm_send_message,_wasm_receive_message,_malloc,_free]' \
  -sINITIAL_MEMORY=67108864 \
  -sALLOW_MEMORY_GROWTH=1 \
  -sEXPORTED_RUNTIME_METHODS='["cwrap", "HEAPU8", "HEAP8", "UTF8ToString", "stringToUTF8", "lengthBytesUTF8"]' \
  -g \
  -sASSERTIONS=2 \
  -sERROR_ON_UNDEFINED_SYMBOLS=0 \
  -o static/crypto_wasm.js \
  ../main_test.c ../sha256.c ../X25519.c ../X3DH.c ../session_manager.c ../session_test.c ../wasm_interface.c
