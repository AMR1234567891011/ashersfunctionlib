# How this should work
1. Generate private key and transform that into a curve point
1. Generate and the Identity Key (IK) in the same way
1. Make signed prekeys (SPK) and send bundles to the server
1. Complete X3DH to establish shared secret between Bob and Alice. Keep signed prekey from remote user
1. The initiating user intializes their Double ratchet sending chain and computes cks.
1. On reception of first messsage responding users will calculate their receiving chain ckr.
1. If the responding user wants to send a message back, they calculate their sending chain and send.
1. On reception of a message the initating user will generate their ckr.
1. When ever a public key that does not match previous, a new DH ratchet is started which requires the cks and ckr to be repeated

## 