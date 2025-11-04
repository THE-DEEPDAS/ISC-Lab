// SHA-1 Implementation
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <cstdint>
#include <algorithm>
using namespace std;

class SHA1 {
private:
    uint32_t H[5];

    static uint32_t ROTL(uint32_t x, unsigned int n) {
        n &= 31u;
        return (x << n) | (x >> (32 - n));
    }

    static uint32_t f(int t, uint32_t B, uint32_t C, uint32_t D) {
        if (t >= 0 && t <= 19) {
            return (B & C) | ((~B) & D);
        } else if (t >= 20 && t <= 39) {
            return B ^ C ^ D;
        } else if (t >= 40 && t <= 59) {
            return (B & C) | (B & D) | (C & D);
        } else { // 60..79
            return B ^ C ^ D;
        }
    }

    static uint32_t K(int t) {
        if (t >= 0 && t <= 19) {
            return 0x5A827999u;
        } else if (t >= 20 && t <= 39) {
            return 0x6ED9EBA1u;
        } else if (t >= 40 && t <= 59) {
            return 0x8F1BBCDCu;
        } else { // 60..79
            return 0xCA62C1D6u;
        }
    }

    // Pad message per SHA-1: append 0x80, then 0x00 until length % 64 == 56, then append 8-byte big-endian length
    static vector<uint8_t> padMessage(const string& message) {
        vector<uint8_t> padded;
        padded.reserve(message.size() + 64);
        // message bytes
        for (unsigned char c : message) padded.push_back(static_cast<uint8_t>(c));

        uint64_t originalBits = static_cast<uint64_t>(message.size()) * 8ULL;

        // append 0x80
        padded.push_back(0x80);

        // append 0x00 until (len % 64) == 56
        while ((padded.size() % 64) != 56) {
            padded.push_back(0x00);
        }

        // append 64-bit big-endian length
        for (int i = 7; i >= 0; --i) {
            padded.push_back(static_cast<uint8_t>((originalBits >> (i * 8)) & 0xFF));
        }

        return padded;
    }

    static uint32_t bytesToWord(const vector<uint8_t>& bytes, size_t offset) {
        return (static_cast<uint32_t>(bytes[offset]) << 24) |
               (static_cast<uint32_t>(bytes[offset + 1]) << 16) |
               (static_cast<uint32_t>(bytes[offset + 2]) << 8) |
               (static_cast<uint32_t>(bytes[offset + 3]));
    }

public:
    SHA1() {
        H[0] = 0x67452301u;
        H[1] = 0xEFCDAB89u;
        H[2] = 0x98BADCFEu;
        H[3] = 0x10325476u;
        H[4] = 0xC3D2E1F0u;
    }

    string hash(const string& message, bool verbose = true) {
        if (verbose) {
            cout << "Input message: \"" << message << "\"" << endl;
            cout << "Message length: " << message.length() << " bytes (" << (message.length() * 8) << " bits)" << endl;
        }

        vector<uint8_t> padded = padMessage(message);

        if (verbose) {
            cout << "\nPadded message (hex):" << endl;
            for (size_t i = 0; i < padded.size(); ++i) {
                cout << hex << setw(2) << setfill('0') << static_cast<int>(padded[i]) ;
                if ((i + 1) % 16 == 0) cout << "\n";
                else cout << " ";
            }
            cout << dec << endl;
            cout << "Padded message length: " << padded.size() << " bytes (" << (padded.size() * 8) << " bits)" << endl;
        }

        int numBlocks = static_cast<int>(padded.size() / 64);
        if (verbose) cout << "Number of 512-bit blocks: " << numBlocks << endl << endl;

        // Process each block
        for (int block = 0; block < numBlocks; ++block) {
            uint32_t W[80];
            size_t off = block * 64;
            for (int t = 0; t < 16; ++t) {
                W[t] = bytesToWord(padded, static_cast<size_t>(off + t * 4));
            }
            for (int t = 16; t < 80; ++t) {
                W[t] = ROTL(W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16], 1);
            }

            uint32_t A = H[0];
            uint32_t B = H[1];
            uint32_t C = H[2];
            uint32_t D = H[3];
            uint32_t E = H[4];

            for (int t = 0; t < 80; ++t) {
                uint32_t temp = ROTL(A, 5) + f(t, B, C, D) + E + W[t] + K(t);
                E = D;
                D = C;
                C = ROTL(B, 30);
                B = A;
                A = temp;

                if (verbose && t < 5) {
                    cout << "Round " << t << ": A=" << hex << setw(8) << setfill('0') << A
                         << " B=" << setw(8) << B << " C=" << setw(8) << C
                         << " D=" << setw(8) << D << " E=" << setw(8) << E << dec << endl;
                }
            }

            H[0] = H[0] + A;
            H[1] = H[1] + B;
            H[2] = H[2] + C;
            H[3] = H[3] + D;
            H[4] = H[4] + E;

            if (verbose) {
                cout << "Hash values after block " << (block + 1) << ":" << endl;
                cout << hex << setw(8) << setfill('0') << H[0] << " "
                     << setw(8) << H[1] << " " << setw(8) << H[2] << " "
                     << setw(8) << H[3] << " " << setw(8) << H[4] << dec << endl << endl;
            }
        }

        // produce final hex digest (lowercase)
        ostringstream oss;
        oss << hex << setfill('0');
        for (int i = 0; i < 5; ++i) {
            oss << setw(8) << H[i];
        }
        string res = oss.str();
        // ensure lowercase
        transform(res.begin(), res.end(), res.begin(), ::tolower);
        return res;
    }
};


int main() {
    string input;
    cout << "Enter message to hash: ";
    getline(cin, input);

    SHA1 sha1;
    string digest = sha1.hash(input, true);

    cout << "=== FINAL RESULT ===" << endl;
    cout << "SHA-1 digest: " << digest << endl;
    cout << "Length: 160 bits (40 hex characters)" << endl;
}