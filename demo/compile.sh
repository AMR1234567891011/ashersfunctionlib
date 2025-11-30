#!/bin/bash

echo "=== Compiling Crypto Libraries ==="

# Compile WASM
echo "Building WebAssembly..."
emcc -O3 -s WASM=1 \
  -s EXPORTED_FUNCTIONS="[_x3dh_woR,_x3dh_woS,_main_tests,_wasm_init_sessions,_wasm_new_session,_wasm_send_message,_wasm_receive_message,_malloc,_free]" \
  -s EXPORTED_RUNTIME_METHODS="[ccall,cwrap]" \
  -o crypto_wasm.js \
  ../main_test.c ../sha256.c ../X25519.c ../X3DH.c ../session_manager.c ../session_test.c ../wasm_interface.c

echo "WASM built: crypto_wasm.js"

# Compile shared library
echo "Building shared library..."
gcc -shared -fPIC -o libcrypto.so \
  ../session_manager.c ../sha256.c ../X25519.c ../X3DH.c \
  -I..

echo "Shared library built: libcrypto.so"

echo "=== Build complete ==="