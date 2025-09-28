#include "des.hpp"
#include <iostream>
#include <iomanip>
#include <cstdint>
int main()
{
    // Example 64-bit key with parity bits (from DES standard)
    uint64_t key = 0x133457799BBCDFF1ULL;
    uint64_t subkeys[16];
    des::key_schedule(key, subkeys);
    std::cout << "Key schedule for key 0x133457799BBCDFF1\n";
    for (int i = 0; i < 16; ++i)
    {
        std::cout << "Round " << std::setw(2) << (i + 1)
                  << ": 0x" << std::hex << std::uppercase
                  << std::setw(12) << std::setfill('0') << subkeys[i]
                  << std::dec << "\n";
    }
    
    // Test feistel function with a simple example
    uint32_t test_R = 0xF0AAF0AA;
    uint64_t test_subkey = 0x1B02EFFC7072; 
    uint32_t feistel_result = des::feistel(test_R, test_subkey);
    
    std::cout << "\nFeistel function test:\n";
    std::cout << "Input R: 0x" << std::hex << std::uppercase << test_R << "\n";
    std::cout << "Subkey:  0x" << std::hex << std::uppercase << test_subkey << "\n";
    std::cout << "Output:  0x" << std::hex << std::uppercase << feistel_result << std::dec << "\n";
    
    return 0;
}
/*
Round 1: 0x1B02EFFC7072
Round 2: 0x79AED9DBC9E5
Round 3: 0x55FC8A06D1FA
Round 4: 0x72ADD6DB351D
Round 5: 0x7CEC07EB53A8
Round 6: 0x63A53E507B2F
Round 7: 0xEC84B7F618BC
Round 8: 0xF75E369A2D9F
Round 9: 0xDB4F1C64763C
Round 10: 0x81AC77D6ECF2
Round 11: 0x5F6A1D2F573E
Round 12: 0x0DC98C1AEDA7
Round 13: 0x33638D0CBFD0
Round 14: 0x196A0B2C95F5
Round 15: 0x42C7F0EDE5E0
Round 16: 0x7A9BE42F2009
*/