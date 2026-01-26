#include <stdint.h>
#include "../include/X25519.h"
#include "../include/session_manager.h"
#include "../include/double_ratchet.h"
// 32-byte root key
const unsigned char ROOT_KEY[32] = {
        0xd4, 0xc7, 0xe9, 0xa9, 0xfe, 0x3b, 0x0b, 0x2b,
        0x9c, 0x1f, 0x63, 0x48, 0xd5, 0xe7, 0xa1, 0x0f,
        0xb8, 0xf5, 0x49, 0xc7, 0xe6, 0x75, 0x4a, 0x8c,
        0x1d, 0xe2, 0xf0, 0xa9, 0xbb, 0xd3, 0xe5, 0xf2
    };
unsigned char ALICE_PRIV[32] = {
        0x77, 0x07, 0x6d, 0x0a, 0x73, 0x18, 0xa5, 0x7d,
        0x3c, 0x16, 0xc1, 0x72, 0x51, 0xb2, 0x66, 0x45,
        0xdf, 0x4c, 0x2f, 0x87, 0xeb, 0xc0, 0x99, 0x2a,
        0xb1, 0x77, 0xfb, 0xa5, 0x1d, 0xb9, 0x2c, 0x2a
    };
unsigned char ALICE_PUBLIC[32] = {0x00};
    // 32-byte Bob private key
const unsigned char BOB_PRIV[32] = {
        0x8c, 0x3a, 0x11, 0xd0, 0xb6, 0x9f, 0x7c, 0x2e,
        0x34, 0xe9, 0xa8, 0xf0, 0xb6, 0xc1, 0xd9, 0xe2,
        0xf7, 0xa5, 0xc3, 0xb8, 0xa2, 0xe4, 0xd9, 0xf0,
        0xb6, 0xc5, 0x03, 0xd2, 0xa1, 0xe0, 0xf7, 0xc9
    };
unsigned char BOB_PUBLIC[32] = {0x00};
SessionManager sma = {0};
SessionManager *sm_alice = &sma;
SessionManager smb = {0};
SessionManager *sm_bob = &smb;
int session_init_test(){
    scalar_mult(BOB_PUBLIC, BOB_PRIV, _9);//get bobs public key
    scalar_mult(ALICE_PUBLIC, ALICE_PRIV, _9);
    session_manager_init(sm_alice);
    session_manager_init(sm_bob);
    unsigned char remote_id_alice[32] = {0x01}; // Alice's identity
    unsigned char remote_id_bob[32] = {0x02};   // Bob's identity
    // //tests alice -> bob session creation via session manager
    int a = session_manager_create_session(sm_alice, ROOT_KEY, remote_id_bob, BOB_PUBLIC);
    Session *sa;
    int idx_a = session_manager_find_session(sm_alice, remote_id_bob);
    if(idx_a >= 0) sa = &sm_alice->sessions[idx_a];
    int b = session_manager_accept_session(sm_bob, ROOT_KEY, remote_id_alice, sa->ratchet.dhs, BOB_PRIV);
    Session *sb;
    int idx_b = session_manager_find_session(sm_bob, remote_id_alice);
    if(idx_b >= 0) sb = &sm_bob->sessions[idx_b];
    //tests bob -> alice completing other half of the session and ratchet
    int c = session_manager_create_session(sm_bob, ROOT_KEY, remote_id_alice, ALICE_PUBLIC);
    int d = session_manager_accept_session(sm_alice, ROOT_KEY, remote_id_bob, sb->ratchet.dhs, ALICE_PRIV);
    printf("\nAlice: %d\nSend:\n", idx_a);
    printkey(sa->ratchet.cks);
    printf("\nRecv:\n");
    printkey(sa->ratchet.ckr);
    printf("\nBob: %d\nSend\n", idx_b);
    printkey(sb->ratchet.cks);
    printf("\nRecv:\n");
    printkey(sb->ratchet.ckr);
    return 0;
}
int session_send_test(){
    return 0;
}
int session_recv_test(){
    return 0;
}
int session_out_of_order_test(){
    return 0;
}
int session_new_public_test(){
    return 0;
}