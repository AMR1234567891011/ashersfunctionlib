cmake_minimum_required(VERSION 3.10)
project(DoubleRatchetTests C)

set(CMAKE_C_STANDARD 99)

set(SOURCES
    src/aes.c
    src/cbc.c
    src/double_ratchet_test.c
    src/double_ratchet.c
    src/main_test.c
    src/session_manager.c
    src/session_manager_test.c
    src/sha256.c
    src/X3DH.c
    src/X25519.c
)

add_executable(tests ${SOURCES})

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    target_compile_options(tests PRIVATE -march=x86-64)
endif()