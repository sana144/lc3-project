#include "memory.h"

Memory::Memory(uint16_t size) {
    memory.resize(size);
}

uint16_t Memory::read(uint16_t address) const {
    if (address < memory.size()) {
        return memory[address];
    }
    else
    {
        return 0;
    }
}

void Memory::write(uint16_t address, uint16_t value) {
    if (address < memory.size()) {
        memory[address] = value;
    }
}
