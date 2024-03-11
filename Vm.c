// Emmanuel Colas
// Vm.c - A program to simulate the fetch and execute in a P machine.

#include <stdlib.h> // Libraries with pas as a global array. Needed for base function.
#include <stdio.h>
#include <string.h>
#define MAX_SIZE 500
int pas[MAX_SIZE];

int base(int BP, int L) // Base function so that we can go to different activation records in our stack.
{
    int arb = BP; // arb = activation record base
    while (L > 0) // find base L levels down
    {
        arb = pas[arb];
        L--;
    }
    return arb;
}

int main()
{
    int AR_break[500]; // AR-break is used to keep track of where to put the '|' character when printing tje array.
    for (int i = 0; i < MAX_SIZE; i++)
        pas[i] = 0; // Intializing variables and setting out arryas to be filled with zeros for the value.
    for (int i = 0; i < MAX_SIZE; i++)
        AR_break[i] = 0;

    int count = 0;
    int Halt = 1;
    int pc = 0;
    int op = 0;
    int L = pas[pc + 1];
    int M = pas[pc + 2];
    char OPR[4];

    FILE *file = fopen("test.txt", "r"); //  Opening the file and assigning the integers to the "Text" portion of the stack.
    while (fscanf(file, "%d", &pas[count]) == 1)
        count++; // Use the variable 'count' to keep track of how many integers in the file and where to start the stack.
    fclose(file);

    int bp = count;
    int sp = bp - 1;

    printf("               \t\tPC\tBP\tSP\tstack\n");
    printf("Initial values: \t%d       %d      %d\n", pc, bp, sp);
    while (Halt == 1) // while loop that will run the code until halt condition is met.
    {
        op = pas[pc]; // op, L, and M are part of the instruction register of our fetch cycle. They are assigned values at the beginnig of each cycle.
        L = pas[pc + 1];
        M = pas[pc + 2];
        pc = pc + 3;

        switch (op) // The switch case that will read the op code and do operations accordingly. 1-9 each correspond to a different ISA.
        {
        case 1: //  Pushes a constant value (literal) M onto the stack
            strcpy(OPR, "LIT");
            sp = sp + 1;
            pas[sp] = M;
            break;
        case 2:        // Operation to be performed on the data at the top of the stack. (or return from function)
            switch (M) // This switch is to decide which arthmetic function to perform. You can tell which is which by
            {          // the string copy function being used to name it.
            case 0:
                strcpy(OPR, "RTN");
                sp = bp - 1;
                bp = pas[sp + 2];
                pc = pas[sp + 3];
                break;
            case 1:
                strcpy(OPR, "ADD");
                pas[sp - 1] = pas[sp - 1] + pas[sp];
                sp = sp - 1;
                break;
            case 2:
                strcpy(OPR, "SUB");
                pas[sp - 1] = pas[sp - 1] - pas[sp];
                sp = sp - 1;
                break;
            case 3:
                strcpy(OPR, "MUL");
                pas[sp - 1] = pas[sp - 1] * pas[sp];
                sp = sp - 1;
                break;
            case 4:
                strcpy(OPR, "DIV");
                pas[sp - 1] = pas[sp - 1] / pas[sp];
                sp = sp - 1;
                break;
            case 5:
                strcpy(OPR, "EQL");
                pas[sp - 1] = pas[sp - 1] == pas[sp];
                sp = sp - 1;
                break;
            case 6:
                strcpy(OPR, "NEQ");
                pas[sp - 1] = pas[sp - 1] != pas[sp];
                sp = sp - 1;
                break;
            case 7:
                strcpy(OPR, "LSS");
                pas[sp - 1] = pas[sp - 1] < pas[sp];
                sp = sp - 1;
                break;
            case 8:
                strcpy(OPR, "LEQ");
                pas[sp - 1] = pas[sp - 1] <= pas[sp];
                sp = sp - 1;
                break;
            case 9:
                strcpy(OPR, "GTR");
                pas[sp - 1] = pas[sp - 1] > pas[sp];
                sp = sp - 1;
                break;
            case 10:
                strcpy(OPR, "GEQ");
                pas[sp - 1] = pas[sp - 1] >= pas[sp];
                sp = sp - 1;
                break;
            case 11:
                strcpy(OPR, "ODD");
                pas[sp] = pas[sp] % 2;
            }
            break;
        case 3: // Load value to top of stack from the stack location at offset M from L lexicographical levels down
            strcpy(OPR, "LOD");
            sp = sp + 1;
            pas[sp] = pas[base(bp, L) + M];
            break;
        case 4: // Store value at top of stack in the stack location at offset M from L lexicographical levels down
            strcpy(OPR, "STO");
            pas[base(bp, L) + M] = pas[sp];
            sp = sp - 1;
            break;
        case 5: // Call procedure at code index M (generates new Activation Record and PC = M)
            strcpy(OPR, "CAL");
            pas[sp + 1] = base(bp, L);
            pas[sp + 2] = bp;
            pas[sp + 3] = pc;
            bp = sp + 1;
            pc = M;
            for (int i = 0; i < MAX_SIZE; i++)
            { // uses the AR_break array to keep track of where the '|' character should be placed when printing.
                if (AR_break[i] == 0)
                {
                    AR_break[i] = bp;
                    break;
                }
            }
            break;
        case 6: // Allocate M memory words (increment SP by M). First three are reserved to Static Link (SL), Dynamic Link (DL),and Return Address (RA)
            strcpy(OPR, "INC");
            sp = sp + M;
            break;
        case 7: // Jump to instruction M (PC = M)
            strcpy(OPR, "JMP");
            pc = M;
            break;
        case 8: // Jump to instruction M if top stack element is 0
            strcpy(OPR, "JPC");
            if (pas[sp] == 0)
                pc = M;
            sp = sp - 1;
            break;
        case 9:
            strcpy(OPR, "SYS");
            if (M == 1) // Write the top stack element to the screen
            {
                printf("Output result is : %d\n", pas[sp]);
                sp = sp - 1;
            }
            else if (M == 2) // Read in input from the user and store it on top of the stack
            {
                sp = sp + 1;
                printf("Give me an interger:");
                scanf("%d", &pas[sp]);
            }
            else if (M == 3) // End of program (Set Halt flag to zero)
            {
                Halt = 0;
            }
            break;
        }
        printf("\t%s  %d\t %d  \t%d \t%d\t%d  ", OPR, L, M, pc, bp, sp); // Prints the values of each pointer

        for (int i = count; i < sp + 1; i++)
        { // for loop to print out the values in the stack
            for (int j = 0; j < MAX_SIZE; j++)
            { // second for loop that checks AR_break to print '|'
                if (i == AR_break[j])
                    printf("| ");
            }

            printf("%d ", pas[i]);
        }
        printf("\n");
    }
    return 0;
}