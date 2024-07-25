#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

const int MAX_MEM_SIZE = (1 << 13);

void fetchStage(int *icode, int *ifun, int *rA, int *rB, wordType *valC, wordType *valP) {
    wordType pc = getPC();
    byteType byte = getByteFromMemory(pc);

    *icode = (byte >> 4) & 0xf;
    *ifun = byte & 0xf;

    if (*icode == NOP || *icode == HALT) {
        *valP = pc + 1;

        if (*icode == HALT) {
            setStatus(STAT_HLT);
        }
    }

    if (*icode == IRMOVQ || *icode == MRMOVQ || *icode == RMMOVQ) {
        byte = getByteFromMemory(pc + 1);
        *rA = (byte >> 4) & 0xf;
        *rB = byte & 0xf;
        *valC = getWordFromMemory(pc + 2);

        *valP = pc + 10;
    }

    if (*icode == RRMOVQ || *icode == OPQ || *icode == PUSHQ || *icode == POPQ || *icode == CMOVXX) {
        byte = getByteFromMemory(pc + 1);
        *rA = (byte >> 4) & 0xf;
        *rB = byte & 0xf;

        *valP = pc + 2;
    }

    if (*icode == JXX || *icode == CALL) {
        *valC = getWordFromMemory(pc + 1);
        *valP = pc + 9;
    }
}

void decodeStage(int icode, int rA, int rB, wordType *valA, wordType *valB) {
    if (icode == RRMOVQ || icode == CMOVXX ) {
        *valA = getRegister(rA);
    }

    if (icode == OPQ || icode == RMMOVQ) {
        *valA = getRegister(rA);
        *valB = getRegister(rB);
    }

    if (icode == MRMOVQ) {
        *valB = getRegister(rB);
    }

    if (icode == CALL || icode == PUSHQ) {
        *valA = getRegister(rA);
        *valB = getRegister(RSP);
    }

    if (icode == RET || icode == POPQ) {
        *valA = getRegister(RSP);
        *valB = getRegister(RSP);
    }
}

void executeStage(int icode, int ifun, wordType valA, wordType valB, wordType valC, wordType *valE, bool *Cnd) {
    *Cnd = FALSE;
    bool overflow = FALSE;
    switch (icode) {
        case IRMOVQ:
            *valE = valC;
            break;
        case RRMOVQ:
          if (ifun == 0) {  // Unconditional move (RRMOVQ)
                  *valE = valA;
              } else {  // Conditional move (CMOVXX)
                  *valE = valA;
                  *Cnd = Cond(ifun);
                  // if (Cnd) {
                      
                  // }
              }
              break;
        case RMMOVQ:
        case MRMOVQ:
            *valE = valB + valC;
            break;
        case OPQ:
            switch (ifun) {
                case ADD:
                    *valE = valB + valA;
                    overflow = (((valA < 0) == (valB < 0)) && ((*valE < 0) != (valA < 0)));
                    break;
                case SUB:
                    *valE = valB - valA;
                    overflow = (((valA < 0) != (valB < 0)) && ((*valE < 0) != (valB < 0)));
                    break;
                case AND:
                    *valE = valB & valA;
                    break;
                case XOR:
                    *valE = valB ^ valA;
                    break;
            }
            setFlags(*valE < 0, *valE == 0, overflow);
            break;
        case JXX:
            *Cnd = Cond(ifun);
            break;
        case CALL:
        case PUSHQ:
            *valE = valB - 8;
            break;
        case RET:
        case POPQ:
            *valE = valB + 8;
            break;
    }

}

void memoryStage(int icode, wordType valA, wordType valP, wordType valE, wordType *valM) {
    switch (icode) {
        case RMMOVQ:
        case PUSHQ:
            setWordInMemory(valE, valA);
            break;
        case MRMOVQ:
            *valM = getWordFromMemory(valE);
            break;
        case CALL:
            setWordInMemory(valE, valP);
            break;
        case RET:
        case POPQ:
            *valM = getWordFromMemory(valA);
            break;
    }
}

void writebackStage(int icode, int rA, int rB, wordType valE, wordType valM, bool Cnd) {
    if(icode == CMOVXX){
      if (Cnd) {
      setRegister(rB, valE);
      }
    }
    switch (icode) {
        case RRMOVQ:
        case IRMOVQ:
        case OPQ:
            setRegister(rB, valE);
            break;
        case MRMOVQ:
          setRegister(rA, valM);
            break;
        case POPQ:
            setRegister(rA, valM);
            setRegister(RSP, valE);
            break;
        case CALL:
        case RET:
        case PUSHQ:
            setRegister(RSP, valE);
            break;
        
    }
}

void pcUpdateStage(int icode, wordType valC, wordType valP, bool Cnd, wordType valM) {
    wordType newPC = valP;

    switch (icode) {
        case JXX:
            newPC = Cnd ? valC : valP;
            break;
        case CALL:
            newPC = valC;
            break;
        case RET:
            newPC = valM;
            break;
    }

    setPC(newPC);
}

void stepMachine(int stepMode) {
    /* FETCH STAGE */
    int icode = 0, ifun = 0;
    int rA = 0, rB = 0;
    wordType valC = 0;
    wordType valP = 0;
 
    /* DECODE STAGE */
    wordType valA = 0;
    wordType valB = 0;

    /* EXECUTE STAGE */
    wordType valE = 0;
    bool Cnd = FALSE;

    /* MEMORY STAGE */
    wordType valM = 0;

    fetchStage(&icode, &ifun, &rA, &rB, &valC, &valP);
    applyStageStepMode(stepMode, "Fetch", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);

    decodeStage(icode, rA, rB, &valA, &valB);
    applyStageStepMode(stepMode, "Decode", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);
  
    executeStage(icode, ifun, valA, valB, valC, &valE, &Cnd);
    applyStageStepMode(stepMode, "Execute", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);
  
    memoryStage(icode, valA, valP, valE, &valM);
    applyStageStepMode(stepMode, "Memory", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);
  
    writebackStage(icode, rA, rB, valE, valM, Cnd);
    applyStageStepMode(stepMode, "Writeback", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);
  
    pcUpdateStage(icode, valC, valP, Cnd, valM);
    applyStageStepMode(stepMode, "PC update", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);

    incrementCycleCounter();
}

int main(int argc, char **argv) {
    int stepMode = 0;
    FILE *input = parseCommandLine(argc, argv, &stepMode);

    initializeMemory(MAX_MEM_SIZE);
    initializeRegisters();
    loadMemory(input);

    applyStepMode(stepMode);
    while (getStatus() != STAT_HLT) {
        stepMachine(stepMode);
        applyStepMode(stepMode);
#ifdef DEBUG
        printMachineState();
        printf("\n");
#endif
    }
    printMachineState();
    return 0;
}