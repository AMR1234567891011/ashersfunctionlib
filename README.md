update the readme with info on new demo and note to expose the 8000 port when running container and to run the following within container, cd demo && chmod +x compile.sh && ./compile.sh && python3 server.py # Cryptographic Protocol Implementation

## Overview
Complete cryptographic suite implementing SHA-256, HMAC, HKDF, X25519, and X3DH key agreement protocol. All algorithms implemented from scratch for educational purposes.

## Test Suite

### 1. SHA-256 Hash Test
**Purpose**: Validates SHA-256 implementation with long text input.

**Test Input**: "Monster Mash" song lyrics (277 characters)

**Output**: 32-byte cryptographic hash

**Verification**: Consistent hashing of large inputs

### 2. HMAC-SHA256 Test  
**Purpose**: Tests message authentication with both text and binary inputs.

**Test Cases**:
- Text: "Monster Mash" lyrics with key "ITS THE MONSTER MASH"
- Binary: 3-byte message with 32-byte binary key

**Output**: 32-byte HMAC digests

### 3. HKDF Test
**Purpose**: Tests key derivation function for cryptographic key generation.

**Procedure**:
- Extract: Derive PRK (Pseudo-Random Key) from input message
- Expand: Generate 64 bytes OKM (Output Keying Material) using PRK + info

**Algorithm**: HKDF-SHA256 following RFC 5869

### 4. X25519 Test
**Purpose**: Validates Elliptic Curve Diffie-Hellman over Curve25519.

**Test Vectors**: Standardized test pairs for Alice and Bob

**Operations**:
- Public key generation from private keys
- Shared secret computation
- Cross-verification between parties

**Verification**: Both parties compute identical shared secrets

### 5. X3DH Test
**Purpose**: Implements Extended Triple Diffie-Hellman key agreement protocol (used in Signal protocol).

**Key Types**:
- Identity Keys (IK): Long-term key pairs
- Signed Pre-Keys (SPK): Medium-term keys  
- Ephemeral Keys (EK): One-time use keys

**Protocol**: X3DH

**Security Properties**:
- Forward secrecy through ephemeral keys
- Identity authentication via long-term keys
- Key compromise protection through multiple DH combinations

## Algorithm Features

- **SHA-256**: NIST-standard cryptographic hash
- **HMAC-SHA256**: Message authentication code
- **HKDF-SHA256**: Key derivation function
- **X25519**: Elliptic curve cryptography
- **X3DH**: Multi-stage key agreement protocol

## Build & Run
```bash
gcc -o crypto_test main.c sha256.c X25519.c X3DH.c session_manager.c session_test.c
./crypto_test
```
