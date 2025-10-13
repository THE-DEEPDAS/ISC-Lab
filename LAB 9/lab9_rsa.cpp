#include <iostream>
#include <stdexcept>
#include <cstdint>
using namespace std;

// ---------- TODO(1): Extended Euclidean Algorithm ----------
struct EG { long long g, x, y; };
EG egcd(long long a, long long b) {
    if (b == 0) {
        return {a, 1, 0};
    }
    auto t = egcd(b, a % b);
    long long x = t.y;
    long long y = t.x - (a / b) * t.y;
    return {t.g, x, y};
}

// ---------- TODO(2): Modular Inverse ----------
long long modinv(long long a, long long m) {
    auto t = egcd(a, m);
    if (t.g != 1 && t.g != -1) {
        throw std::runtime_error("Inverse does not exist");
    }
    long long inv = t.x % m;
    if (inv < 0) inv += m;
    return inv;
}

// ---------- TODO(3): Modular Exponentiation ----------
long long modexp(long long base, long long exp, long long mod) {
    base %= mod;
    long long res = 1;
    while (exp > 0) {
        if (exp & 1LL) {
            res = (res * base) % mod;
        }
        base = (base * base) % mod;
        exp >>= 1;
    }
    return res;
}

int main() {
    cout << "=== RSA Implementation (Toy Example) ===\n";

    // ---------- TODO(4): Key Generation ----------
    long long p = 61, q = 53;   // Example primes (students can change)
    long long n = p * q;
    long long phi = (p - 1) * (q - 1);
    long long e = 65537;
    if (egcd(e, phi).g != 1) {
        e = 3;
        while (e < phi && egcd(e, phi).g != 1) {
            e += 2;
        }
        if (egcd(e, phi).g != 1) {
            throw std::runtime_error("Failed to find public exponent co-prime with phi");
        }
    }
    long long d = modinv(e, phi);

    cout << "p = " << p << "\nq = " << q << "\n";
    cout << "n = " << n << "\nphi(n) = " << phi << "\n";
    cout << "Public exponent e = " << e << "\nPrivate exponent d = " << d << "\n";

    // ---------- TODO(5): Encrypt & Decrypt ----------
    long long m = 65;   // Example message (integer)
    long long c = modexp(m, e, n); // ciphertext
    long long m_dec = modexp(c, d, n);

    cout << "\nMessage m = " << m;
    cout << "\nCiphertext c = " << c;
    cout << "\nDecrypted message m_dec = " << m_dec << "\n";

    if (m == m_dec) cout << "\n[OK] Decryption successful!\n";
    else cout << "\n[ERROR] Decryption failed!\n";

    return 0;
}
