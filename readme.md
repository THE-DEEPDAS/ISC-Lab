User Input → Password Verification (Argon2) → Load User Data → Menu Access
this is the login flow

message sending flow will be
Compose Message → Decrypt Sender's Signing Key → AES-CBC Encrypt → Ed25519 Sign → SHA-256 Hash → Store in Recipient's Inbox

message reading flow
Select Inbox → Decrypt User's Keys → AES-CBC Decrypt → Verify Signature → Verify Hash(integrity) → Display Message

1. Argon2 (Password Hashing)
Purpose: Secure password storage and verification
Type: Memory-hard key derivation function
Configuration: Interactive settings (moderate security/speed)
Usage: crypto_pwhash_str() - Hash passwords for storage
crypto_pwhash_str_verify() - Verify login passwords
crypto_pwhash() - Derive keys from passwords
2. Ed25519 (Digital Signatures)
Purpose: Message authentication and non-repudiation
Type: EdDSA (Edwards-curve Digital Signature Algorithm)
Key Size: 32-byte public key, 64-byte private key
Usage: crypto_sign_keypair() - Generate signature keypair
crypto_sign_detached() - Sign plaintext messages
crypto_sign_verify_detached() - Verify signatures
3. SHA-256 (Message Integrity)
Purpose: Ensure message hasn't been tampered with
Type: Cryptographic hash function
Output: 256-bit (64 hex characters)
Usage: crypto_hash_sha256() - Hash plaintext for integrity checking
4. Salsa20/Poly1305 (Private Key Encryption)
Purpose: Encrypt private keys at rest
Type: Authenticated encryption (via libsodium's secretbox)
Components: Salsa20: Stream cipher for confidentiality
Poly1305: MAC for authentication
Usage: crypto_secretbox_easy() - Encrypt private keys
crypto_secretbox_open_easy() - Decrypt private keys
5. Random Number Generation
Algorithm: ChaCha20-based CSPRNG (libsodium)
Usage: randombytes_buf() - Generate AES keys, IVs, salts, nonces
Ensures cryptographic randomness for all key material
6. Base64 Encoding
Purpose: Convert binary data to text for JSON storage
Usage: sodium_bin2base64() and sodium_base642bin()

confidentiality: Plaintext → AES-CBC Encryption → Ciphertext

Authentication (Sender Verification): Plaintext → Ed25519 Private Key → Digital Signature
Signature → Ed25519 Public Key → Verification ✓/✗

Integrit (Tamper Detection): Plaintext → SHA-256 → Hash
Received Hash = Computed Hash? → ✓/✗

Key Protection (At-Rest Security): Private Keys → Password + Salt → Argon2 → Derived Key → Salsa20/Poly1305 → Encrypted Keys

each message contains;
{
  "cipher_b64": "AES-CBC encrypted message",
  "iv_b64": "Initialization vector",
  "sender": "sender_username", 
  "sign_pub_b64": "Sender's Ed25519 public key",
  "signature_b64": "Ed25519 signature of plaintext",
  "sha256": "SHA-256 hash of plaintext",
  "timestamp": "2024-11-10 15:30:45"
}

hybrid cryptosystem:
Symmetric encryption (AES-CBC) for efficiency
Asymmetric signatures (Ed25519) for authentication
Hash functions (SHA-256, Argon2) for integrity and password security
Authenticated encryption (Salsa20/Poly1305) for key protection

1. argon 2
The password and salt are mixed through multiple passes over a large block of memory.

Each block depends on the previous ones — making parallelization hard.

Final output: A fixed-length hash (key), typically 32 bytes.

2. Ed25519
EdDSA (Edwards-curve Digital Signature Algorithm) using Curve25519.

Based on elliptic-curve cryptography (ECC).

Key Generation:

Derive public key from private key using elliptic curve point multiplication.

Signing:

Hash private key + message → generate signature (R, S).

Verification:

Verify elliptic-curve relation holds for given public key, message, and signature.

3. SHA-256 — Cryptographic Hash Function
Message is divided into 512-bit blocks.

Each block passes through 64 rounds of nonlinear operations:

Bitwise rotations

Modular additions

Logical functions (Ch, Maj)

Final state = 256-bit digest (H₀–H₇ combined).

4. Salsa20/Poly1305 — Authenticated Encryption
Salsa20: Generates a pseudorandom keystream using a 256-bit key + 24-byte nonce.
Ciphertext = Plaintext ⊕ Keystream.

Poly1305: Computes a 16-byte authentication tag (MAC) over ciphertext.

On decryption, the MAC is verified before decryption — ensures authenticity.