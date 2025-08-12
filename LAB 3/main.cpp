#include <bits/stdc++.h>
using namespace std;

vector<unsigned char> readFile(const string &filename)
{
    ifstream file(filename, ios ::binary);
    return vector<unsigned char>((istreambuf_iterator<char>(file)), {});
}

void writeFile(const string &filename, const vector<unsigned char> &data)
{
    ofstream file(filename, ios ::binary);
    file.write((char *)data.data(), data.size());
}

vector<unsigned char> generateKey(size_t length)
{
    vector<unsigned char> key(length);
    // fill 'key' with random bytes
    generate_n(key.begin(), length, rand);
    return key;
}

vector<unsigned char> xorData(const vector<unsigned char> &a,
                              const vector<unsigned char> &b)
{
    vector<unsigned char> result(a.size());
    // XOR each byte of 'a' with corresponding byte of 'b'
    for (size_t i = 0; i < a.size(); ++i)
    {
        result[i] = a[i] ^ b[i];
    }
    return result;
}

int main()
{
    // Encryption
    auto plaintext = readFile("plaintext.txt");

    auto key = generateKey(plaintext.size());
    writeFile("key.bin", key);

    auto ciphertext = xorData(plaintext, key);
    writeFile("ciphertext.bin", ciphertext);

    // Decryption
    auto readKey = readFile("key.bin");

    auto decrypted = xorData(ciphertext, readKey);
    writeFile("decrypted.txt", decrypted);
}