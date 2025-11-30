#AI generated
from flask import Flask, jsonify, request, send_file
from flask_cors import CORS
import ctypes
import secrets
from ctypes import c_uint8, c_uint32, POINTER, Structure, c_int
import json

app = Flask(__name__)
CORS(app)

# Load your C library
lib = ctypes.CDLL('./libsignal.so')

# Define structures matching your C code
class SessionManager(Structure):
    _fields_ = [
        ("sessions", c_uint8 * 2560),  # 10 sessions * 256 bytes each
        ("session_count", c_uint8)
    ]

# Setup function signatures for your existing C functions
lib.session_manager_init.argtypes = [POINTER(SessionManager)]
lib.session_manager_init.restype = c_int

lib.session_manager_create_session.argtypes = [POINTER(SessionManager), POINTER(c_uint8), POINTER(c_uint8)]
lib.session_manager_create_session.restype = c_int

lib.session_send_message.argtypes = [POINTER(SessionManager), c_int, POINTER(c_uint8), c_uint32, POINTER(c_uint8)]
lib.session_send_message.restype = c_int

lib.session_receive_message.argtypes = [POINTER(SessionManager), c_int, POINTER(c_uint8), c_uint32, POINTER(c_uint8), POINTER(c_uint8)]
lib.session_receive_message.restype = c_int

# X3DH functions (from your X3DH.h)
lib.x3dh_woS.argtypes = [POINTER(c_uint8), POINTER(c_uint8), POINTER(c_uint8), POINTER(c_uint8), POINTER(c_uint8)]
lib.x3dh_woS.restype = None

lib.x3dh_woR.argtypes = [POINTER(c_uint8), POINTER(c_uint8), POINTER(c_uint8), POINTER(c_uint8), POINTER(c_uint8)]
lib.x3dh_woR.restype = None

# X25519 functions
lib.scalar_mult.argtypes = [POINTER(c_uint8), POINTER(c_uint8), POINTER(c_uint8)]
lib.scalar_mult.restype = None

# Global session manager
global_sm = SessionManager()
lib.session_manager_init(ctypes.byref(global_sm))

# Store user data
users = {}  # {username: {identity_private, identity_public, prekey_private, prekey_public}}
sessions = {}  # {username: session_index}
pending_messages = []  # Store encrypted messages

@app.route('/')
def home():
    return send_file("index.html")

@app.route('/register/<username>', methods=['POST'])
def register_user(username):
    """Register user with identity key, signed prekey, AND ephemeral key"""
    if username in users:
        return jsonify({'error': 'User already exists'}), 400
    
    # Generate identity key pair
    identity_private = (c_uint8 * 32)()
    identity_public = (c_uint8 * 32)()
    
    for i in range(32):
        identity_private[i] = secrets.randbits(8)
    
    base_point = (c_uint8 * 32)(*[9] + [0]*31)
    lib.scalar_mult(identity_public, identity_private, base_point)
    
    # Generate signed prekey pair
    prekey_private = (c_uint8 * 32)()
    prekey_public = (c_uint8 * 32)()
    
    for i in range(32):
        prekey_private[i] = secrets.randbits(8)
    
    lib.scalar_mult(prekey_public, prekey_private, base_point)
    
    # Generate ephemeral key pair
    ephemeral_private = (c_uint8 * 32)()
    ephemeral_public = (c_uint8 * 32)()
    
    for i in range(32):
        ephemeral_private[i] = secrets.randbits(8)
    
    lib.scalar_mult(ephemeral_public, ephemeral_private, base_point)
    
    users[username] = {
        'identity_private': list(identity_private),
        'identity_public': list(identity_public),
        'prekey_private': list(prekey_private),
        'prekey_public': list(prekey_public),
        'ephemeral_private': list(ephemeral_private),
        'ephemeral_public': list(ephemeral_public)
    }
    
    return jsonify({
        'identity_public': list(identity_public),
        'prekey_public': list(prekey_public),
        'ephemeral_public': list(ephemeral_public),
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
    """Start session with another user using stored ephemeral keys"""
    data = request.json
    from_user = data['from']
    to_user = data['to']
    
    if from_user not in users or to_user not in users:
        return jsonify({'error': 'User not found'}), 404
    
    from_user_data = users[from_user]
    to_user_data = users[to_user]
    
    # Generate AND STORE ephemeral key for this user if not exists
    if 'ephemeral_private' not in from_user_data:
        ephemeral_private = (c_uint8 * 32)()
        ephemeral_public = (c_uint8 * 32)()
        
        for i in range(32):
            ephemeral_private[i] = secrets.randbits(8)
        
        base_point = (c_uint8 * 32)(*[9] + [0]*31)
        lib.scalar_mult(ephemeral_public, ephemeral_private, base_point)
        
        # Store the ephemeral keys
        from_user_data['ephemeral_private'] = list(ephemeral_private)
        from_user_data['ephemeral_public'] = list(ephemeral_public)
    
    # Use stored ephemeral keys
    ephemeral_private = (c_uint8 * 32)(*from_user_data['ephemeral_private'])
    
    # Perform X3DH using STORED ephemeral key
    shared_secret = (c_uint8 * 32)()
    
    from_identity_private = (c_uint8 * 32)(*from_user_data['identity_private'])
    to_identity_public = (c_uint8 * 32)(*to_user_data['identity_public'])
    to_prekey_public = (c_uint8 * 32)(*to_user_data['prekey_public'])
    
    lib.x3dh_woS(shared_secret, from_identity_private, to_identity_public, 
                 ephemeral_private, to_prekey_public)
    
    # Create session
    session_idx = lib.session_manager_create_session(
        ctypes.byref(global_sm),
        shared_secret,
        (c_uint8 * 32)(*to_user_data['identity_public'])
    )
    
    if session_idx >= 0:
        sessions[from_user] = session_idx
        
        return jsonify({
            'session_index': session_idx,
            'status': 'session_established',
            'with_user': to_user,
            'ephemeral_public': from_user_data['ephemeral_public']  # Return for debugging
        })
    else:
        return jsonify({'error': 'session_creation_failed'}), 400
@app.route('/debug')
def debug():
    """Debug endpoint to see what's happening"""
    debug_info = {
        'users': list(users.keys()),
        'sessions': sessions,
        'user_keys': {user: {
            'identity_public': users[user]['identity_public'][:4],  # First 4 bytes
            'has_ephemeral': 'ephemeral_public' in users[user]
        } for user in users},
        'pending_messages_count': len(pending_messages)
    }
    return jsonify(debug_info)

@app.route('/send_message', methods=['POST'])
def send_message():
    """Send encrypted message using session"""
    data = request.json
    username = data['from']
    to_username = data['to']
    message = data['message']
    
    if username not in sessions:
        return jsonify({'error': 'No active session'}), 400
    
    session_idx = sessions[username]
    
    # Encrypt message using your C session manager
    plaintext = message.encode('utf-8')
    plaintext_buf = (c_uint8 * len(plaintext))(*plaintext)
    ciphertext_buf = (c_uint8 * len(plaintext))()
    
    result = lib.session_send_message(
        ctypes.byref(global_sm),
        session_idx,
        plaintext_buf,
        len(plaintext),
        ciphertext_buf
    )
    
    if result == 0:
        ciphertext = list(ciphertext_buf)
        pending_messages.append({
            'to': to_username,
            'from': username,
            'ciphertext': ciphertext,
            'length': len(plaintext)
        })
        
        return jsonify({
            'status': 'sent',
            'ciphertext': ciphertext
        })
    else:
        return jsonify({'error': 'send_failed'}), 400

@app.route('/get_messages/<username>')
def get_messages(username):
    """Get pending messages for user"""
    user_messages = [msg for msg in pending_messages if msg['to'] == username]
    
    # Decrypt messages using session
    decrypted_messages = []
    for msg in user_messages:
        if username in sessions:
            session_idx = sessions[username]
            ciphertext_buf = (c_uint8 * msg['length'])(*msg['ciphertext'])
            plaintext_buf = (c_uint8 * msg['length'])()
            
            result = lib.session_receive_message(
                ctypes.byref(global_sm),
                session_idx,
                ciphertext_buf,
                msg['length'],
                plaintext_buf,
                None
            )
            
            if result == 0:
                plaintext = bytes(plaintext_buf).decode('utf-8', errors='ignore')
                decrypted_messages.append({
                    'from': msg['from'],
                    'message': plaintext
                })
    
    # Remove delivered messages
    pending_messages[:] = [msg for msg in pending_messages if msg['to'] != username]
    
    return jsonify(decrypted_messages)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8000, debug=True)