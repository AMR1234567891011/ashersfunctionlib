#AI generated
from flask import Flask, jsonify, request, send_file
from flask_cors import CORS
import ctypes
import secrets
from ctypes import c_uint8, c_uint32, POINTER, Structure, c_int
import json

app = Flask(__name__)
CORS(app)

lib = ctypes.CDLL('./libsignal.so')

class SessionManager(Structure):
    _fields_ = [
        ("sessions", c_uint8 * 2560),  # 10 sessions * 256 bytes each
        ("session_count", c_uint8)
    ]

lib.session_manager_init.argtypes = [POINTER(SessionManager)]
lib.session_manager_init.restype = c_int

lib.session_manager_create_session.argtypes = [POINTER(SessionManager), POINTER(c_uint8), POINTER(c_uint8)]
lib.session_manager_create_session.restype = c_int

lib.session_send_message.argtypes = [POINTER(SessionManager), c_int, POINTER(c_uint8), c_uint32, POINTER(c_uint8)]
lib.session_send_message.restype = c_int

lib.session_receive_message.argtypes = [POINTER(SessionManager), c_int, POINTER(c_uint8), c_uint32, POINTER(c_uint8), POINTER(c_uint8)]
lib.session_receive_message.restype = c_int
lib.x3dh_woS.argtypes = [POINTER(c_uint8), POINTER(c_uint8), POINTER(c_uint8), POINTER(c_uint8), POINTER(c_uint8)]
lib.x3dh_woS.restype = None

lib.x3dh_woR.argtypes = [POINTER(c_uint8), POINTER(c_uint8), POINTER(c_uint8), POINTER(c_uint8), POINTER(c_uint8)]
lib.x3dh_woR.restype = None
lib.scalar_mult.argtypes = [POINTER(c_uint8), POINTER(c_uint8), POINTER(c_uint8)]
lib.scalar_mult.restype = None
global_sm = SessionManager()
lib.session_manager_init(ctypes.byref(global_sm))
users = {}  # {username: {identity_private, identity_public, prekey_private, prekey_public}}
sessions = {}  # {username: session_index}
pending_messages = []  # Store encrypted messages

@app.route('/')
def home():
    return send_file("index.html")

@app.route('/register/<username>', methods=['POST'])
def register_user(username):
    """Register user with identity key and signed prekey"""
    if username in users:
        return jsonify({'error': 'User already exists'}), 400
    
    identity_private = (c_uint8 * 32)()
    identity_public = (c_uint8 * 32)()
    
    # Generate random private key, then compute public key
    for i in range(32):
        identity_private[i] = secrets.randbits(8)
    base_point = (c_uint8 * 32)(*[9] + [0]*31)  #BASE POINT IS 9
    lib.scalar_mult(identity_public, identity_private, base_point)
    
    # Generate signed prekey pair
    prekey_private = (c_uint8 * 32)()
    prekey_public = (c_uint8 * 32)()
    
    for i in range(32):
        prekey_private[i] = secrets.randbits(8)
    
    lib.scalar_mult(prekey_public, prekey_private, base_point)
    
    users[username] = {
        'identity_private': list(identity_private),
        'identity_public': list(identity_public),
        'prekey_private': list(prekey_private),
        'prekey_public': list(prekey_public)
    }
    
    return jsonify({
        'identity_public': list(identity_public),
        'prekey_public': list(prekey_public),
        'status': 'registered'
    })

@app.route('/users')
def get_users():
    return jsonify(list(users.keys()))

@app.route('/prekey_bundle/<username>')
def get_prekey_bundle(username):
    """Get user's prekey bundle for X3DH"""
    if username not in users:
        return jsonify({'error': 'User not found'}), 404
    
    user = users[username]
    return jsonify({
        'identity_public': user['identity_public'],
        'signed_prekey_public': user['prekey_public']
    })
@app.route('/start_session', methods=['POST'])
def start_session():
    """Start session - handles both initiator and responder with proper X3DH"""
    data = request.json
    from_user = data['from']
    to_user = data['to']
    
    if from_user not in users or to_user not in users:
        return jsonify({'error': 'User not found'}), 404
    
    from_user_data = users[from_user]
    to_user_data = users[to_user]
    
    session_key = f"{from_user}-{to_user}"
    
    ephemeral_private = (c_uint8 * 32)()
    ephemeral_public = (c_uint8 * 32)()
    
    for i in range(32):
        ephemeral_private[i] = secrets.randbits(8)
    
    base_point = (c_uint8 * 32)(*[9] + [0]*31)
    lib.scalar_mult(ephemeral_public, ephemeral_private, base_point)
    
    if 'ephemeral_keys' not in from_user_data:
        from_user_data['ephemeral_keys'] = {}
    from_user_data['ephemeral_keys'][session_key] = {
        'private': list(ephemeral_private),
        'public': list(ephemeral_public)
    }
    
    # x3dh_woS(shared_secret, initiator_ik_priv, responder_ik_pub, initiator_ek_priv, responder_spk_pub)
    shared_secret = (c_uint8 * 32)()
    lib.x3dh_woS(shared_secret,
                (c_uint8 * 32)(*from_user_data['identity_private']),
                (c_uint8 * 32)(*to_user_data['identity_public']),
                ephemeral_private,
                (c_uint8 * 32)(*to_user_data['prekey_public']))
    
    session_idx = lib.session_manager_create_session(
        ctypes.byref(global_sm),
        shared_secret,
        (c_uint8 * 32)(*to_user_data['identity_public'])
    )
    
    if session_idx >= 0:
        sessions[from_user] = {
            'session_index': session_idx,
            'with_user': to_user,
            'shared_secret': list(shared_secret),
            'role': 'initiator'
        }
        
        responder_session_idx = lib.session_manager_create_session(
            ctypes.byref(global_sm),
            shared_secret,
            (c_uint8 * 32)(*from_user_data['identity_public'])
        )
        
        if responder_session_idx >= 0:
            sessions[to_user] = {
                'session_index': responder_session_idx,
                'with_user': from_user,
                'shared_secret': list(shared_secret),
                'role': 'responder',
                'ephemeral_public': list(ephemeral_public) 
            }
        
        return jsonify({
            'session_index': session_idx,
            'status': 'session_established',
            'with_user': to_user,
            'role': 'initiator'
        })
    else:
        return jsonify({'error': 'session_creation_failed'}), 400
@app.route('/complete_session', methods=['POST'])
def complete_session():
    """Responder completes session setup using initiator's ephemeral key"""
    data = request.json
    from_user = data['from']  # responder
    to_user = data['to']      # initiator
    
    if from_user not in users or to_user not in users:
        return jsonify({'error': 'User not found'}), 404
    
    from_user_data = users[from_user]
    to_user_data = users[to_user]
    
    session_key = f"{to_user}-{from_user}"  # initiator-responder
    
    # Check if initiator has stored ephemeral key
    if ('ephemeral_keys' not in to_user_data or 
        session_key not in to_user_data['ephemeral_keys']):
        return jsonify({'error': 'Initiator has no ephemeral key'}), 400
    
    ephemeral_public = to_user_data['ephemeral_keys'][session_key]['public']
    
    # x3dh_woR(shared_secret, initiator_ik_pub, responder_ik_priv, initiator_ek_pub, responder_spk_priv)
    shared_secret = (c_uint8 * 32)()
    lib.x3dh_woR(shared_secret,
                (c_uint8 * 32)(*to_user_data['identity_public']),
                (c_uint8 * 32)(*from_user_data['identity_private']),
                (c_uint8 * 32)(*ephemeral_public),
                (c_uint8 * 32)(*from_user_data['prekey_private']))
    
    # Create session for responder
    session_idx = lib.session_manager_create_session(
        ctypes.byref(global_sm),
        shared_secret,
        (c_uint8 * 32)(*to_user_data['identity_public'])
    )
    
    if session_idx >= 0:
        sessions[from_user] = {
            'session_index': session_idx,
            'with_user': to_user,
            'shared_secret': list(shared_secret),
            'role': 'responder'
        }
        return jsonify({
            'session_index': session_idx,
            'status': 'session_established',
            'with_user': to_user,
            'role': 'responder'
        })
    else:
        return jsonify({'error': 'session_creation_failed'}), 400

@app.route('/send_message', methods=['POST'])
def send_message():
    """Send encrypted message using session"""
    data = request.json
    username = data['from']
    to_username = data['to']
    message = data['message']
    
    if username not in sessions:
        return jsonify({'error': 'No active session'}), 400
    
    session_info = sessions[username]
    session_idx = session_info['session_index']
    
    plaintext = message.encode('utf-8')
    plaintext_buf = (c_uint8 * len(plaintext))(*plaintext)
    ciphertext_buf = (c_uint8 * (len(plaintext) + 64))()
    
    result = lib.session_send_message(
        ctypes.byref(global_sm),
        ctypes.c_int(session_idx), 
        plaintext_buf,
        ctypes.c_uint32(len(plaintext)),
        ciphertext_buf
    )
    
    if result == 0:
        # Get actual ciphertext length 
        ciphertext = list(ciphertext_buf)[:len(plaintext)] 
        pending_messages.append({
            'to': to_username,
            'from': username,
            'ciphertext': ciphertext,
            'length': len(plaintext) 
        })
        
        return jsonify({
            'status': 'sent',
            'ciphertext_length': len(ciphertext)
        })
    else:
        return jsonify({'error': f'send_failed with code {result}'}), 400

@app.route('/get_messages/<username>')
def get_messages(username):
    """Get pending messages for user"""
    user_messages = [msg for msg in pending_messages if msg['to'] == username]
    
    # Decrypt messages using session
    decrypted_messages = []
    for msg in user_messages:
        if username in sessions:
            session_info = sessions[username]
            session_idx = session_info['session_index']
            
            ciphertext_buf = (c_uint8 * msg['length'])(*msg['ciphertext'])
            plaintext_buf = (c_uint8 * msg['length'])()
            
            result = lib.session_receive_message(
                ctypes.byref(global_sm),
                ctypes.c_int(session_idx),  
                ciphertext_buf,
                ctypes.c_uint32(msg['length']), 
                plaintext_buf,
                None
            )
            
            if result == 0:
                plaintext = bytes(plaintext_buf).decode('utf-8', errors='ignore').rstrip('\x00')
                decrypted_messages.append({
                    'from': msg['from'],
                    'message': plaintext
                })
            else:
                print(f"Decryption failed with code: {result}")
    
    pending_messages[:] = [msg for msg in pending_messages if msg['to'] != username]
    
    return jsonify(decrypted_messages)
def verify_crypto_functions():
    """Verify C crypto functions work correctly before serving"""
    print("=== VERIFYING CRYPTO FUNCTIONS ===")
    
    alice_ik_sk = [0x77, 0x07, 0x6d, 0x0a, 0x73, 0x18, 0xa5, 0x7d,
                   0x3c, 0x16, 0xc1, 0x72, 0x51, 0xb2, 0x66, 0x45,
                   0xdf, 0x4c, 0x2f, 0x87, 0xeb, 0xc0, 0x99, 0x2a,
                   0xb1, 0x77, 0xfb, 0xa5, 0x1d, 0xb9, 0x2c, 0x2a]
    
    bob_ik_sk = [0x5d, 0xab, 0x08, 0x7e, 0x62, 0x4a, 0x8a, 0x4b,
                 0x79, 0xe1, 0x7f, 0x8b, 0x83, 0x80, 0x0e, 0xe6,
                 0x6f, 0x3b, 0xb1, 0x29, 0x26, 0x18, 0xb6, 0xfd,
                 0x1c, 0x2f, 0x8b, 0x27, 0xff, 0x88, 0xe0, 0xeb]
    
    bob_spk_sk = [0x4a, 0x5d, 0x9d, 0x5b, 0xa4, 0xce, 0x2d, 0xe1,
                  0x72, 0x8e, 0x3b, 0xf4, 0x80, 0x35, 0x0f, 0x25,
                  0xe0, 0x7e, 0x21, 0xc9, 0x47, 0xd1, 0x9e, 0x33,
                  0x76, 0xf0, 0x9b, 0x3c, 0x1e, 0x16, 0x17, 0x42]
    
    alice_ek_sk = [0x24, 0xb2, 0x7b, 0x25, 0x40, 0x9d, 0xeb, 0x30,
                   0x0b, 0x1d, 0xfb, 0xf9, 0x55, 0xeb, 0x75, 0x75,
                   0xe0, 0x39, 0xff, 0x1e, 0xb1, 0xf5, 0xe7, 0x8c,
                   0xb6, 0xc6, 0xb6, 0x49, 0x33, 0xc8, 0x7f, 0x7a]

    # Compute public keys
    alice_ik_pk = (c_uint8 * 32)()
    bob_ik_pk = (c_uint8 * 32)()
    bob_spk_pk = (c_uint8 * 32)()
    alice_ek_pk = (c_uint8 * 32)()
    
    base_point = (c_uint8 * 32)(*[9] + [0]*31)
    
    lib.scalar_mult(alice_ik_pk, (c_uint8 * 32)(*alice_ik_sk), base_point)
    lib.scalar_mult(bob_ik_pk, (c_uint8 * 32)(*bob_ik_sk), base_point)
    lib.scalar_mult(bob_spk_pk, (c_uint8 * 32)(*bob_spk_sk), base_point)
    lib.scalar_mult(alice_ek_pk, (c_uint8 * 32)(*alice_ek_sk), base_point)

    # ALICE computes (x3dh_woS)
    alice_sk = (c_uint8 * 32)()
    lib.x3dh_woS(alice_sk,
                (c_uint8 * 32)(*alice_ik_sk),
                bob_ik_pk,
                (c_uint8 * 32)(*alice_ek_sk),
                bob_spk_pk)

    # BOB computes (x3dh_woR)
    bob_sk = (c_uint8 * 32)()
    lib.x3dh_woR(bob_sk,
                alice_ik_pk,
                (c_uint8 * 32)(*bob_ik_sk),
                alice_ek_pk,
                (c_uint8 * 32)(*bob_spk_sk))

    # Convert results to lists for comparison
    alice_secret = list(alice_sk)
    bob_secret = list(bob_sk)

    # Test if they match
    secrets_match = alice_secret == bob_secret
    
    print(f"X3DH Secrets Match: {secrets_match}")
    print(f"Alice Secret: {bytes(alice_secret).hex()[:16]}...")
    print(f"Bob Secret:   {bytes(bob_secret).hex()[:16]}...")

    if not secrets_match:
        print("X3DH FAILED - Secrets don't match!")
        return False

    # Test session creation and messaging
    alice_session = lib.session_manager_create_session(
        ctypes.byref(global_sm),
        (c_uint8 * 32)(*alice_secret),
        bob_ik_pk
    )
    
    bob_session = lib.session_manager_create_session(
        ctypes.byref(global_sm),
        (c_uint8 * 32)(*bob_secret),
        alice_ik_pk
    )
    
    session_test_passed = alice_session >= 0 and bob_session >= 0
    print(f"Session Creation: {session_test_passed} (Alice:{alice_session}, Bob:{bob_session})")
    
    if not session_test_passed:
        print("Session Creation FAILED!")
        return False

    # Test encryption/decryption
    test_message = "Hello from X3DH test!"
    plaintext = test_message.encode('utf-8')
    ciphertext = (c_uint8 * len(plaintext))()
    decrypted = (c_uint8 * len(plaintext))()
    
    # Alice encrypts
    encrypt_result = lib.session_send_message(
        ctypes.byref(global_sm),
        alice_session,
        (c_uint8 * len(plaintext))(*plaintext),
        len(plaintext),
        ciphertext
    )
    
    # Bob decrypts
    decrypt_result = lib.session_receive_message(
        ctypes.byref(global_sm),
        bob_session,
        ciphertext,
        len(plaintext),
        decrypted,
        None
    )
    
    decrypted_text = bytes(decrypted).decode('utf-8', errors='ignore')
    message_test_passed = (encrypt_result == 0 and 
                          decrypt_result == 0 and 
                          decrypted_text == test_message)
    
    print(f"Encryption/Decryption: {message_test_passed}")
    print(f"Original: '{test_message}'")
    print(f"Decrypted: '{decrypted_text}'")
    
    if not message_test_passed:
        print("Encryption/Decryption FAILED!")
        return False

    print("ALL CRYPTO TESTS PASSED!")
    return True

if __name__ == '__main__':
    lib.session_manager_init(ctypes.byref(global_sm))
    
    if not verify_crypto_functions():
        print("FATAL: Crypto verification failed. Exiting.")
        exit(1)
    
    app.run(host='0.0.0.0', port=8000, debug=True)