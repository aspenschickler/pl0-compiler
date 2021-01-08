#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

typedef enum { 
    nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, 
    multsym = 6,  slashsym = 7, oddsym = 8, eqsym = 9, neqsym = 10, lessym = 11, leqsym = 12,
    gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18,
    periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23, thensym = 24, 
    whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30,
    writesym = 31, readsym = 32, elsesym = 33
} token_type;

typedef struct {
    int kind;
    char name[20];
    int val;
    int level;
    int addr;
    int mark;
} symbol;

typedef struct {
    int op;
    int r;
    int l;
    int m;
} instruction;

#define MAX_SYMBOL_TABLE_SIZE 100
#define CODE_SIZE 500
#define MAX_LEX_LEVELS 3

void get_token(void);
void enter(int kind, char name[20], int val, int level, int addr, int mark);
void throw_error(int e_type);
void emit(int op, int r, int l, int m);

void block(int lev);
void statement(int lev);
void condition(int lev);
void expression(int lev);
void term(int lev);
void factor(int lev);

// Open file pointers for reading and writing.
FILE *ifp;
FILE *ofp;

// Setting up global buffer to read in lexemes.
int token;

symbol symbol_table[MAX_SYMBOL_TABLE_SIZE] = {0}; // Declaring the symbol table.
instruction code_table[CODE_SIZE] = {0}; // Declaring code table.

int stx = 0; // Symbol table index.
int ctx = 0; // Code table index.
int dx = 0; // Data allocation index.

int rx = -1; // Register index.
int vx = 0; // Total variables index.

// Declaring address for setting.
int cur_becomes_addr = 0;

// Delcaring temps for table insertion.
char temp_ident[20];
int temp_val = 0;
int addr_index = 4;

int proc_x = 0;
int ret_x = 0;

int proc_mark = 0;

// Driver function.
int par(char* par_input) {

    ifp = fopen(par_input, "r");
    ofp = fopen("rsc/vm_input.txt", "w");

    get_token();

    block(0);

    // End of program.
    if (token != periodsym) {
        throw_error(9); // period expected...
    }
    
    printf("No errors, program is syntactically correct.\n\n");

    emit(11, 0, 0, 3);

    // Write the finished code table to the output.

    printf("Generated code:\n");
    int i = 0;
    for (i = 0; i < ctx - 1; i++) {
        printf("%d\t%d\t%d\t%d\n", code_table[i].op, code_table[i].r, code_table[i].l, code_table[i].m);
        fprintf(ofp, "%d %d %d %d\n", code_table[i].op, code_table[i].r, code_table[i].l, code_table[i].m);
    }
    fprintf(ofp, "%d %d %d %d", code_table[i].op, code_table[i].r, code_table[i].l, code_table[i].m);
    printf("%d\t%d\t%d\t%d\n", code_table[i].op, code_table[i].r, code_table[i].l, code_table[i].m);
    printf("\n");

    printf("Symbol table:\n");
    printf("Kind\t|\t");
    for (i = 0; i < stx - 1; i++) {
        printf("%d\t", symbol_table[i].kind);
    }
    printf("%d\t", symbol_table[i].kind);
    printf("\n");
    printf("Name\t|\t");
    for (i = 0; i < stx - 1; i++) {
        printf("%s\t", symbol_table[i].name);
    }
    printf("%s\t", symbol_table[i].name);
    printf("\n");
    printf("Val\t|\t");
    for (i = 0; i < stx - 1; i++) {
        printf("%d\t", symbol_table[i].val);
    }
    printf("%d\t", symbol_table[i].val);
    printf("\n");
    printf("Lvl\t|\t");
    for (i = 0; i < stx - 1; i++) {
        printf("%d\t", symbol_table[i].level);
    }
    printf("%d\t", symbol_table[i].level);
    printf("\n");
    printf("Addr\t|\t");
    for (i = 0; i < stx - 1; i++) {
        printf("%d\t", symbol_table[i].addr);
    }
    printf("%d\t", symbol_table[i].addr);
    printf("\n");
    printf("Mark\t|\t");
    for (i = 0; i < stx - 1; i++) {
        printf("%d\t", symbol_table[i].mark);
    }
    printf("%d\t", symbol_table[i].mark);
    printf("\n");
    printf("\n");

    fclose(ofp);
    fclose(ifp);

    return 0;
}

void block(int lev) {

    if (lev > MAX_LEX_LEVELS) {
        throw_error(26);
    }

    int dx = 4;
    int start_of_proc_ctx = ctx;
    emit(7, 0, 0, -1);

    do {
        // CONSTANTS
        if (token == constsym) {
            do {
                get_token();
                if (token != identsym) {
                    throw_error(4); // missing identifier...
                }
                fscanf(ifp, "%s", temp_ident);
                get_token();
                if (token != eqsym) {
                    throw_error(3); // identifier should be followed by =...
                }
                get_token();
                if (token != numbersym) {
                    throw_error(2); // = should be followed by number...
                }
                fscanf(ifp, "%d", &temp_val);
                enter(1, temp_ident, temp_val, lev, addr_index, 0);

                strcpy(temp_ident, "");
                temp_val = 0;

                get_token();
            } while (token == commasym);
            if (token != semicolonsym) {
                throw_error(5); // declaration must end with ;...
            }
            get_token();
        }
        // IDENTIFIERS
        if (token == varsym) {
            do {
                get_token();
                if (token != identsym) {
                    throw_error(4); // missing identifier...
                }

                fscanf(ifp, "%s", temp_ident);
                enter(2, temp_ident, 0, lev, addr_index, 0);

                dx++;

                strcpy(temp_ident, "");

                get_token();

            } while (token == commasym);
            if (token != semicolonsym) {
                throw_error(5); // declaration must end with ;...
            }

            get_token();
        }
        // PROCEDURES
        while (token == procsym) {
            proc_x = proc_x + 1;
            get_token();
            if (token != identsym) {
                throw_error(4); // missing procedure declaration...
            }
            addr_index = 4;

            fscanf(ifp, "%s", temp_ident);
            enter(3, temp_ident, 0, lev, addr_index, proc_mark++);

            vx++;

            get_token();
            if (token != semicolonsym) {
                throw_error(6); // procedure declaration must end with ;...
            }
            get_token();

            block(lev + 1);

            if (token != semicolonsym) {
                throw_error(8); // no ; at the end of block...
            }
            get_token();
        }
    } while ((token == constsym) || (token == varsym) || (token == procsym));
    
    code_table[start_of_proc_ctx].m = ctx;
    emit(6, 0, 0, dx);
    statement(lev);
    if (ret_x < proc_x) {
        emit(2, 0, 0, 0);
        ret_x = ret_x + 1;
    }
    proc_mark = 0;

}

void statement(int lev) {
    if (token == identsym) {
        fscanf(ifp, "%s", temp_ident);
        get_token();
        if (token != becomessym) {

            throw_error(13); // := missing in statement...
        }
        int temp_becomes_offset, temp_becomes_level;
        int no_iden = 1;
        for (int i = stx - 1; i >= 0; i--) {
            if ((strcmp(temp_ident, symbol_table[i].name) == 0) && symbol_table[i].kind != 1) {
                temp_becomes_offset = symbol_table[i].addr;
                temp_becomes_level = symbol_table[i].level;
                no_iden = 0;
                break;
            }
        }
        if (no_iden) {
            throw_error(11);
        }
        get_token();
        expression(lev);
        emit(4, rx, lev - temp_becomes_level, temp_becomes_offset);
        rx--;
    }
    else if (token == callsym) {
        get_token();
        if (token != identsym) {
            throw_error(14); // missing identifier
        }

        fscanf(ifp, "%s", temp_ident);

        int no_iden = 1;
        for (int i = stx - 1; i >= 0; i--) {
            if ((strcmp(temp_ident, symbol_table[i].name) == 0)) {
               if (symbol_table[i].kind != 3) {
                    throw_error(26);
                }
                else {
                    printf("Current level: %d\n", lev);
                    printf("Name called: %s\n", symbol_table[i].name);
                    if (symbol_table[i].level + 1 <= lev) {
                        if (symbol_table[i].mark != 0) {
                            throw_error(11);
                        }
                    }
                    printf("LEVEL: %d\nPROCLEVEL: %d\n", lev, symbol_table[i].level);
                    emit(5, 0, lev - symbol_table[i].level, symbol_table[i].addr);
                    no_iden = 0;
                    break;
                }
            }
        }
        if (no_iden) {
            throw_error(11);
        }

        get_token();
    }
    else if (token == beginsym) {
        rx = -1;
        get_token();
        statement(lev);
        while (token == semicolonsym) {
            get_token();
            statement(lev);
        }

        if (token != endsym) {
            throw_error(17); // begin must be closed with end
        }

        int scope_stx = stx - 1;
        int break_flag = 0;
        while (break_flag == 0) {
            if (symbol_table[scope_stx].kind == 3) {
                break_flag = 1;
            }
            else {
                symbol_table[scope_stx].mark = 1;
                scope_stx = scope_stx - 1;
            }

            if (scope_stx < 0) {
                break_flag = 1;
            }
        }

        get_token();
    }
    else if (token == ifsym) {
        get_token();
        condition(lev);
        if (token != thensym) {
            throw_error(16); // if condition must be followed by then
        }
        int start_of_then = ctx;
        emit(8, rx, 0, -1);
        rx = rx - 1;
        get_token();
        statement(lev);
        if (token != elsesym) {
            code_table[start_of_then].m = ctx;
        }
        else {
            int start_of_else = ctx;
            emit(7, 0, 0, -1);
            code_table[start_of_then].m = ctx;
            get_token();
            statement(lev);
            code_table[start_of_else].m = ctx;
        }
    }
    else if (token == whilesym) {
        int ctx1 = ctx;
        get_token();
        condition(lev);
        int ctx2 = ctx;
        emit(8, rx, 0, -1);
        rx = rx - 1;
        if (token != dosym) {
            throw_error(18); // while condition must be followed by do
        }
        else {
            get_token();
        }
        statement(lev);
        emit(7, 0, 0, ctx1);
        code_table[ctx2].m = ctx;
    }
    else if (token == readsym) {
        get_token();
        if (token != identsym) {
            throw_error(4); // missing procedure declaration
        }
        fscanf(ifp, "%s", temp_ident);
        
        int no_iden = 1;
        for (int i = stx - 1; i >= 0; i--) {
            if ((strcmp(temp_ident, symbol_table[i].name) == 0)) {
                if (symbol_table[i].kind == 1) {
                    throw_error(12);
                }
                else if (symbol_table[i].kind == 2) {
                    rx = rx + 1;
                    emit(10, rx, 0, 2);
                    emit(4, rx, lev - symbol_table[i].level, symbol_table[i].addr);
                }
                emit(9, 0, 0, 1);
                rx = rx - 1;
                no_iden = 0;
                break;
            }
        }
        if (no_iden) {
            throw_error(11);
        }




        get_token();
    }
    else if (token == writesym) {
        get_token();
        if (token != identsym) {
            throw_error(4); // missing procedure declaration
        }
        fscanf(ifp, "%s", temp_ident);

        int no_iden = 1;
        for (int i = stx - 1; i >= 0; i--) {
            if ((strcmp(temp_ident, symbol_table[i].name) == 0) && symbol_table[i].kind != 1) {
                rx = rx + 1;
                if (symbol_table[i].kind == 1) {
                    emit(1, rx, 0, symbol_table[i].val);
                }
                else if (symbol_table[i].kind == 2) {
                    emit(3, rx, lev - symbol_table[i].level, symbol_table[i].addr);
                }
                emit(9, 0, 0, 1);
                rx = rx - 1;
                no_iden = 0;
                break;
            }
        }
        if (no_iden) {
            throw_error(11);
        }

        get_token();
    }
}

void condition(int lev) {
    if (token == oddsym) {
        get_token();
        expression(lev);
        emit(17, rx, 0, 0);
    }
    else {
        expression(lev);

        if (token != lessym && token != leqsym && token != gtrsym && token != geqsym && token != eqsym && token != neqsym) {
            throw_error(20); // relational operator missing in conditional statement...
        }
        if (token == becomessym) {
            throw_error(1);
        }
        int conditionOp = token;

        get_token();
        expression(lev);

        if (conditionOp == lessym) {
            rx = rx + 1;
            emit(23, rx - 2, rx - 1, rx - 2); // lessym -> GTR
            rx = rx - 2;
        }
        else if (conditionOp == leqsym) {
            rx = rx + 1;
            emit(24, rx - 2, rx - 1, rx - 2); // leqsym -> GEQ
            rx = rx - 2;
        }
        else if (conditionOp == gtrsym) {
            rx = rx + 1;
            emit(21, rx - 2, rx - 1, rx - 2); // gtrsym -> LSS
            rx = rx - 2;
        }
        else if (conditionOp == geqsym) {
            rx = rx + 1;
            emit(22, rx - 2, rx - 1, rx - 2); // geqsym -> LEQ
            rx = rx - 2;
        }
        else if (conditionOp == eqsym) {
            rx = rx + 1;
            emit(19, rx - 2, rx - 1, rx - 2);
            rx = rx - 2;
        }
        else if (conditionOp == neqsym) {
            rx = rx + 1;
            emit(20, rx - 2, rx - 1, rx - 2);
            rx = rx - 2;
        }
    }
}

void expression(int lev) {
    int addop;
    if (token == plussym || token == minussym) {
        addop = token;
        get_token();
        term(lev);
        if (addop == minussym) {
            rx = rx + 1;
            emit(12, rx - 1, rx - 1, 0);
            rx = rx - 1;
        }
    }
    else
        term(lev);
    while (token == plussym || token == minussym) {
        addop = token;
        get_token();
        term(lev);
        if (addop == plussym) {
            rx = rx + 1;
            emit(13, rx - 2, rx - 1, rx - 2);
            rx = rx - 2;
        }
        else {
            rx = rx + 1;
            emit(14, rx - 2, rx - 2,  rx - 1);
            rx = rx - 2;
        }
    }
}

void term(int lev) {
    int mulop;
    factor(lev);
    while (token == multsym || token == slashsym) {
        mulop = token;
        get_token();
        factor(lev);
        if (mulop == multsym) {
            rx = rx + 1;
            emit(15, rx - 2, rx - 1, rx - 2);
            rx = rx - 2;
        }
        else {
            rx = rx + 1;
            emit(16, rx - 2, rx - 2,  rx - 1);
            rx = rx - 2;
        }
    }
}

void factor(int lev) {
    if (token == identsym) {
        fscanf(ifp, "%s", temp_ident);

        int no_iden = 1;
        for (int i = stx - 1; i >= 0; i--) {
            if ((strcmp(temp_ident, symbol_table[i].name) == 0) && symbol_table[i].kind != 1) {
                rx = rx + 1;
                if (symbol_table[i].kind == 1) {
                    emit(1, rx, 0, symbol_table[i].val);
                }
                else if (symbol_table[i].kind == 2) {
                    emit(3, rx, lev - symbol_table[i].level, symbol_table[i].addr);
                }
                else {
                    throw_error(21);
                }
                no_iden = 0;
                break;
            }
        }
        if (no_iden) {
            throw_error(11);
        }

        get_token();
    }
    else if (token == numbersym) {
        fscanf(ifp, "%d", &temp_val);
        rx = rx + 1;
        emit(1, rx, 0, temp_val);

        get_token();
    }
    else if (token == lparentsym) {
        get_token();
        expression(lev);
        if (token != rparentsym) {
            throw_error(22); // left ( has not been closed...
        }
        get_token();
    }
    else {
        throw_error(23); // identifier, (, or number expected...
    }
}

void get_token(void) {
    // Add the new token.
    fscanf(ifp, "%d", &token);
}

void enter(int kind, char name[20], int val, int level, int addr, int mark) {
    if (stx >= 5000) {
        throw_error(26);
    }

    if (kind == 3) {
        symbol_table[stx].addr = ctx;
        addr_index = 4;
    }
    else {
        symbol_table[stx].addr = addr;
        addr_index++;
    }

    symbol_table[stx].kind = kind;
    strcpy(symbol_table[stx].name, name);
    symbol_table[stx].val = val;
    symbol_table[stx].level = level;
    symbol_table[stx].mark = mark;

    stx++;

}

void emit(int op, int r, int l, int m) {
    if (ctx >= CODE_SIZE) {
        throw_error(25);
    }
    else {
        code_table[ctx].op = op;
        code_table[ctx].r = r;
        code_table[ctx].l = l;
        code_table[ctx].m = m;
        ctx++;
    }
}

void throw_error(int e_type) {
    switch(e_type) {
        case 1:
            printf("\nError 1: Use = instead of :=.\n");
        break;
        case 2:
            printf("\nError 2: = must be followed by a number.\n");
        break;
        case 3:
            printf("\nError 3: Identifier must be followed by =.\n");
        break;
        case 4:
            printf("\nError 4: const and var must be followed by identifier.\n");
        break;
        case 5:
            printf("\nError 5: Semicolon or comma missing.\n");
        break;
        case 6:
            printf("\nError 6: Incorrect symbol after procedure delcaration.\n");
        break;
        case 7:
            printf("\nError 7: Statement expected.\n");
        break;
        case 8:
            printf("\nError 8: Incorrect symbol after statement part in block.\n");
        break;
        case 9:
            printf("\nError 9: Period expected.\n");
        break;
        case 10:
            printf("\nError 10: Semicolon between statements missing.\n");
        break;
        case 11:
            printf("\nError 11: Undeclared identifier.\n");
        break;
        case 12:
            printf("\nError 12: Assignment to constant is not allowed.\n");
        break;
        case 13:
            printf("\nError 13: Assignment operator expected.\n");
        break;
        case 16:
            printf("\nError 16: then expected.\n");
        break;
        case 17:
            printf("\nError 17: Semicolon or end expected.\n");
        break;
        case 18:
            printf("\nError 18: do expected.\n");
        break;
        case 19:
            printf("\nError 19: Incorrect symbol following statement.\n");
        break;
        case 20:
            printf("\nError 20: Relational operator expected.\n");
        break;
        case 21:
            printf("\nError 21: Expression cannot have a procedure as an identifier.\n");
        break;
        case 22:
            printf("\nError 22: Right parenthesis missing.\n");
        break;
        case 23:
            printf("\nError 23: The preceding factor cannot begin with this symbol.\n");
        break;
        case 24:
            printf("\nError 24: An expression cannot begin with this symbol.\n");
        break;
        case 25:
            printf("\nError 25: This number is too large.\n");
        break;
        case 26:
            printf("\nError 26: Cannot call anything other than a procedure.\n");
        break;
        default:
        break;
    }
    
    exit(1);
}