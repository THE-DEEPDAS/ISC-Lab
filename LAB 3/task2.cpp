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
    auto message1 = readFile("message1.txt");
    auto message2 = readFile("message2.txt");

    auto key = generateKey(message1.size());
    writeFile("key.bin", key);

    auto message1Ciphertext = xorData(message1, key);
    writeFile("ciphertext1.bin", message1Ciphertext);

    auto message2Ciphertext = xorData(message2, key);
    writeFile("ciphertext2.bin", message2Ciphertext);

    // Decryption
    auto readKey = readFile("key.bin");

    auto c1xorc2 = xorData(message1Ciphertext, message2Ciphertext);
    auto m1xorm2 = xorData(message1, message2);
    if (m1xorm2 == c1xorc2) cout << "The XOR of the two messages is equal to the XOR of their ciphertexts. Hence Proved" << endl;
    else cout << "The XOR of the two messages is NOT equal to the XOR of their ciphertexts." << endl;
}