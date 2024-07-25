#include <assert.h>
#include <stdio.h>
#include "utils.h"


void test_cmovxx() {
    // Set condition flags for testing
    setZF(true);
    setSF(false);
    setOF(false);

    // Test cmovle (LE condition)
    int icode = CMOVXX;
    int ifun = LE;
    wordType valA = 42;
    wordType valB = 0;
    wordType valC = 0;
    wordType valE = 0;
    bool Cnd = false;

    executeStage(icode, ifun, valA, valB, valC, &valE, &Cnd);
    assert(Cnd == true); // Condition should be met
    assert(valE == valA); // valE should be set to valA

    // Test cmovl (L condition)
    ifun = L;
    setSF(true);
    setOF(false);

    executeStage(icode, ifun, valA, valB, valC, &valE, &Cnd);
    assert(Cnd == true); // Condition should be met
    assert(valE == valA); // valE should be set to valA

    // Test cmove (E condition)
    ifun = E;
    setZF(true);

    executeStage(icode, ifun, valA, valB, valC, &valE, &Cnd);
    assert(Cnd == true); // Condition should be met
    assert(valE == valA); // valE should be set to valA

    // Test cmovne (NE condition)
    ifun = NE;
    setZF(false);

    executeStage(icode, ifun, valA, valB, valC, &valE, &Cnd);
    assert(Cnd == true); // Condition should be met
    assert(valE == valA); // valE should be set to valA

    // Test cmovge (GE condition)
    ifun = GE;
    setSF(false);
    setOF(false);

    executeStage(icode, ifun, valA, valB, valC, &valE, &Cnd);
    assert(Cnd == true); // Condition should be met
    assert(valE == valA); // valE should be set to valA

    // Test cmovg (G condition)
    ifun = G;
    setSF(false);
    setOF(false);
    setZF(false);

    executeStage(icode, ifun, valA, valB, valC, &valE, &Cnd);
    assert(Cnd == true); // Condition should be met
    assert(valE == valA); // valE should be set to valA
}

int main() {
    test_cmovxx();
    printf("All tests passed.\n");
    return 0;
}
