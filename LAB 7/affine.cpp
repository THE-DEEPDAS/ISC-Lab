#include <bits/stdc++.h>
using namespace std;

// ----- small helpers -----
static inline int mod(int x, int m)
{
    int r = x % m;
    return (r < 0) ? r + m : r;
}
static inline bool is_valid_a(int a)
{
    // Valid a iff gcd(a,26) == 1
    int x = abs(a), y = 26;
    while (y)
    {
        int t = x % y;
        x = y;
        y = t;
    }
    return x == 1;
}

// Convert to uppercase A..Z only, drop everything else
static string sanitize_letters_only_upper(const string &s)
{
    string t;
    t.reserve(s.size());
    for (char c : s)
        if (isalpha((unsigned char)c))
            t.push_back((char)toupper(c));
    return t;
}

// ===== \todo: modular inverse a^{-1} (mod 26) via extended Euclid =====
// Return the multiplicative inverse of a modulo m (=26 here).
// If gcd(a,m)!=1, throw.
static int modinv(int a, int m)
{
    int r0 = mod(a, m), r1 = m, s0 = 1, s1 = 0;
    // reduce a mod m to keep numbers small
    while (r1 != 0) {
        int q = r0 / r1;
        int r2 = r0 - q * r1; r0 = r1; r1 = r2;
        int s2 = s0 - q * s1; s0 = s1; s1 = s2;
    }
    if (r0 != 1) {
        throw runtime_error("modinv: a is not invertible modulo m");
    }
    return mod(s0, m);
}

// ===== \todo: encrypt =====
// For each letter X in 0..25, compute Y = (a*X+b) mod 26 and map back to ’A’+Y.
static string encrypt_text(const string &plain, int a, int b)
{
    string out;
    out.reserve(plain.size());
    for (char c : plain) {
        if (isalpha((unsigned char)c)) {
            char U = (char)toupper((unsigned char)c);
            int x = U - 'A';
            int y = mod(a * x + b, 26);
            out.push_back((char)('A' + y));
        } else {
            // keep non-letter characters as-is
            out.push_back(c);
        }
    }
    return out;
}

// ===== \todo: decrypt =====
// Use a_inv = a^{-1} (mod 26): X = a_inv*(Y-b) mod 26.
static string decrypt_text(const string &cipher, int a, int b)
{
    int a_inv = modinv(a, 26);
    string out;
    out.reserve(cipher.size());
    for (char c : cipher) {
        if (isalpha((unsigned char)c)) {
            char U = (char)toupper((unsigned char)c);
            int y = U - 'A';
            int x = mod(a_inv * mod(y - b, 26), 26);
            out.push_back((char)('A' + x));
        } else {
            // keep non-letter characters as-is
            out.push_back(c);
        }
    }
    return out;
}

int main(int argc, char **argv)
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (argc != 4)
    {
        cerr << "Usage: " << argv[0] << " <enc|dec> a b\n";
        return 1;
    }

    string mode = argv[1];
    int a = stoi(argv[2]);
    int b = stoi(argv[3]);

    if (!is_valid_a(a))
    {
        cerr << "Error: a must be coprime with 26. Choose a in {1,3,5,7,9,11,15,17,19,21,23,25}.\n";
        return 1;
    }
    b = mod(b, 26);

    string line;
    if (!getline(cin, line))
        line = "";

    // Do not drop non-letters: pass the full line, letters will be processed and
    // non-letters kept intact by encrypt_text/decrypt_text.
    string in = line;

    try
    {
        if (mode == "enc")
        {
            cout << encrypt_text(in, a, b) << "\n";
        }
        else if (mode == "dec")
        {
            cout << decrypt_text(in, a, b) << "\n";
        }
        else
        {
            cerr << "Mode must be 'enc' or 'dec'.\n";
            return 1;
        }
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

/*
---------------------------
Minimal self-check (manual)
---------------------------
Example (once TODOs are done):

$ echo HELLOWORLD | ./affine enc 5 8
RCLLAOAPLX

$ echo RCLLAOAPLX | ./affine dec 5 8
HELLOWORLD
*/