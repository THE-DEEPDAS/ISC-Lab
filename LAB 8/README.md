## Build
Build: g++ -O2 -std=c++17 main.cpp des.cpp des_tables.cpp -o des
Usage: ./des (Windows PowerShell: .\des.exe)
Purpose: Show DES key schedule + single Feistel round for the standard sample key.

## Key Schedule (des_tables.cpp)
Input: 64-bit key with parity. Drop parity via PC-1 (64->56). Split into C,D (28 bits).
Rotate each round using: 1 1 2 2 2 2 2 2 1 2 2 2 2 2 2 1.
After each rotate apply PC-2 (56->48) to form Ki. Store Ki in low 48 bits of uint64_t.
No reversal here; decryption would just iterate keys in reverse.

## Feistel f(R, K) (des.cpp)
1. Expand R (32->48) with E table.
2. XOR with subkey (mask to 48 bits).
3. 8 S-box lookups (row: bits 5 & 0, col: bits 4..1).
4. Concatenate 8 * 4-bit outputs (32 bits).
5. Permute with P box.

## Notes
- Bit numbering is MSB-first; tables are used directly (subtract 1 for 0-based).
- Rotations are masked with 0x0FFFFFFF to keep 28 bits.
- Differences you might see vs some online subkey lists usually come from alternate formatting of the 48-bit value, not logic errors.

## Driver (main.cpp)
Prints 16 subkeys for key 0x133457799BBCDFF1 and runs one sample f() call.

## Summary
Filled TODOs by direct table-driven bit permutations and rotations, mirroring the concise style used in earlier labs.
