#include <bits/stdc++.h>
using namespace std;

int mod(int x, int m)
{
    int r = x % m;
    return (r < 0) ? r + m : r;
}

int modinv(int a, int m)
{
    int r0 = a, r1 = m, s0 = 1, s1 = 0;
    r0 = mod(r0, m);
    while (r1)
    {
        int q = r0 / r1;
        int r2 = r0 - q * r1;
        r0 = r1;
        r1 = r2;
        int s2 = s0 - q * s1;
        s0 = s1;
        s1 = s2;
    }
    if (r0 != 1)
        return -1;
    return mod(s0, m);
}

string decrypt(const string &cipher, int a, int b)
{
    int a_inv = modinv(a, 26);
    if (a_inv == -1)
        return "";
    string out;
    for (char c : cipher)
    {
        if (isalpha(c))
        {
            int y = toupper(c) - 'A';
            int x = mod(a_inv * (y - b), 26);
            out.push_back('A' + x);
        }
    }
    return out;
}

int main()
{
    string cipher;
    getline(cin, cipher);

    // count frequencies
    vector<int> freq(26, 0);
    for (char c : cipher)
        if (isalpha(c))
            freq[toupper(c) - 'A']++;

    // find top 2 letters
    vector<pair<int, int>> f; // (count, letter)
    for (int i = 0; i < 26; i++)
        f.push_back({freq[i], i});
    sort(f.rbegin(), f.rend());
    int y1 = f[0].second, y2 = f[1].second;

    // E=4, T=19
    vector<pair<int, int>> mappings = {{4, 19}, {19, 4}};

    for (auto mp : mappings)
    {
        int p1 = mp.first, p2 = mp.second;

        for (auto cy : {make_pair(y1, y2), make_pair(y2, y1)})
        {
            int c1 = cy.first, c2 = cy.second;
            int diff_p = mod(p2 - p1, 26);
            int diff_c = mod(c2 - c1, 26);
            int inv = modinv(diff_p, 26);
            if (inv == -1)
                continue;

            int a = mod(inv * diff_c, 26);
            int b = mod(c1 - a * p1, 26);

            string plain = decrypt(cipher, a, b);
            if (!plain.empty())
            {
                cout << "Candidate key: a=" << a << ", b=" << b << "\n";
                cout << "Decryption: "
                     << plain.substr(0, 100) << "\n\n";
            }
        }
    }
    return 0;
}
