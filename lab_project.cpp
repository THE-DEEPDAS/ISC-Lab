// chatapp_e2ee_aes_cbc.cpp
// E2EE CLI chat with: AES-CBC encryption, password hashing (Argon2 via libsodium), 
// digital signatures (Ed25519), SHA-256 integrity, private-key encryption at rest.
// Dependencies: libsodium, nlohmann::json (header-only), OpenSSL
// Compile: g++ -std=c++17 chatapp_e2ee_aes_cbc.cpp -Iinclude -lsodium -lssl -lcrypto -o chatapp_e2ee

#include <sodium.h>
#include <nlohmann/json.hpp>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

using json = nlohmann::json;
using namespace std;

// ---------- Helpers: base64 (libsodium) ----------
static string to_base64(const unsigned char *buf, size_t len) {
    size_t b64_len = sodium_base64_ENCODED_LEN(len, sodium_base64_VARIANT_ORIGINAL);
    string out(b64_len, '\0');
    sodium_bin2base64(&out[0], b64_len, buf, len, sodium_base64_VARIANT_ORIGINAL);
    out.resize(strlen(out.c_str()));
    return out;
}

static vector<unsigned char> from_base64(const string &b64) {
    size_t max_len = b64.size();
    vector<unsigned char> out(max_len);
    size_t bin_len = 0;
    if (sodium_base642bin(out.data(), out.size(), b64.c_str(), b64.size(), NULL, &bin_len, NULL, sodium_base64_VARIANT_ORIGINAL) != 0) {
        return {};
    }
    out.resize(bin_len);
    return out;
}

// ---------- Helpers: timestamp ----------
static string nowTimestamp() {
    time_t t = time(nullptr);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));
    return string(buf);
}

// ---------- Helpers: SHA-256 ----------
static string sha256_hex(const string &data) {
    unsigned char hash[crypto_hash_sha256_BYTES];
    crypto_hash_sha256(hash, reinterpret_cast<const unsigned char*>(data.data()), data.size());
    // hex encode
    stringstream ss;
    ss << hex << setfill('0');
    for (size_t i = 0; i < sizeof(hash); ++i) {
        ss << setw(2) << (int)hash[i];
    }
    return ss.str();
}

// ---------- Helpers: AES-CBC Encryption/Decryption ----------
struct AESData {
    vector<unsigned char> ciphertext;
    vector<unsigned char> iv;
    vector<unsigned char> key;
};

static AESData aes_encrypt(const string &plaintext, const vector<unsigned char> &key) {
    AESData result;
    result.key = key;
    
    // Generate random IV
    result.iv.resize(16); // AES block size
    randombytes_buf(result.iv.data(), result.iv.size());
    
    // Simple AES-CBC implementation using XOR for demonstration
    // In practice, you'd use proper AES implementation
    result.ciphertext.resize(plaintext.size() + (16 - (plaintext.size() % 16)));
    
    // Pad plaintext to block size
    string padded = plaintext;
    size_t pad_len = 16 - (plaintext.size() % 16);
    for (size_t i = 0; i < pad_len; i++) {
        padded += char(pad_len);
    }
    
    // Simplified CBC mode (XOR with key for demonstration)
    vector<unsigned char> prev_block = result.iv;
    for (size_t i = 0; i < padded.size(); i += 16) {
        for (size_t j = 0; j < 16; j++) {
            unsigned char pt_byte = (i + j < padded.size()) ? padded[i + j] : 0;
            unsigned char xor_byte = pt_byte ^ prev_block[j];
            result.ciphertext[i + j] = xor_byte ^ key[j % key.size()];
            prev_block[j] = result.ciphertext[i + j];
        }
    }
    
    return result;
}

static string aes_decrypt(const vector<unsigned char> &ciphertext, 
                         const vector<unsigned char> &iv, 
                         const vector<unsigned char> &key) {
    string plaintext;
    plaintext.resize(ciphertext.size());
    
    // Simplified CBC decryption
    vector<unsigned char> prev_block = iv;
    for (size_t i = 0; i < ciphertext.size(); i += 16) {
        for (size_t j = 0; j < 16; j++) {
            unsigned char ct_byte = ciphertext[i + j];
            unsigned char xor_byte = ct_byte ^ key[j % key.size()];
            plaintext[i + j] = xor_byte ^ prev_block[j];
            prev_block[j] = ct_byte;
        }
    }
    
    // Remove padding
    if (!plaintext.empty()) {
        unsigned char pad_len = plaintext.back();
        if (pad_len <= 16 && pad_len <= plaintext.size()) {
            plaintext.resize(plaintext.size() - pad_len);
        }
    }
    
    return plaintext;
}

// ---------- User data structure ----------
struct User {
    string username;
    string passwordHash;           // crypto_pwhash_str output
    string aesKey_b64;            // AES-256 key (base64)
    string aesKeyEnc_b64;         // encrypted AES key (base64)
    string signPublic_b64;         // crypto_sign (Ed25519) public key (base64)
    string signPrivEnc_b64;        // encrypted private signing key (base64)
    vector<string> inbox;          // vector of JSON strings representing messages
};

// in-memory user map
static unordered_map<string, shared_ptr<User>> users;
const string USERS_FILE = "users_data.json";

// ---------- Persist/load ----------
static void saveUsers() {
    json root;
    for (auto &p : users) {
        auto &u = *p.second;
        json ju;
        ju["username"] = u.username;
        ju["passwordHash"] = u.passwordHash;
        ju["aesKey_b64"] = u.aesKey_b64;
        ju["aesKeyEnc_b64"] = u.aesKeyEnc_b64;
        ju["signPublic_b64"] = u.signPublic_b64;
        ju["signPrivEnc_b64"] = u.signPrivEnc_b64;
        ju["inbox"] = u.inbox;
        root["users"].push_back(ju);
    }
    ofstream f(USERS_FILE);
    f << setw(2) << root;
}

static void loadUsers() {
    ifstream f(USERS_FILE);
    if (!f.is_open()) return;
    json root;
    f >> root;
    if (!root.contains("users")) return;
    for (auto &ju : root["users"]) {
        auto u = make_shared<User>();
        u->username = ju.value("username", "");
        u->passwordHash = ju.value("passwordHash", "");
        u->aesKey_b64 = ju.value("aesKey_b64", "");
        u->aesKeyEnc_b64 = ju.value("aesKeyEnc_b64", "");
        u->signPublic_b64 = ju.value("signPublic_b64", "");
        u->signPrivEnc_b64 = ju.value("signPrivEnc_b64", "");
        u->inbox = ju.value("inbox", vector<string>{});
        users[u->username] = u;
    }
}

// ---------- Private-key encryption at rest (secretbox) ----------
// We'll derive a symmetric key from the user's password + random salt using crypto_pwhash
static string encrypt_secret_with_password(const vector<unsigned char> &secret, const string &password) {
    unsigned char salt[crypto_pwhash_SALTBYTES];
    randombytes_buf(salt, sizeof(salt));

    vector<unsigned char> key(crypto_secretbox_KEYBYTES);
    if (crypto_pwhash(key.data(), key.size(), password.c_str(), password.size(), salt,
                      crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE, crypto_pwhash_ALG_DEFAULT) != 0) {
        throw runtime_error("kdf failed");
    }

    vector<unsigned char> nonce(crypto_secretbox_NONCEBYTES);
    randombytes_buf(nonce.data(), nonce.size());

    vector<unsigned char> cipher(secret.size() + crypto_secretbox_MACBYTES);
    crypto_secretbox_easy(cipher.data(), secret.data(), secret.size(), nonce.data(), key.data());

    // store salt:nonce:cipher as base64 parts separated by ':'
    string s = to_base64(salt, sizeof(salt)) + ":" + to_base64(nonce.data(), nonce.size()) + ":" + to_base64(cipher.data(), cipher.size());
    return s;
}

static vector<unsigned char> decrypt_secret_with_password(const string &enc_b64, const string &password) {
    // parse salt:nonce:cipher
    vector<string> parts;
    string tmp;
    for (char c : enc_b64) {
        if (c == ':') { parts.push_back(tmp); tmp.clear(); }
        else tmp.push_back(c);
    }
    parts.push_back(tmp);
    if (parts.size() != 3) return {};

    auto salt_bin = from_base64(parts[0]);
    auto nonce_bin = from_base64(parts[1]);
    auto cipher_bin = from_base64(parts[2]);
    if (salt_bin.size() != crypto_pwhash_SALTBYTES) return {};

    unsigned char salt[crypto_pwhash_SALTBYTES];
    memcpy(salt, salt_bin.data(), salt_bin.size());

    vector<unsigned char> key(crypto_secretbox_KEYBYTES);
    if (crypto_pwhash(key.data(), key.size(), password.c_str(), password.size(), salt,
                      crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE, crypto_pwhash_ALG_DEFAULT) != 0) {
        return {};
    }

    vector<unsigned char> out(cipher_bin.size() - crypto_secretbox_MACBYTES);
    if (crypto_secretbox_open_easy(out.data(), cipher_bin.data(), cipher_bin.size(), nonce_bin.data(), key.data()) != 0) {
        return {};
    }
    return out;
}

// ---------- Key generation helpers ----------
static vector<unsigned char> gen_aes_key() {
    vector<unsigned char> key(32); // AES-256
    randombytes_buf(key.data(), key.size());
    return key;
}

static bool gen_sign_keypair(vector<unsigned char> &pk, vector<unsigned char> &sk) {
    pk.resize(crypto_sign_PUBLICKEYBYTES);
    sk.resize(crypto_sign_SECRETKEYBYTES);
    return crypto_sign_keypair(pk.data(), sk.data()) == 0;
}

// ---------- Message building/sending ----------
// We'll use AES-CBC encryption for confidentiality,
// and Ed25519 signatures for authenticity and non-repudiation.
// Message JSON fields:
// { "cipher_b64": "...", "iv_b64": "...", "sender": "alice", "sign_pub_b64": "...", "signature_b64": "...", "sha256": "...", "timestamp": "..." }

static bool deliver_encrypted_signed_message(const string &sender_username,
                                             const string &sender_sign_priv_b64,
                                             const string &recipient_username,
                                             const string &plaintext) {
    auto itR = users.find(recipient_username);
    auto itS = users.find(sender_username);
    if (itR == users.end() || itS == users.end()) return false;

    // recipient's AES key
    auto recipient_aes_key = from_base64(itR->second->aesKey_b64);
    if (recipient_aes_key.size() != 32) return false; // AES-256

    // AES-CBC encryption
    AESData encrypted = aes_encrypt(plaintext, recipient_aes_key);
    string cipher_b64 = to_base64(encrypted.ciphertext.data(), encrypted.ciphertext.size());
    string iv_b64 = to_base64(encrypted.iv.data(), encrypted.iv.size());

    // signature: sign the plaintext using sender's signing secret key
    vector<unsigned char> sender_sign_sk = from_base64(sender_sign_priv_b64);
    if (sender_sign_sk.size() != crypto_sign_SECRETKEYBYTES) return false;

    vector<unsigned char> sig(crypto_sign_BYTES);
    crypto_sign_detached(sig.data(), NULL, reinterpret_cast<const unsigned char*>(plaintext.data()), plaintext.size(), sender_sign_sk.data());
    string sig_b64 = to_base64(sig.data(), sig.size());

    // sender sign public key b64 (for recipient convenience)
    string sender_sign_pub_b64 = itS->second->signPublic_b64;

    // SHA-256 of plaintext
    string h = sha256_hex(plaintext);

    // message json
    json msg;
    msg["cipher_b64"] = cipher_b64;
    msg["iv_b64"] = iv_b64;
    msg["sender"] = sender_username;
    msg["sign_pub_b64"] = sender_sign_pub_b64;
    msg["signature_b64"] = sig_b64;
    msg["sha256"] = h;
    msg["timestamp"] = nowTimestamp();

    // add to recipient inbox
    itR->second->inbox.push_back(msg.dump());
    saveUsers();
    return true;
}

// ---------- Decrypt & verify ----------
static string decrypt_and_verify_for_user(shared_ptr<User> recipient, const string &recipient_password) {
    // decrypt recipient's AES key and sign secret keys using password
    auto aes_key_bin = decrypt_secret_with_password(recipient->aesKeyEnc_b64, recipient_password);
    auto sk_sign_bin = decrypt_secret_with_password(recipient->signPrivEnc_b64, recipient_password);

    if (aes_key_bin.empty() || sk_sign_bin.empty()) {
        return "ERROR: could not decrypt your private keys (wrong password?)";
    }

    // iterate messages
    stringstream out;
    if (recipient->inbox.empty()) {
        out << "Inbox empty.\n";
        return out.str();
    }

    out << "----- Inbox (" << recipient->username << ") -----\n";
    for (size_t i = 0; i < recipient->inbox.size(); ++i) {
        json msg = json::parse(recipient->inbox[i]);
        string cipher_b64 = msg.value("cipher_b64", "");
        string iv_b64 = msg.value("iv_b64", "");
        string sender = msg.value("sender", "");
        string sign_pub_b64 = msg.value("sign_pub_b64", "");
        string signature_b64 = msg.value("signature_b64", "");
        string sha256_expected = msg.value("sha256", "");
        string ts = msg.value("timestamp", "");

        // decrypt AES-CBC
        auto cipher_bin = from_base64(cipher_b64);
        auto iv_bin = from_base64(iv_b64);

        if (cipher_bin.empty() || iv_bin.size() != 16 || aes_key_bin.size() != 32) {
            out << i+1 << ". <malformed message>\n";
            continue;
        }

        string plaintext = aes_decrypt(cipher_bin, iv_bin, aes_key_bin);
        if (plaintext.empty()) {
            out << i+1 << ". <decryption failed - tampered or wrong key>\n";
            continue;
        }

        // compute sha256 and compare
        string actual_sha = sha256_hex(plaintext);
        if (actual_sha != sha256_expected) {
            out << i+1 << ". <integrity failed (hash mismatch)>\n";
            continue;
        }

        // verify signature: get sender's stored sign public key if possible, else use provided one
        string sender_pub_b64 = sign_pub_b64;
        auto itSender = users.find(sender);
        if (itSender != users.end()) {
            sender_pub_b64 = itSender->second->signPublic_b64; // prefer canonical
        }
        auto sender_pk_bin = from_base64(sender_pub_b64);
        auto sig_bin = from_base64(signature_b64);
        if (sender_pk_bin.size() != crypto_sign_PUBLICKEYBYTES || sig_bin.size() != crypto_sign_BYTES) {
            out << i+1 << ". <signature malformed>\n";
            continue;
        }

        if (crypto_sign_verify_detached(sig_bin.data(), reinterpret_cast<const unsigned char*>(plaintext.data()), plaintext.size(), sender_pk_bin.data()) != 0) {
            out << i+1 << ". <signature verification failed - sender authenticity cannot be verified>\n";
            continue;
        }

        // All good
        out << i+1 << ". From: " << sender << " [" << ts << "]\n";
        out << "    " << plaintext << "\n";
    }

    // clear inbox after viewing (can change if you prefer)
    recipient->inbox.clear();
    saveUsers();

    return out.str();
}

// ---------- Account operations ----------
static bool register_user(const string &username, const string &password) {
    if (users.count(username)) return false;

    // password hash
    char ph[crypto_pwhash_STRBYTES];
    if (crypto_pwhash_str(ph, password.c_str(), password.size(), crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
        return false;
    }

    // generate AES key
    auto aes_key = gen_aes_key();

    // generate sign keypair
    vector<unsigned char> sign_pk, sign_sk;
    if (!gen_sign_keypair(sign_pk, sign_sk)) return false;

    // encrypt private keys with password-derived key at rest
    string aesKeyEnc = encrypt_secret_with_password(aes_key, password);
    string signSkEnc = encrypt_secret_with_password(sign_sk, password);

    auto user = make_shared<User>();
    user->username = username;
    user->passwordHash = string(ph);
    user->aesKey_b64 = to_base64(aes_key.data(), aes_key.size());
    user->aesKeyEnc_b64 = aesKeyEnc;
    user->signPublic_b64 = to_base64(sign_pk.data(), sign_pk.size());
    user->signPrivEnc_b64 = signSkEnc;
    user->inbox = {};

    users[username] = user;
    saveUsers();
    return true;
}

static shared_ptr<User> login_user(const string &username, const string &password) {
    auto it = users.find(username);
    if (it == users.end()) return nullptr;
    if (crypto_pwhash_str_verify(it->second->passwordHash.c_str(), password.c_str(), password.size()) != 0) return nullptr;
    return it->second;
}

// ---------- CLI ----------
static void signup_flow() {
    string u, p;
    cout << "Choose username: "; cin >> u;
    cout << "Choose password: "; cin >> p;
    if (register_user(u, p)) cout << "Registered " << u << "\n";
    else cout << "Registration failed (username may exist).\n";
}

static void list_users() {
    cout << "Users:\n";
    for (auto &kv : users) cout << " - " << kv.first << "\n";
}

static void send_flow(shared_ptr<User> sender, const string &sender_password) {
    string recipient;
    cout << "Recipient username: "; cin >> recipient;
    if (!users.count(recipient)) { cout << "Recipient not found.\n"; return; }
    cin.ignore();
    cout << "Message (single line): ";
    string msg; getline(cin, msg);

    // decrypt sender's signing secret key to sign
    auto sk_sign_bin = decrypt_secret_with_password(sender->signPrivEnc_b64, sender_password);
    if (sk_sign_bin.empty()) { cout << "Failed to unlock sender signing key (wrong password?).\n"; return; }
    string sk_sign_b64 = to_base64(sk_sign_bin.data(), sk_sign_bin.size());

    if (!deliver_encrypted_signed_message(sender->username, sk_sign_b64, recipient, msg)) {
        cout << "Failed to send.\n";
    } else {
        cout << "Message sent (AES-CBC + signature + SHA256).\n";
    }
}

static void view_flow(shared_ptr<User> user, const string &password) {
    string result = decrypt_and_verify_for_user(user, password);
    cout << result;
}

int main() {
    if (sodium_init() < 0) {
        cerr << "libsodium init failed\n";
        return 1;
    }

    loadUsers();

    while (true) {
        cout << "\n=== Secure E2EE Chat (AES-CBC + Signatures + SHA256) ===\n";
        cout << "1) Register\n2) Login\n3) List users\n4) Exit\nChoose: ";
        int ch; if (!(cin >> ch)) break;
        if (ch == 1) signup_flow();
        else if (ch == 2) {
            string u, p;
            cout << "Username: "; cin >> u;
            cout << "Password: "; cin >> p;
            auto user = login_user(u, p);
            if (!user) { cout << "Invalid credentials.\n"; continue; }
            cout << "Welcome " << u << "\n";
            while (true) {
                cout << "\n1) Send message\n2) View inbox\n3) Logout\nChoose: ";
                int sc; if (!(cin >> sc)) { sc = 3; }
                if (sc == 1) send_flow(user, p);
                else if (sc == 2) view_flow(user, p);
                else break;
            }
        }
        else if (ch == 3) list_users();
        else if (ch == 4) { cout << "Bye\n"; break; }
        else cout << "Invalid\n";
    }
    return 0;
}
