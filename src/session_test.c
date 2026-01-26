// session_test.c
#include "../include/session_manager.h"
#include <stdio.h>
#include <string.h>

void session_demo_test() {
    printf("\n=== SESSION DEMO TEST ===\n");
    SessionManager sm;
    session_manager_init(&sm);
    unsigned char shared_secret[32];
    for(int i = 0; i < 32; i++) {
        shared_secret[i] = i + 0x80;
    }
    
    // Create identities
    unsigned char alice_identity[32] = {0x01}; // Alice's identity
    unsigned char bob_identity[32] = {0x02};   // Bob's identity
    
    printf("Creating sessions...\n");
    
    // Alice creates session with Bob
    int alice_session = session_manager_create_session(&sm, shared_secret, bob_identity, );
    printf("Alice session ID: %d\n", alice_session);
    
    // Bob creates session with Alice  
    int bob_session = session_manager_create_session(&sm, shared_secret, alice_identity, );
    printf("Bob session ID: %d\n", bob_session);
    
    printf("\n--- Testing basic encryption/decryption ---\n");
    
    // Simple test without ratchet - just use the same chain key
    const char *test_msg = "Test message";
    unsigned char ciphertext[256];
    unsigned char decrypted[256];
    unsigned char ratchet_key_out[256];
    // Alice encrypts
    session_send_message(&sm, alice_session, (unsigned char*)test_msg, strlen(test_msg), ciphertext);
    printf("Original: '%s'\n", test_msg);
    printf("Ciphertext: ");
    for(int i = 0; i < strlen(test_msg); i++) printf("%02x", ciphertext[i]);
    printf("\n");
    
    // Bob decrypts (no ratchet step)
    session_receive_message(&sm, bob_session, ciphertext, strlen(test_msg), decrypted);
    decrypted[strlen(test_msg)] = '\0';
    printf("Decrypted: '%s'\n", decrypted);
    
    if(memcmp(test_msg, decrypted, strlen(test_msg)) == 0) {
        printf("Basic encryption/decryption works!\n");
    } else {
        printf("Basic encryption/decryption failed!\n");
    }
    
    printf("\nSession demo completed!\n");
}