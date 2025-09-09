## Build Process
Build: g++ -O2 -std=c++17 affine.cpp -o affine
Usage: ./affine enc a b
       ./affine dec a b
Input: Reads a single line of text from STDIN.
Policy: Keeps only letters A..Z (uppercased). Non-letters are dropped.

## Text Handling policy
Alphabetic characters are the only ones processed: each letter is uppercased and mapped to 0..25, then transformed with the affine formula modulo 26.
Non-letter characters (digits, punctuation, spaces, etc.) are preserved and printed unchanged, i.e., they are not removed or transformed.

## Task B Writeup on approach
The approach assumes that E and T are the most frequent plaintext letters, and maps them to the two most frequent ciphertext letters. For each possible pairing, we solved the simultaneous equations to obtain candidate keys (a, b) and used them to decrypt the ciphertext. This produced two possible plaintexts. By comparing the outputs, the candidate that gave more meaningful English words was selected as the correct decryption. The strategy succeeded because English text has strong letter frequency patterns, and the affine cipher preserves these distributions, allowing us to exploit them effectively.