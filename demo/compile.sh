#!/bin/bash
emcc -O3 -sWASM=1 \
  -s'EXPORTED_FUNCTIONS=[_x3dh_woR,_x3dh_woS,_main_tests,_wasm_init_sessions,_wasm_new_session,_wasm_send_message,_wasm_receive_message,_malloc,_free]' \
  -s'INITIAL_MEMORY=67108864' \
  -s'ALLOW_MEMORY_GROWTH=1' \
  -s'EXPORTED_RUNTIME_METHODS=["cwrap"]' \
  -g4 -s ASSERTIONS=2 \
  -o crypto_wasm.js \
  ../main_test.c ../sha256.c ../X25519.c ../X3DH.c ../session_manager.c ../session_test.c ../wasm_interface.c

mkdir static
mv crypto_wasm.js static
mv crypto_wasm.wasm static