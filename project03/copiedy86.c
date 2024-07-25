#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

const int MAX_MEM_SIZE  = (1 << 13);

void fetchStage(int *icode, int *ifun, int *rA, int *rB, wordType *valC, wordType *valP) {
  wordType curr = getPC();
  // setFlags(0,0,0);
  byteType newBytes = getByteFromMemory(curr);
  
  *icode = newBytes >> 4;
  
  *ifun = newBytes & 0xF;
  
  // Check if instruction needs rA and rB
  if (*icode == HALT) {
    // We must halt here
    *valP = getPC() + 1;
    setStatus(STAT_HLT);
    return;
  }

  if (*icode == NOP) {
    // Nop 
    *valP = getPC() + 1;
    return;
  }

  if (*icode == RRMOVQ)
  {
    byteType newReg = getByteFromMemory(curr + 1);
    *rA = newReg >> 4;
    *rB = newReg & 0x0F;
    *valP = getPC() + 2;
  }

  if (*icode == CMOVXX)
  {
    // hmmmm
    byteType newReg = getByteFromMemory(curr + 1);
    *rA = newReg >> 4;
    *rB = newReg & 0x0F;
    *valP =  getPC() + 2;
  }

  if (*icode == IRMOVQ)
  {
    byteType newReg = getByteFromMemory(curr + 1);
    *rA = newReg >> 4;
    *rB = newReg & 0x0F;
    *valC = getWordFromMemory(getPC() + 2);
    *valP =  getPC() + 10;
  }

  if (*icode == RMMOVQ)
  {
    byteType newReg = getByteFromMemory(curr + 1);
    *rA = newReg >> 4;
    *rB = newReg & 0x0F;
    *valC = getWordFromMemory(getPC() + 2);
    *valP =  getPC() + 10;
  }

  if (*icode == MRMOVQ)
  {
    byteType newReg = getByteFromMemory(curr + 1);
    *rA = newReg >> 4;
    *rB = newReg & 0x0F;
    *valC = getWordFromMemory(getPC() + 2);
    *valP =  getPC() + 10;
  }

  if (*icode == OPQ)
  {
    byteType newReg = getByteFromMemory(curr + 1);
    *rA = newReg >> 4;
    *rB = newReg & 0x0F;
    *valP =  getPC() + 2;
  }

  if (*icode == JXX)
  {
    *valC = getWordFromMemory(getPC() + 1);
    *valP = getPC() + 9;
  }

  if (*icode == CALL)
  {
    *valC = getWordFromMemory(getPC() + 1);
    *valP = getPC() + 9;
  }

  if (*icode == RET) 
  {
    *valP = getPC() + 1;
  }

  if (*icode == PUSHQ)
  {
    byteType newReg = getByteFromMemory(curr + 1);
    *rA = newReg >> 4;
    *rB = newReg & 0x0F;
    *valP =  getPC() + 2;
  }

  if (*icode == POPQ)
  {
    byteType newReg = getByteFromMemory(curr + 1);
    *rA = newReg >> 4;
    *rB = newReg & 0x0F;
    *valP =  getPC() + 2;
  }
}

void decodeStage(int icode, int rA, int rB, wordType *valA, wordType *valB) {
 if (icode == HALT) {
    return;
  }

  if (icode == NOP) {
    return;
  }

  if (icode == RRMOVQ)
  {
    *valA = getRegister(rA);
  }

  if (icode == CMOVXX)
  {
    *valA = getRegister(rA);
  }

  if (icode == IRMOVQ)
  {
    return;
  }

  if (icode == RMMOVQ)
  {
    *valA = getRegister(rA);
    *valB = getRegister(rB);
  }

  if (icode == MRMOVQ)
  {
    *valB = getRegister(rB);
  }

  if (icode == OPQ)
  {
    *valA = getRegister(rA);
    *valB = getRegister(rB);

  }

  if (icode == JXX)
  {
    return;
  }

  if (icode == CALL)
  {
    *valB = getRegister(RSP);
  }

  if (icode == RET) 
  {
    *valA = getRegister(RSP);
    *valB = getRegister(RSP);
  }

  if (icode == PUSHQ)
  {
    *valA = getRegister(rA);
    *valB = getRegister(RSP);
  }

  if (icode == POPQ)
  {
    *valA = getRegister(RSP);
    *valB = getRegister(RSP);
  }
}

void executeStage(int icode, int ifun, wordType valA, wordType valB, wordType valC, wordType *valE, bool *Cnd) {
  if (icode == HALT) {
    return;
  }

  if (icode == NOP) {
    return;
  }

  if (icode == RRMOVQ)
  {
    *valE = 0 + valA;
  }

  if (icode == CMOVXX)
  {
    *valE = 0 + valA;
    *Cnd = Cond(ifun);
  }

  if (icode == IRMOVQ)
  {
    *valE = 0 + valC;
  }

  if (icode == RMMOVQ)
  {
    *valE = valB + valC;
  }

  if (icode == MRMOVQ)
  {
    *valE = valB + valC;
  }

  if (icode == OPQ)
  {
    // check page 201 for conditions to set flags
    if (ifun == ADD)
    {
      *valE = valB + valA; 
      if (valB < 0 && valA < 0 && *valE >= 0){
        if (*valE == 0){
          setFlags(0, 1, 1);
        }
        else {
          setFlags(0, 0, 1);
        }
      }
      else if (valB >= 0 && valA >= 0 && *valE < 0){
        setFlags(1, 0, 1);
      }
      else if (*valE == 0){
        setFlags(0, 1, 0);
      }
      else if (*valE < 0){
        setFlags(1,0,0);
      }
      else {
        setFlags(0,0,0);
      }
    }
    if (ifun == SUB)
    {
      // hmmmm
      // int temp = -valB;
      *valE = valB - valA;
      if (valB < 0 && valA > 0 && *valE > 0){
        setFlags(0, 0, 1);
      }
      else if (valB >= 0 && valA < 0 && *valE <= 0){
        if (*valE == 0){
          setFlags(0,1,1);
        }
        else{
          setFlags(0, 0, 1);
        }
      }
      else if (*valE == 0){
        setFlags(0, 1, 0);
      }
      else if (*valE < 0){
        setFlags(1,0,0);
      }
      else {
        setFlags(0,0,0);
      }
    }
    if (ifun == AND)
    {
      // hmmmm
      *valE = valB & valA;
      if (*valE == 0){
        printf("WHATTHEHECK");
        setFlags(0,1,0);
      }
      else if (*valE < 0){
        setFlags(1,0,0);
      }
      else {
        setFlags(0,0,0);
      }
    }
    if (ifun == XOR)
    {
      // hmmmm
      *valE = valB ^ valA;
      if (*valE == 0){
        setFlags(0,1,0);
      }
      else if (*valE < 0){
        setFlags(1,0,0);
      }
      else {
        setFlags(0,0,0);
      }
    }
  }

  if (icode == JXX)
  {
    
    *Cnd = Cond(ifun);
  }

  if (icode == CALL)
  {
    *valE = valB + (-8);
  }

  if (icode == RET) 
  {
    *valE= valB + 8;
  }

  if (icode == PUSHQ)
  {
    *valE =  valB + (-8);
  }

  if (icode == POPQ)
  {
    *valE = valB + 8;
  }
}

void memoryStage(int icode, wordType valA, wordType valP, wordType valE, wordType *valM) {
 if (icode == HALT) {
    return;
  }

  if (icode == NOP) {
    return;
  }

  if (icode == RRMOVQ)
  {
    return;
  }

  if (icode == CMOVXX)
  {
    return;
  }

  if (icode == IRMOVQ)
  {
    return;
  }

  if (icode == RMMOVQ)
  {
    setWordInMemory(valE, valA);
  }

  if (icode == MRMOVQ)
  {
    *valM = getWordFromMemory(valE);
  }

  if (icode == OPQ)
  {
    return;
  }

  if (icode == JXX)
  {
    return;
  }

  if (icode == CALL)
  {
    setWordInMemory(valE, valP);
  }

  if (icode == RET) 
  {
    *valM = getWordFromMemory(valA);
  }

  if (icode == PUSHQ)
  {
    setWordInMemory(valE, valA);
  }

  if (icode == POPQ)
  {
    *valM = getWordFromMemory(valA);
  }
}

void writebackStage(int icode, int rA, int rB, wordType valE, wordType valM, bool Cnd) {
  if (icode == HALT) {
    return;
  }

  if (icode == NOP) {
    return;
  }

  if (icode == RRMOVQ)
  {
    setRegister(rB, valE);
  }

  if (icode == CMOVXX)
  {
    if (Cnd) {
      setRegister(rB, valE);
    }
  }

  if (icode == IRMOVQ)
  {
    setRegister(rB, valE);
  }

  if (icode == RMMOVQ)
  {
    return;
  }

  if (icode == MRMOVQ)
  {
    setRegister(rA, valM);
  }

  if (icode == OPQ)
  {
    setRegister(rB, valE);
  }

  if (icode == JXX)
  {
    return;
  }

  if (icode == CALL)
  {
    setRegister(RSP, valE);
  }

  if (icode == RET) 
  {
    setRegister(RSP, valE);
  }

  if (icode == PUSHQ)
  {
    setRegister(RSP, valE);
  }

  if (icode == POPQ)
  {
    setRegister(RSP, valE);
    setRegister(rA, valM);
  }
}

void pcUpdateStage(int icode, wordType valC, wordType valP, bool Cnd, wordType valM) {
  if (icode == HALT) {
    setPC(valP);
  }

  if (icode == NOP) {
    setPC(valP);
  }

  if (icode == RRMOVQ)
  {
    setPC(valP);
  }

  if (icode == CMOVXX)
  {
    setPC(valP);
  }

  if (icode == IRMOVQ)
  {
    setPC(valP);
  }

  if (icode == RMMOVQ)
  {
    setPC(valP);
  }

  if (icode == MRMOVQ)
  {
    setPC(valP);
  }

  if (icode == OPQ)
  {
    setPC(valP);
  }

  if (icode == JXX)
  {
    if (Cnd){
      setPC(valC);
    }
    else{
      setPC(valP);
    }
  }

  if (icode == CALL)
  {
    setPC(valC);
  }

  if (icode == RET) 
  {
    setPC(valM);
  }

  if (icode == PUSHQ)
  {
    setPC(valP);
  }

  if (icode == POPQ)
  {
    setPC(valP);
  }
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
  bool Cnd = 0;

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

/** 
 * main
 * */
int main(int argc, char **argv) {
  int stepMode = 1;
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