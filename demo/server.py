#now uses E2EE 
from flask import Flask, jsonify, request, send_file
from flask_cors import CORS
import secrets
import json

app = Flask(__name__)
CORS(app)

users = {}  # {username: {ik_pub, spk_pub, KEY_BUNDLE_...: {ephemeral_public, initiator_id_public}}} # NEW: Updated comment to reflect current usage
pending_messages = []  # Store encrypted messages
KEY_SIZE = 32

@app.route('/')
def home():
    return send_file("index.html")

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
        'identity_public': user['ik_pub'],
        'signed_prekey_public': user['spk_pub'] 
    })
@app.route('/register/<username>', methods=['POST'])
def register_user(username):
    if username in users:
        return jsonify({'error': 'User already exists'}), 400
    data = request.json

    ik_pub = data.get('identity_public')
    spk_pub = data.get('prekey_public')
    
    if not ik_pub or not spk_pub or len(ik_pub) != KEY_SIZE or len(spk_pub) != KEY_SIZE:
        return jsonify({'error': 'Invalid key data provided (requires 32-byte IK and SPK public keys)'}), 400
    
    users[username] = {
        'ik_pub': ik_pub,
        'spk_pub': spk_pub
    }
    return jsonify({
        'status': 'registered',
        'identity_public': ik_pub,
        'prekey_public': spk_pub
    })

@app.route('/start_session', methods=['POST'])
def start_session():
    data = request.json
    from_user = data.get('from') #init
    to_user = data.get('to')
    eph_pub = data.get('ephemeral_public') 
    if from_user not in users or to_user not in users:
        return jsonify({'error': 'User not found'}), 404
    
    # if not eph_pub or len(eph_pub) != KEY_SIZE:
    #     return jsonify({'error': 'Invalid Ephemeral Public Key provided'}), 400
    
    session_key_idx = f"KEY_BUNDLE_{from_user}_{to_user}"
    
    if to_user not in users:
        users[to_user] = {'ik_pub': [], 'spk_pub': []} 
        
    users[to_user][session_key_idx] = {
        'eph_pub': eph_pub,
        'ik_pub': users[from_user]['ik_pub']
    }
    
    return jsonify({
        'status': 'ephemeral_key_posted',
        'recipient': to_user
    })

@app.route('/fetch_ephemeral/<responder_user>/<initiator_user>')
def fetch_ephemeral(responder_user, initiator_user):
    if responder_user not in users or initiator_user not in users:
        return jsonify({'error': 'User not found'}), 404

    session_key_index = f"KEY_BUNDLE_{initiator_user}_{responder_user}" 
    
    if session_key_index not in users[responder_user]:
        return jsonify({'error': 'No pending session initiation found from this user'}), 404
    
    bundle = users[responder_user].pop(session_key_index) 
    return jsonify({
        'status': 'session_bundle_retrieved',
        'initiator_ik_public': bundle['ik_pub'],
        'ephemeral_public': bundle['eph_pub'],
        'message_type': 'SESSION_INIT'
    })

@app.route('/send_message', methods=['POST'])
def send_message():
    data = request.json
    username = data.get('from')
    to_username = data.get('to')
    required = ['ciphertext', 'length', 'ratchet_pub']
    
    if not all(key in data for key in required):
        return jsonify({'error': 'message is missing argument (ciphertext, length, ratchet_pub)'}), 400
    
    if to_username not in users:
        return jsonify({'error': 'target is not registered'}), 400
    
    encrypted_message = {
        'to': to_username,
        'from': username, 
        'ciphertext': data['ciphertext'],
        'length': data['length'],
        'ratchet_pub': data['ratchet_pub']
    }
    pending_messages.append(encrypted_message)
    
    return jsonify({
        'status': 'sent to target',
        'message_size': encrypted_message['length']
    })

@app.route('/get_messages/<username>')
def get_messages(username):
    user_messages = [msg for msg in pending_messages if msg['to'] == username]
    
    messages_to_deliver = []
    for msg in user_messages:
        messages_to_deliver.append({
            'from': msg['from'],
            'ciphertext': msg['ciphertext'],
            'length': msg['length'],
            'ratchet_pub_key': msg.get('ratchet_pub') 
        })
    
    pending_messages[:] = [msg for msg in pending_messages if msg['to'] != username]
    
    return jsonify(messages_to_deliver)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8000, debug=True)