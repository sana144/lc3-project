#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H
#include "registers.h"
#include "memory.h"

extern uint16_t ir, nzp, dr, sr1, base_r, sr2, imm5;
extern int16_t offset9, offset6, offset11;
extern uint16_t flag, opcode, address, tmp_sr1, temp_sr2, GateALU, value, sr, imm_flag;

void Fetch( Memory&);
void Decode();
void EvalAddress( Memory&);
void FetchOperands(Memory&);
void Execute();
void Store(Memory &);
bool HultInstruction();

#endif // INSTRUCTIONS_H
