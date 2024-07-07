#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <vector>


class Memory
{
public:
    Memory(uint16_t size);

    uint16_t read(uint16_t address) const;
    void write(uint16_t address, uint16_t value);

private:
    std::vector<uint16_t> memory;
};

#endif // MEMORY_H
