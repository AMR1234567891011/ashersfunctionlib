#!/bin/bash

echo "=== Compiling Crypto Libraries ==="

# Compile WASM for JS
echo "Building WebAssembly with Memory Exports..."

emcc -O3 -s WASM=1 \
  -s EXPORTED_FUNCTIONS="[_x3dh_woR,_x3dh_woS,_main_tests,_wasm_init_sessions,_wasm_new_session,_wasm_send_message,_wasm_receive_message,_malloc,_free]" \
  -s **INITIAL_MEMORY**=67108864 \
  -s **ALLOW_MEMORY_GROWTH**=1 \
  -s **EXTRA_EXPORTED_RUNTIME_METHODS**="['cwrap']" \
  -o crypto_wasm.js \
  ../main_test.c ../sha256.c ../X25519.c ../X3DH.c ../session_manager.c ../session_test.c ../wasm_interface.c

echo "WASM built: crypto_wasm.js"

# Compile shared library for python
echo "Building shared library..."
gcc -shared -fPIC -o libsignal.so \
  ../session_manager.c ../sha256.c ../X25519.c ../X3DH.c \
  -I..

echo "Shared library built: libsignal.so"

mkdir static
mv crypto_wasm.js static/
mv crypto_wasm.wasm static/

echo "=== Build complete ==="