#include "registers.h"
#include <cstdint>

Registers::Registers()
{
    PC = 0;
    IR = 0;
    CC = 0;
    for (int i = 0; i < 8; ++i) {
        R[i] = 0;
    }
    MAR = 0;
    MDR = 0;
}

uint16_t Registers::getPC() const
{
    return PC;
}

void Registers::setPC(uint16_t value)
{
    PC = value;
}

uint16_t Registers::getIR() const
{
    return IR;
}

void Registers::setIR(uint16_t value)
{
    IR = value;
}

uint16_t Registers::getCC() const
{
    return CC;
}

void Registers::setCC(uint16_t value)
{
    CC = value;
}

uint16_t Registers::getR(uint8_t index) const
{
    if (index < 8) {
        return R[index];
    } else {
        return 0;
    }
}

void Registers::setR(uint8_t index, uint16_t value)
{
    if (index < 8) {
        R[index] = value;
    }
}

uint16_t Registers::getMAR() const
{
    return MAR;
}

void Registers::setMAR(uint16_t value)
{
    MAR = value;
}

uint16_t Registers::getMDR() const
{
    return MDR;
}

void Registers::setMDR(uint16_t value)
{
    MDR = value;
}
