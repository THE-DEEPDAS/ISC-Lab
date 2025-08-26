// Substitution Cipher Skeleton Code
#include <bits/stdc++.h>
using namespace std;

// Generate random substitution key
string generateKey() {
	string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    // so we just shuffle the alphabets so i will get a 26 charachter's permutation
	random_shuffle(alphabet.begin(), alphabet.end());
	return alphabet;
}

// Encrypt function (TO BE COMPLETED)
string encrypt(string text, string key) {
	// TODO: Implement encryption using the key mapping
    string ans = "";

    for(char c: text){
        if(!isalpha(c)) {
            ans += c;
            continue;
        }
        char base = isupper(c) ? 'A' : 'a';
        int pos = c - base;
        ans += key[pos];
    }

	return ans;
}

// Decrypt function (TO BE COMPLETED)
string decrypt(string cipher, string key) {
	// TODO: Implement decryption using the inverse key mapping
    // need to find the index of the charachter in key, then do 'a' + index
    string ans = "";

    for(char c: cipher){
        if(!isalpha(c)){
            ans += c;
            continue;
        }
        int idx = key.find(c);
        char base = isupper(c) ? 'A' : 'a';
        ans += (base + idx);
    }

	return ans;
}

int main() {
	// generate key and print mapping like "A -> C"
	string key = generateKey();
	for (int i = 0; i < 26; ++i) {
		cout << static_cast<char>('A' + i) << " -> " << key[i] << endl;
	}

	// Read plaintext from plaintext.txt
	ifstream infile("plaintext.txt", ios::in | ios::binary);
	if (!infile) {
		cerr << "Unable to open plaintext.txt for reading\n";
		return 1;
	}
	string plaintext((istreambuf_iterator<char>(infile)), istreambuf_iterator<char>());
	infile.close();

	// Encrypt and write to cipher.txt
	string cipher = encrypt(plaintext, key);
	ofstream outfile("cipher.txt", ios::out | ios::binary);
	if (!outfile) {
		cerr << "Unable to open cipher.txt for writing\n";
		return 1;
	}
	outfile << cipher;
	outfile.close();
	cout << "Cipher written to cipher.txt\n";

	// Decrypt and write to decrypted.txt
	string decrypted = decrypt(cipher, key);
	ofstream dout("decrypted.txt", ios::out | ios::binary);
	if (!dout) {
		cerr << "Unable to open decrypted.txt for writing\n";
		return 1;
	}
	dout << decrypted;
	dout.close();
	cout << "Decrypted written to decrypted.txt\n";

	return 0;
}