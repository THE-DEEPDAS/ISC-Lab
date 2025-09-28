#include "des.hpp"
#include <cstdint>
#include <stdexcept>

// External declarations for tables from des_tables.cpp
extern const int Ebit[48];
extern const int Pbox[32];
extern const uint8_t SBOX[8][64];

namespace des
{

    // E-expansion, S-boxes, P-permutation, and key schedule are provided in des_tables.cpp

    uint32_t feistel(uint32_t R, uint64_t subkey)
    {
        // Step 1: E-expansion (32 -> 48 bits)
        uint64_t expanded = 0;
        for(int i = 0; i < 48; ++i) {
            int from = Ebit[i] - 1; // Convert from 1-based to 0-based indexing
            uint64_t bit = (R >> (32 - 1 - from)) & 1ULL; // Extract bit from position 'from'
            expanded = (expanded << 1) | bit;
        }
        
        // Step 2: XOR with subkey (48 bits)
        expanded ^= (subkey & 0x0000FFFFFFFFFFFFULL); // Use only low 48 bits of subkey
        
        // Step 3: Apply S-boxes (8 boxes, 6 bits -> 4 bits each)
        uint32_t sbox_output = 0;
        for(int i = 0; i < 8; ++i) {
            // Extract 6 bits for S-box i
            int shift = (7 - i) * 6; // Start from most significant 6 bits
            uint8_t six_bits = (expanded >> shift) & 0x3F; // Extract 6 bits
            
            // Calculate row and column for S-box lookup
            int row = ((six_bits >> 5) & 1) << 1 | (six_bits & 1); // Bits 0 and 5 form row (2 bits)
            int col = (six_bits >> 1) & 0xF; // Bits 1-4 form column (4 bits)
            
            // Lookup in S-box
            uint8_t sbox_value = SBOX[i][row * 16 + col];
            
            // Combine into output (4 bits per S-box)
            sbox_output = (sbox_output << 4) | sbox_value;
        }
        
        // Step 4: Apply P-permutation (32 -> 32 bits)
        uint32_t result = 0;
        for(int i = 0; i < 32; ++i) {
            int from = Pbox[i] - 1; // Convert from 1-based to 0-based indexing
            uint32_t bit = (sbox_output >> (32 - 1 - from)) & 1UL;
            result = (result << 1) | bit;
        }
        
        return result;
    }

} // namespace des