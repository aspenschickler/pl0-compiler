#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MAX_STACK_HEIGHT 23
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

int base(int l, int base, int * stack);
void ERROR_StackOverflow();

struct Instruction {
    int op;
    int r;
    int l;
    int m;
} instruction;

int vm (char* vm_input) {
    // Initial values
    
    // Stack and code registers
    int BP = 0;
    int GP = -1;
    int SP = MAX_STACK_HEIGHT;
    int PC = 0;
    struct Instruction IR;
    IR.op = 0;
    IR.r = 0;
    IR.l = 0;
    IR.m = 0;
    int RF[8] = {0};
    int stack[MAX_STACK_HEIGHT] = {0};
    struct Instruction * instructionSet = calloc(MAX_CODE_LENGTH, sizeof(struct Instruction));

    // File I/O
    FILE *ifp = fopen(vm_input, "r");
    FILE *ofp = fopen("rsc/output1.txt", "w");
    FILE *ofp2 = fopen("rsc/output2.txt", "w");

    // Halt flag
    int halt = 1;

    // Read in code to instruction set and reformat for output
    char opcode[4];
    fprintf(ofp, "Line\tOP\tR\tL\tM\n");
    for (int i = 0; i < MAX_CODE_LENGTH; i++) {
        fscanf(ifp, "%d %d %d %d", &instructionSet[i].op, &instructionSet[i].r, &instructionSet[i].l, &instructionSet[i].m);
        // Writing to file
        switch (instructionSet[i].op) {
            case 1:
                strcpy(opcode, "lit");
                break;
            case 2:
                strcpy(opcode, "rtn");
                break;
            case 3:
                strcpy(opcode, "lod");
                break;
            case 4:
                strcpy(opcode, "sto");
                break;
            case 5:
                strcpy(opcode, "cal");
                break;
            case 6:
                strcpy(opcode, "inc");
                break;
            case 7:
                strcpy(opcode, "jmp");
                break;
            case 8:
                strcpy(opcode, "jpc");
                break;
            case 9:
            case 10:
            case 11:
                strcpy(opcode, "sio");
                break;
            case 12:
                strcpy(opcode, "neg");
                break;
            case 13:
                strcpy(opcode, "add");
                break;
            case 14:
                strcpy(opcode, "sub");
                break;
            case 15:
                strcpy(opcode, "mul");
                break;
            case 16:
                strcpy(opcode, "div");
                break;
            case 17:
                strcpy(opcode, "odd");
                break;
            case 18:
                strcpy(opcode, "mod");
                break;
            case 19:
                strcpy(opcode, "eql");
                break;
            case 20:
                strcpy(opcode, "neq");
                break;
            case 21:
                strcpy(opcode, "lss");
                break;
            case 22:
                strcpy(opcode, "leq");
                break;
            case 23:
                strcpy(opcode, "gtr");
                break;
            case 24:
                strcpy(opcode, "geq");
                break;
        }
        fprintf(ofp, "%d\t%s\t%d\t%d\t%d\n", i, opcode, instructionSet[i].r, instructionSet[i].l, instructionSet[i].m);
        if (instructionSet[i].op == 11)
            break;
    }

    // Writing headers to output file
    fprintf(ofp2, "OP\tR\tL\tM\tRF\t\t\tGP\tPC\tBP\tSP\tDATA STACK\n");

    // Main while loop will imitate the instruction cycle
    while (halt) {




        // Printing virtual machine values
        fprintf(ofp2, "%d\t%d\t%d\t%d\t", IR.op, IR.r, IR.l, IR.m);
        for (int i = 0; i < 8; i++) {
            fprintf(ofp2, "%d ", RF[i]);
        }
        fprintf(ofp2, "\t");
        fprintf(ofp2, "%d\t", GP);
        fprintf(ofp2, "%d\t", PC);
        fprintf(ofp2, "%d\t", BP);
        fprintf(ofp2, "%d\t", SP);
        for (int i = 0; i < MAX_STACK_HEIGHT; i++) {
            fprintf(ofp2, "%d ", stack[i]);
        }
        fprintf(ofp2, "\n");

        // Fetch
        IR.op = instructionSet[PC].op;
        IR.r = instructionSet[PC].r;
        IR.l = instructionSet[PC].l;
        IR.m = instructionSet[PC].m;   
        PC += 1;

        // Execute
        switch (IR.op) {
            case 1: // LIT R, 0, M
                RF[IR.r] = IR.m;
                break;
            case 2: // RTN 0, 0, 0
                SP = BP + 1;
                BP = stack[SP-3];
                PC = stack[SP-4];
                break;
            case 3: // LOD R, L, M
                if (base(IR.l, BP, stack) == 0)
                    RF[IR.r] = stack[base(IR.l, BP, stack) + IR.m];
                else
                    RF[IR.r] = stack[base(IR.l, BP, stack) - IR.m];
                break;
            case 4: // STO R, L, M
                if (base(IR.l, BP, stack) == 0) {
                    stack[base(IR.l, BP, stack) + IR.m] = RF[IR.r];
                }
                else {
                    stack[base(IR.l, BP, stack) - IR.m] = RF[IR.r];
                }
                break;
            case 5: // CAL 0, L, M
                if (SP-4 <= GP) {
                    ERROR_StackOverflow();
                    halt = 0;
                }
                stack[SP-1] = 0;
                stack[SP-2] = base(IR.l, BP, stack);
                stack[SP-3] = BP;
                stack[SP-4] = PC;
                BP = SP-1;
                PC = IR.m;
                break;
            case 6: // INC 0, 0, M
                if (SP-IR.m <= GP) {
                    ERROR_StackOverflow();
                    halt = 0;
                }
                if (BP == 0)
                    GP = GP+IR.m;
                else
                    SP = SP-IR.m;
                break;
            case 7: // JMP 0, 0, M
                PC = IR.m;
                break;
            case 8: // JPC R, 0, M
                if (RF[IR.r] == 0)
                    PC = IR.m;
                break;
            case 9: // SIO R, 0, 1
                printf("Console$ %d\n", RF[IR.r]);
                break;
            case 10: // SIO R, 0, 2
                printf("Console$ ");
                scanf(" %d\n", &RF[IR.r]);
                break;
            case 11: // SIO 0, 0, 3
                halt = 0;
                break;
            case 12: // NEG
                RF[IR.r] = -RF[IR.l];
                break;
            case 13: // ADD
                RF[IR.r] = RF[IR.l] + RF[IR.m];
                break;
            case 14: // SUB
                RF[IR.r] = RF[IR.l] - RF[IR.m];
                break;
            case 15: // MUL
                RF[IR.r] = RF[IR.l] * RF[IR.m];
                break;
            case 16: // DIV
                RF[IR.r] = RF[IR.l] / RF[IR.m];
                break;
            case 17: // ODD
                RF[IR.r] = RF[IR.r] % 2;
                break;
            case 18: // MOD
                RF[IR.r] = RF[IR.l] % RF[IR.m];
                break;
            case 19: // EQL
                RF[IR.r] = RF[IR.l] == RF[IR.m];
                break;
            case 20: // NEQ
                RF[IR.r] = RF[IR.l] != RF[IR.m];
                break;
            case 21: // LSS
                RF[IR.r] = RF[IR.l] < RF[IR.m];
                break;
            case 22: // LEQ
                RF[IR.r] = RF[IR.l] <= RF[IR.m];
                break;
            case 23: // GTR
                RF[IR.r] = RF[IR.l] > RF[IR.m];
                break;
            case 24: // GEQ
                RF[IR.r] = RF[IR.l] >= RF[IR.m];
                break;
            default:
                break;
        }
    }

    // Printing final virtual machine values
        fprintf(ofp2, "%d\t%d\t%d\t%d\t", IR.op, IR.r, IR.l, IR.m);
        for (int i = 0; i < 8; i++) {
            fprintf(ofp2, "%d ", RF[i]);
        }
        fprintf(ofp2, "\t");
        fprintf(ofp2, "%d\t", GP);
        fprintf(ofp2, "%d\t", PC);
        fprintf(ofp2, "%d\t", BP);
        fprintf(ofp2, "%d\t", SP);
        for (int i = 0; i < MAX_STACK_HEIGHT; i++) {
            fprintf(ofp2, "%d ", stack[i]);
        }
        fprintf(ofp2, "\n");

    fclose(ifp);
    fclose(ofp);
    fclose(ofp2);

    return 0;
}

int base(int l, int base, int * stack) {
    int bl;
    bl = base;
    while (l > 0) {
        bl = stack[bl - l];
        l--;
    }
    return bl;
}

void ERROR_StackOverflow() {
    printf("Stack Overflow has occured.\n");
}