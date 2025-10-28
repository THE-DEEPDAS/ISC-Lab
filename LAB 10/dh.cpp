#include <bits/stdc++.h>
using namespace std;

// Safe modular multiplication for 64-bit numbers
uint64_t modmul(uint64_t a, uint64_t b, uint64_t mod)
{
    uint64_t res = 0;
    a %= mod;
    while (b > 0)
    {
        if (b & 1)
            res = (res + a) % mod;
        a = (2 * a) % mod;
        b >>= 1;
    }
    return res;
}

// Function: Modular exponentiation using repeated squaring
uint64_t modexp(uint64_t base, uint64_t exp, uint64_t mod)
{
    uint64_t result = 1;
    base %= mod;

    while (exp > 0)
    {
        if (exp & 1)
            result = modmul(result, base, mod);
        base = modmul(base, base, mod);
        exp >>= 1;
    }
    return result;
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const uint64_t p = 4294967311ULL; // 64-bit prime
    const uint64_t g = 5ULL;          // primitive root modulo p

    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<uint64_t> dist(2, p - 2);

    uint64_t a = dist(gen); // Alice's private key
    uint64_t b = dist(gen); // Bob's private key

    uint64_t A = modexp(g, a, p); // Alice's public value
    uint64_t B = modexp(g, b, p); // Bob's public value

    uint64_t KA = modexp(B, a, p); // Shared key computed by Alice
    uint64_t KB = modexp(A, b, p); // Shared key computed by Bob

    cout << "Public prime p = " << p << "\n";
    cout << "Generator g = " << g << "\n\n";
    cout << "Alice private a = " << a << "\n";
    cout << "Alice public A = " << A << "\n\n";
    cout << "Bob private b = " << b << "\n";
    cout << "Bob public B = " << B << "\n\n";
    cout << "Shared key computed by Alice = " << KA << "\n";
    cout << "Shared key computed by Bob = " << KB << "\n";
    cout << "Keys match: " << (KA == KB ? "YES" : "NO") << "\n";

    return 0;
}