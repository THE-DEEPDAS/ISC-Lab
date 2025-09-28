#pragma once
#include <cstdint>

namespace des {

    // Round function f: takes 32-bit R and 48-bit subkey K (low 48 bits used)
    uint32_t feistel(uint32_t R, uint64_t subkey);

    // Optional: expose key schedule if you want to call it from tests
    void key_schedule(uint64_t key64, uint64_t subkeys[16]);

} // namespace des 
