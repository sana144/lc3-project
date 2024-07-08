# LC3 Simulator Project
This project is a simulator for the LC3 (Little Computer 3), a computer architecture. The simulator allows users to execute LC3 assembly language programs and visualize their execution.

### Instruction Set Simulation

The simulator fully supports the LC3 instruction set, including data movement, arithmetic operations, logic operations, and control flow instructions. This allows for accurate emulation of LC3 assembly programs.
The tool provides a detailed visualization of memory contents and register states, aiding in the understanding of program execution processes.

### LC3 Architecture Overview

The LC3 is a 16-bit computer architecture featuring:

- **Registers**: Eight general-purpose registers (R0-R7), each 16 bits wide.
- **Memory**: A 65,536-location memory, with each location storing a 16-bit word.
- **Program Counter (PC)**: A 16-bit register that holds the address of the next instruction to be executed.
- **Instruction Register (IR)**: Holds the currently executing instruction.
- **Condition Codes (CC)**: Reflects the result of the last executed instruction (negative, zero, or positive).

### Project Structure
The project consists of several key classes, each responsible for a different part of the simulation:

 **1. Memory**
This class manages the memory of the LC3 simulator. It provides methods to read from and write to specific memory addresses.

**2. Registers**
This class represents the registers in the LC3 simulator. It includes methods for getting and setting values for the Program Counter (PC), Instruction Register (IR), Condition Code Register (CC), general-purpose registers (R0-R7), Memory Address Register (MAR), and Memory Data Register (MDR).

**3. FileClass**
This class handles file operations for the simulator. It includes methods for reading from and writing to files, which are essential for loading programs into memory and saving the state of the simulator.

**4. Assembler**
This class is responsible for assembling LC3 assembly language code into machine code that can be executed by the simulator. It processes the input assembly file, handles labels and addresses, and converts instructions into binary format.

**5. Instructions**
This class manages the execution of LC3 instructions. It includes methods for fetching, decoding, evaluating addresses, fetching operands, executing instructions, and storing results back into memory or registers.

**6. lc3**
This class manages the graphical user interface (GUI) of the simulator. It provides a user-friendly interface for uploading assembly code, assembling the code, and stepping through the execution of instructions. It also includes methods for updating the display of registers and memory.


### Instruction Set
- **Data Movement Instructions**: `LD`, `LDI`, `LDR`, `LEA`, `ST`, `STR`, `STI`
- **Arithmetic and Logic Instructions**: `ADD`, `AND`, `NOT`
- **Control Instructions**: `BR`, `JMP`, `JSR`, `JSRR`, `RET`


### Instruction Processing Cycle

The LC3 Simulator emulates the complete instruction processing cycle, which consists of six key stages:

1. **Fetch**
   - Load the next instruction (at the address stored in the PC) from memory into the Instruction Register (IR). Copy the contents of the PC into the Memory 
    Address Register (MAR).Send a "read" signal to memory.Copy the contents of the Memory Data Register (MDR) into the IR.Increment the PC, so that it points to        the next instruction in sequence (PC becomes PC+1).

    At first, you have to upload the input file by pressing the upload button, then assemble it by pressing the assemble button and see the success message if it       is successful. Then, by pressing the Next Step button, go through each of the following steps for each command

2. **Decode**
    Identify the opcode , Depending on the opcode, identify other operands from the remaining bits.
     
3. **Evaluate Address**
   Compute the address used for memory access, if required.
   
4. **Fetch Operands**
   Obtain source operands needed to perform the operation.
   
5. **Execute**
    Perform the operation using the source operands.
     
6. **Store**
    Write results to the destination (register or memory).

These 6 steps continue for each command by pressing the next step button and proceed according to the 6 functions written in the instruction file for each instrunction.

![lc3 simulator](https://github.com/sana144/lc3-project/blob/main/lc3%20simulator.png)
