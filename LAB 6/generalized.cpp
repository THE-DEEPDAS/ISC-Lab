#include <bits/stdc++.h>
using namespace std;

int gcdExtended(int a, int b, int *x, int *y) {
    // Base case
    if (a == 0) {
        *x = 0, *y = 1;
        return b;
    }

    int x1, y1; // To store results of recursive call
    int gcd = gcdExtended(b % a, a, &x1, &y1);

    // Update x and y using results of recursion
    *x = y1 - (b / a) * x1;
    *y = x1;

    return gcd;
}

int modInverse(int a, int m) {
    a = a % m;  // handle numbers larger than m
    int x, y;
    int g = gcdExtended(a, m, &x, &y);

    if (g != 1) {
        // Inverse doesn’t exist if gcd(a, m) != 1
        return -1;
    } else {
        // m is added to handle negative x
        return (x % m + m) % m;
    }
}

vector<vector<int>> matrixInverse(vector<vector<int>> K, int mod)
{
    int n = K.size();
    vector<vector<int>> inv(n, vector<int>(n, 0));
    int det = 0;

    if (n == 2) {
        det = (K[0][0] * K[1][1] - K[0][1] * K[1][0]) % mod;
        if (det < 0) det += mod;
        vector<vector<int>> adj = {
            { K[1][1], -K[0][1] },
            { -K[1][0], K[0][0] }
        };
        int invDet = modInverse(det, mod);
        if (invDet == -1) {
            cerr << "Matrix is not invertible" << endl;
            return {};
        }
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++) {
                int val = adj[i][j];
                if (val < 0) val += mod;
                inv[i][j] = (val * invDet) % mod;
                if (inv[i][j] < 0) inv[i][j] += mod;
            }
    } else if (n == 3) {
        // Calculate determinant
        det = (K[0][0]*(K[1][1]*K[2][2] - K[1][2]*K[2][1])
             - K[0][1]*(K[1][0]*K[2][2] - K[1][2]*K[2][0])
             + K[0][2]*(K[1][0]*K[2][1] - K[1][1]*K[2][0])) % mod;
        if (det < 0) det += mod;
        int invDet = modInverse(det, mod);
        if (invDet == -1) {
            cerr << "Matrix is not invertible" << endl;
            return {};
        }
        // Compute adjugate matrix
        vector<vector<int>> adj(3, vector<int>(3));
        adj[0][0] = (K[1][1]*K[2][2] - K[1][2]*K[2][1]) % mod;
        adj[0][1] = -(K[1][0]*K[2][2] - K[1][2]*K[2][0]) % mod;
        adj[0][2] = (K[1][0]*K[2][1] - K[1][1]*K[2][0]) % mod;
        adj[1][0] = -(K[0][1]*K[2][2] - K[0][2]*K[2][1]) % mod;
        adj[1][1] = (K[0][0]*K[2][2] - K[0][2]*K[2][0]) % mod;
        adj[1][2] = -(K[0][0]*K[2][1] - K[0][1]*K[2][0]) % mod;
        adj[2][0] = (K[0][1]*K[1][2] - K[0][2]*K[1][1]) % mod;
        adj[2][1] = -(K[0][0]*K[1][2] - K[0][2]*K[1][0]) % mod;
        adj[2][2] = (K[0][0]*K[1][1] - K[0][1]*K[1][0]) % mod;
        // Transpose adjugate and multiply by invDet
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++) {
                int val = adj[j][i];
                if (val < 0) val += mod;
                inv[i][j] = (val * invDet) % mod;
                if (inv[i][j] < 0) inv[i][j] += mod;
            }
    }
    return inv;
}

vector<int> encryptBlock(const vector<int>& P, const vector<vector<int>>& K, int mod)
{
    int n = K.size();
    vector<int> C(n, 0);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            C[i] = (C[i] + P[j] * K[j][i]) % mod;
    return C;
}

vector<int> decryptBlock(const vector<int>& C, const vector<vector<int>>& Kinv, int mod)
{
    int n = Kinv.size();
    vector<int> P(n, 0);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            P[i] = (P[i] + C[j] * Kinv[j][i]) % mod;
    return P;
}

string cleanText(const string& s) {
    string res;
    for (char c : s) {
        if (isalpha(c)) res += toupper(c);
    }
    return res;
}

int main()
{
    int n, mod = 26;
    cout << "Enter n (2 or 3): ";
    cin >> n;
    
    vector<vector<int>> K(n, vector<int>(n));
    cout << "Enter key matrix (" << n << "x" << n << "):\n";
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            cin >> K[i][j];

    // Read plaintext from file
    ifstream in("plaintext.txt");
    if (!in) {
        cerr << "Could not open plaintext.txt" << endl;
        return 1;
    }
    string raw, plaintext;
    while (getline(in, raw)) plaintext += raw;
    in.close();
    plaintext = cleanText(plaintext);

    // Pad plaintext
    while (plaintext.size() % n != 0)
        plaintext += 'X';

    // Encrypt
    vector<vector<int>> ciphertext;
    ofstream out("cipher text 1.txt");
    for (size_t i = 0; i < plaintext.size(); i += n) {
        vector<int> P(n);
        for (int j = 0; j < n; j++)
            P[j] = plaintext[i + j] - 'A';
        vector<int> C = encryptBlock(P, K, mod);
        ciphertext.push_back(C);
        for (int x : C) out << (char)(x + 'A');
    }
    out << endl;
    out.close();

    // Decrypt
    vector<vector<int>> Kinv = matrixInverse(K, mod);
    if (Kinv.empty()) {
        cerr << "Key matrix not invertible. Exiting." << endl;
        return 1;
    }
    ofstream dout("decrypted.txt");
    for (const auto& C : ciphertext) {
        vector<int> P = decryptBlock(C, Kinv, mod);
        for (int x : P) dout << (char)(x + 'A');
    }
    dout << endl;
    dout.close();

    cout << "Saras" << endl;
}