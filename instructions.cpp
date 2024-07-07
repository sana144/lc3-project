#include "instructions.h"
#include <cstdint>
#include "lc3.h"

uint16_t ir, nzp, dr, sr1, imm_flag, sr2, imm5, base_r, flag, opcode, address, tmp_sr1, temp_sr2, GateALU, value, sr;
int16_t offset9, offset6, offset11;

bool HultInstruction(){
    return (registers.getMDR() == 0xF025);
}

void Fetch(Memory& memory){
    uint16_t pc = registers.getPC();
    registers.setMAR(pc);
    registers.setMDR(memory.read(pc));
    registers.setPC(pc + 1);
    registers.setIR(registers.getMDR());
}

int16_t signExtend(uint16_t value, int bits) {
    if (value & (1 << (bits - 1))) {
        return value | (~((1 << bits) - 1));
    }
    return value;
}

// Helper function to extract specific bits from the instruction register
uint16_t extractBits(uint16_t ir, int start, int length) {
    return (ir >> start) & ((1 << length) - 1);
}

// Decode function
void Decode() {
    opcode = extractBits(registers.getIR(), 12, 4);

    if (opcode == 0x0) { // BR
        ir = registers.getIR();
        nzp = extractBits(ir, 9, 3);
        offset9 = signExtend(ir & 0x1FF, 9);
    }
    else if (opcode == 0x1) { // ADD
        dr = extractBits(registers.getIR(), 9, 3);
        sr1 = extractBits(registers.getIR(), 6, 3);
        imm_flag = extractBits(registers.getIR(), 5, 1);
        if (imm_flag) {
            imm5 = signExtend(registers.getIR() & 0x1F, 5);
        } else {
            sr2 = extractBits(registers.getIR(), 0, 3);
        }
    }
    else if (opcode == 0x2 || opcode == 0xA) { // LD, LDI
        dr = extractBits(registers.getIR(), 9, 3);
        offset9 = signExtend(registers.getIR() & 0x01FF, 9);
    }
    else if (opcode == 0x6) { // LDR
        offset6 = signExtend(registers.getIR() & 0x003F, 6);
        base_r = extractBits(registers.getIR(), 6, 3);
        dr = extractBits(registers.getIR(), 9, 3);
    }
    else if (opcode == 0xE) { // LEA
        offset9 = signExtend(registers.getIR() & 0x1FF, 9);
        dr = extractBits(registers.getIR(), 9, 3);
    }
    else if (opcode == 0x3 || opcode == 0xB) { // ST, STI
        dr = extractBits(registers.getIR(), 9, 3);
        offset9 = signExtend(registers.getIR() & 0x1FF, 9);
    }
    else if (opcode == 0x7) { // STR
        dr = extractBits(registers.getIR(), 9, 3);
        base_r = extractBits(registers.getIR(), 6, 3);
        offset6 = signExtend(registers.getIR() & 0x3F, 6);
    }
    else if (opcode == 0x4) { // JSR, JSRR
        flag = extractBits(registers.getIR(), 11, 1);
        if (flag) {
            offset11 = signExtend(registers.getIR() & 0x7FF, 11);
        } else {
            base_r = extractBits(registers.getIR(), 6, 3);
        }
    }
    else if (opcode == 0x5) { // AND
        dr = extractBits(registers.getIR(), 9, 3);
        sr1 = extractBits(registers.getIR(), 6, 3);
        imm_flag = extractBits(registers.getIR(), 5, 1);
        if (imm_flag) {
            imm5 = signExtend(registers.getIR() & 0x1F, 5);
        } else {
            sr2 = extractBits(registers.getIR(), 0, 3);
        }
    }
    else if (opcode == 0xC) { // JMP, RET
        if (extractBits(registers.getIR(), 6, 3) == 7) {
            // RET
        } else {
            base_r = extractBits(registers.getIR(), 6, 3);
        }
    }
    else if (opcode == 0x9) { // NOT
        dr = extractBits(registers.getIR(), 9, 3);
        sr = extractBits(registers.getIR(), 6, 3);
    }
}

void EvalAddress(Memory &memory) {
    // BR
    if (opcode == 0x0) {
        address = registers.getPC() + offset9;
    }
    // LD
    else if (opcode == 0x2) {
        address = registers.getPC() + offset9;
        registers.setMAR(address);
    }
    // JSR
    else if (opcode == 0x4) {
        if (flag) {
            address = registers.getPC() + offset11; // Update PC with the offset
        } else {
            // JSRR
            address = registers.getR(base_r);
        }
    }
    // LDI
    else if (opcode == 0xA) {
        address = registers.getPC() + offset9;
        registers.setMAR(address);
        address = memory.read(registers.getMAR());
        registers.setMAR(address);
    }
    // LDR
    else if (opcode == 0x6) {
        address = registers.getR(base_r) + offset6;
        registers.setMAR(address);
    }
    // RET , JMP
    else if (opcode == 0xC) {
        if (((registers.getIR() >> 6) & 0x7) == 7) {
            // RET: Set PC to the value contained in R7
            address = registers.getR(7);
        } else {
            // JMP
            address = registers.getR(base_r);
        }
    }
    // LEA
    else if (opcode == 0xE) {
        address = registers.getPC() + offset9;
    }
    // ST
    else if (opcode == 0x3) {
        address = registers.getPC() + offset9;
    }
    // STI
    else if (opcode == 0xB) {
        address = registers.getPC() + offset9;
    }
    // STR
    else if (opcode == 0x7) {
        address = registers.getR(base_r) + offset6;
    }
}

void FetchOperands(Memory &memory) {
    // ADD
    if (opcode == 0x1) {
        tmp_sr1 = registers.getR(sr1);
        temp_sr2 = registers.getR(sr2);
    }
    // AND
    else if (opcode == 0x5) {
        tmp_sr1 = registers.getR(sr1);
        temp_sr2 = registers.getR(sr2);
    }
    // NOT
    else if (opcode == 0x9) {
        tmp_sr1 = registers.getR(sr);
    }
    // LD
    else if (opcode == 0x2) {
        registers.setMDR(memory.read(registers.getMAR()));
    }
    // LDI
    else if (opcode == 0xA) {
        registers.setMDR(memory.read(registers.getMAR()));
    }
    // LDR
    else if (opcode == 0x6) {
        registers.setMDR(memory.read(registers.getMAR()));
    }
    // ST
    else if (opcode == 0x3) {
        value = registers.getR(dr);
    }
    // STI
    else if (opcode == 0xB) {
        value = registers.getR(dr);
    }
    // STR
    else if (opcode == 0x7) {
        value = registers.getR(dr);
    }
}

void Execute() {
    uint16_t opcode = (registers.getIR() >> 12) & 0xF;

    // ADD
    if (opcode == 0x1) {
        if (imm_flag) {
            // Immediate mode
            GateALU = tmp_sr1 + static_cast<int16_t>(imm5);
        } else {
            // Register mode
            GateALU = tmp_sr1 + temp_sr2;
        }
    }
    // AND
    else if (opcode == 0x5) {
        if (imm_flag) {
            // Immediate mode
            GateALU = registers.getR(sr1) & static_cast<int16_t>(imm5);
        } else {
            // Register mode
            GateALU = registers.getR(sr1) & registers.getR(sr2);
        }
    }
    // NOT
    else if (opcode == 0x9) {
        GateALU = ~tmp_sr1;
    }
}

void Store(Memory &memory){
    // BR
    if (opcode == 0x0) {
        uint16_t cc = registers.getCC();
        bool condition_met = ((nzp & 0x4) && (cc & 0x4)) || // n bit
                             ((nzp & 0x2) && (cc & 0x2)) || // z bit
                             ((nzp & 0x1) && (cc & 0x1));   // p bit
        if (condition_met) {
            registers.setPC(address);
        }
    }
    // ADD
    else if (opcode == 0x1) {
        registers.setR(dr, GateALU);
        uint16_t result = registers.getR(dr);
        if (result == 0) {
            registers.setCC(0x02); // Zero
        } else if (result >> 15) {
            registers.setCC(0x04); // Negative
        } else {
            registers.setCC(0x01); // Positive
        }
    }
    // LD
    else if (opcode == 0x2) {
        uint16_t value = registers.getMDR();
        registers.setR(dr, value);
        if (value == 0) {
            registers.setCC(0x02); // Zero flag
        } else if (value & 0x8000) {
            registers.setCC(0x04); // Negative flag
        } else {
            registers.setCC(0x01); // Positive flag
        }
    }
    // LDI
    else if (opcode == 0xA) {
        uint16_t value = registers.getMDR();
        registers.setR(dr, value);
        if (value == 0) {
            registers.setCC(0x02); // Zero flag
        }
        else if (value & 0x8000) {
            registers.setCC(0x04); // Negative flag
        }
        else {
            registers.setCC(0x01); // Positive flag
        }
    }
    // LDR
    else if (opcode == 0x6) {
        uint16_t value = registers.getMDR();
        registers.setR(dr, value);
        if (value == 0) {
            registers.setCC(0x02); // Zero flag
        } else if (value & 0x8000) {
            registers.setCC(0x04); // Negative flag
        } else {
            registers.setCC(0x01); // Positive flag
        }
    }
    // LEA
    else if (opcode == 0xE) {
        registers.setR(dr, address);
    }
    // ST
    else if (opcode == 0x3) {
        registers.setMAR(address);
        registers.setMDR(value);
        index = registers.getMAR();
        memory.write(registers.getMAR(), registers.getMDR());
    }
    // STI
    else if (opcode == 0xB) {
        registers.setMAR(address);
        registers.setMDR(value);
        memory.write(memory.read(registers.getMAR()), registers.getMDR());
        index=memory.read(registers.getMAR());
    }
    // STR
    else if (opcode == 0x7) {
        registers.setMAR(address);
        registers.setMDR(value);
        index = registers.getMAR();
        memory.write(registers.getMAR(), registers.getMDR());
    }
    // JSR JSRR
    else if (opcode == 0x4) {
        uint16_t currentPC = registers.getPC();
        registers.setR(7, currentPC);
        registers.setPC(address);
    }
    // AND
    else if (opcode == 0x5) {
        registers.setR(dr, GateALU);
        uint16_t result = registers.getR(dr);
        if (result == 0) {
            registers.setCC(0x02); // Zero
        } else if (result >> 15) {
            registers.setCC(0x04); // Negative
        } else {
            registers.setCC(0x01); // Positive
        }
    }
    // RET JMP
    else if (opcode == 0xC) {
        registers.setPC(address);
    }
    // NOT
    else if (opcode == 0x9) {
        registers.setR(dr, GateALU);
        uint16_t result = registers.getR(dr);
        if (result == 0) {
            registers.setCC(0x02); // Zero
        } else if (result >> 15) {
            registers.setCC(0x04); // Negative
        } else {
            registers.setCC(0x01); // Positive
        }
    }
}


