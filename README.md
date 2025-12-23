# Cryptographic Protocol Implementation

## Overview
Complete cryptographic suite implementing SHA-256, HMAC, HKDF, X25519, and X3DH key agreement protocol. All algorithms implemented from scratch for educational purposes. Includes a web-based demo application with Flask backend and WebAssembly frontend.

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

## Demo Application

A web-based demonstration showcasing the cryptographic protocols in action:

### Features:
- **Web Interface**: Terminal-style UI with green/blue themes
- **Real-time Messaging**: Encrypted message exchange between users
- **Session Management**: X3DH key agreement and session establishment
- **WASM Integration**: Cryptographic functions compiled to WebAssembly
- **REST API**: Flask backend with C library integration

### Quick Start:
```bash
# Build and run the demo application
cd demo && chmod +x compile.sh && ./compile.sh && python3 server.py
```
**Important**: When running in a container, expose port 8000 to access the web interface.

### Usage:
1. Access the web interface at `http://localhost:8000`
2. Register users with `register <username>` command
3. Establish sessions with `session <username>` (initiator) and `complete <username>` (responder)
4. Exchange encrypted messages with `send <username> <message>`
5. Retrieve messages with `getmsgs`

## Build & Run

### Standalone Tests:
```bash
gcc -o crypto_test main.c sha256.c X25519.c X3DH.c session_manager.c session_test.c
./crypto_test
```
### Demo Application:
*** Build by running: ***
```bash
docker build ./demo signal_demo
docker run -it --rm -p 8000:8000 signal_demo
```
*** Within container start webser by running: ***
```bash
chmod +x ./demo/compile.sh
./demo/compile.sh
python3 ./demo/server.py
```
### DEMO API FLOW with Alice and Bob
* Alice gets the webpage which generates identity key and signed pre-key, then alice inputs "register alice" to pass her username and associated keys to backend
* Bob does the same
* Alice initiates a session with inputing "session bob" Her frontend first fetches bobs prekey bundle that contains identity key and signed pre key. Then the start_session route is hit which adds the session to bobs session by adding alice's identity_key and an ephemral key used for the handshake. 
* Bob has no way of getting pending requests right now, but he somehow knows to input "complete alice". This first fetches the ephemral key from this prospective session, then gets alice's prekey bundle. 

# THIS IMPLEMENTATION IS ONLY FOR EDUCATION AND REALLY SUCKS FOR SECUTIRY 



