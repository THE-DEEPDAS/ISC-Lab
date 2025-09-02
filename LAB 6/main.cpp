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
    int det = (K[0][0] * K[1][1] - K[0][1] * K[1][0]) % mod;
    if (det < 0) det += mod; // Ensure positive determinant
    vector<vector<int>> adjoint = {
        { K[1][1], -K[0][1] },
        { -K[1][0], K[0][0] }
    };
    int invDet = modInverse(det, mod);
    if (invDet == -1) {
        cerr << "Matrix is not invertible" << endl;
        return {};
    }
    vector<vector<int>> inverse(2, vector<int>(2));
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            int val = adjoint[i][j];
            // Normalize adjoint value to [0, mod-1]
            if (val < 0) val += mod;
            inverse[i][j] = (val * invDet) % mod;
            // Normalize result to [0, mod-1]
            if (inverse[i][j] < 0) inverse[i][j] += mod;
        }
    }
    return inverse;
}

vector<int> encryptBlock(vector<int> P, vector<vector<int>> K,
                         int mod)
{
    // TODO: Implement C = P * K mod 26
    vector<int> C(2);
    for (int i = 0; i < 2; i++) {
        C[i] = (P[0] * K[i][0] + P[1] * K[i][1]) % 26;
    }
    return C;
}

vector<int> decryptBlock(vector<int> C, vector<vector<int>> Kinv, int mod)
{
    // TODO: Implement P = C * Kinv mod 26
    vector<int> P(2);
    for (int i = 0; i < 2; i++) {
        P[i] = (C[0] * Kinv[i][0] + C[1] * Kinv[i][1]) % 26;
    }
    return P;
}

int main()
{
    string plaintext = "HELP";
    vector<vector<int>> K = {{3, 3}, {2, 5}};
    int mod = 26;
    // now for help what i can do is encrypt HE first and then encrypt LP
    // as matrix is of dimension 2 * 2 so I need to split the plaintext into pairs of characters
    vector<string> pairs;
    for (size_t i = 0; i < plaintext.length(); i += 2) {
        if (i + 1 < plaintext.length()) {
            pairs.push_back(plaintext.substr(i, 2));
        } else {
            pairs.push_back(plaintext.substr(i, 1) + "X");
        }
    }
    // Encrypt each pair
    vector<vector<int>> ciphertext;
    for (const string &p : pairs) {
        vector<int> P = {p[0] - 'A', p[1] - 'A'};
        vector<int> C = encryptBlock(P, K, mod);
        cout << "Encrypted " << p << " to " << char(C[0] + 'A') << char(C[1] + 'A') << endl;
        ciphertext.push_back(C);
    }
    cout << "Ciphertext matrix:" << endl;
    // store the encrypted text to ciphertext.txt file
    ofstream out("ciphertext.txt");
    for(const auto &row: ciphertext){
        for(const auto &it: row) {
            out << (it + 'A') << " ";
            cout << (it + 'A') << " ";
        }
        out << endl;
        cout << endl;
    }
    out.close();
    // Decrypt each pair
    vector<vector<int>> Kinv = matrixInverse(K, mod);
    for (const vector<int> &C : ciphertext) {
        vector<int> P = decryptBlock(C, Kinv, mod);
        cout << "Decrypted " << char(C[0] + 'A') << char(C[1] + 'A') << " to " << char(P[0] + 'A') << char(P[1] + 'A') << endl;
    }
}