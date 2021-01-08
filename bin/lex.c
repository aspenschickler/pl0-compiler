#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MAX_IDEN_LEN 11
#define MAX_NUM_LEN 5

typedef enum { 
    nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, 
    multsym = 6,  slashsym = 7, oddsym = 8, eqsym = 9, neqsym = 10, lessym = 11, leqsym = 12,
    gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18,
    periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23, thensym = 24, 
    whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30,
    writesym = 31, readsym = 32, elsesym = 33
} token_type;


const char* reserved_words[] = {"const", "var", "procedure", "call", "begin", "end", "if", "then", "else", "while", "do", "read", "write", "odd"};
const char* special_symbols[] = {"+", "-", "*", "/", "(", ")", "=", ",", ".", "<", ">", ";", ":"};

int lex(char* lex_input) {

    // Open a file pointer for reading tokens.
    FILE *ifp = fopen(lex_input, "r");

    // Open file pointers for writing outputs.
    FILE *ofp = fopen("rsc/par_input.txt", "w");

    printf("Source Program:\n");

    // Token declaration.
    token_type cur_token;

    // The program decides what to do with the incoming text by feeding the characters
    // from the text file into a buffer one by one. and the end of each character feed,
    // the program will check to see if the buffer is equal to significant words, such
    // as keywords, comment starts and ends, and certain operators.
    char buf[20];
    buf[0] = '\0';
    int buf_len = 0;
    // The char c acts as a peek for the buffer, which helps with determining when var
    // names and digits start and end.
    char c;
    int EOF_buf = 0;
    c = (char)fgetc(ifp);
    printf("%c", c);

    // The following are counters to limit the size of numbers and identifiers.
    int word_len = 0;
    int num_len = 0;

    strcpy(buf, "");

    // Loop through file and act accordingly based on input.
    while (EOF_buf != EOF) {

        // If character is blank space, skip.
        if (c == ' ' || c ==  '\t' || c == '\n' || c == '\r') {
            EOF_buf = fgetc(ifp);
            if (EOF_buf != EOF)
                c = (char) EOF_buf;
            printf("%c", c);
            continue;
        }

        // If character is a letter... look for letters and digits to complete string.
        else if (isalpha(c)) {

            // Add the character to the buffer.
            buf[buf_len] = c;
            buf[buf_len + 1] = '\0';
            buf_len++;

            // Increment the word length.
            word_len++;

            // Load the next character before searching.
            EOF_buf = fgetc(ifp);
            if (EOF_buf != EOF)
                c = (char) EOF_buf;
            printf("%c", c);

            // Check for the rest of the string.
            while ((isalpha(c) || isdigit(c)) && word_len < 12 && EOF_buf != EOF) {

                // Add the character to the buffer.
                buf[buf_len] = c;
                buf[buf_len + 1] = '\0';
                buf_len++;

                // Increment the word length.
                word_len++;

                // Load the next character.
                EOF_buf = fgetc(ifp);
                if (EOF_buf != EOF) {
                    c = (char) EOF_buf;
                    printf("%c", c);
                }

            }

            // If the word is too big...
            if (word_len > 11) {
                printf("\n\nERROR: Word is too large.\n");
                return 1;
            }

            int keyword_flag = 0;

            // If the word is a keyword...
            for (int i = 0; i < 14; i++) {
                if (strcmp(reserved_words[i], buf) == 0) {
                    keyword_flag = 1;
                        
                    // Switch statement for correct token.
                    switch (i) {
                        case 0:
                            cur_token = constsym;
                            break;
                        case 1:
                            cur_token = varsym;
                            break;
                        case 2:
                            cur_token = procsym;
                            break;
                        case 3:
                            cur_token = callsym;
                            break;
                        case 4:
                            cur_token = beginsym;
                            break;
                        case 5:
                            cur_token = endsym;
                            break;
                        case 6:
                            cur_token = ifsym;
                            break;
                        case 7:
                            cur_token = thensym;
                            break;
                        case 8:
                            cur_token = elsesym;
                            break;
                        case 9:
                            cur_token = whilesym;
                            break;
                        case 10:
                            cur_token = dosym;
                            break;
                        case 11:
                            cur_token = readsym;
                            break;
                        case 12:
                            cur_token = writesym;
                            break;
                        case 13:
                            cur_token = oddsym;
                            break;
                    }
                }
            }

            if (keyword_flag == 0)
                cur_token = identsym;

            // Add the results to the output.
            if (cur_token == identsym) {
                fprintf(ofp, "%d %s ", cur_token, buf);
            }
            else {
                fprintf(ofp, "%d ", cur_token);
            }

            // Perform clean up for next round.
            strcpy(buf, "");
            buf_len = 0;
            buf[buf_len] = '\0';
            word_len = 0;
            keyword_flag = 0;
            continue;

        }

        // If character is a number...
        else if (isdigit(c)) { 

            // Add the character to the buffer.
            buf[buf_len] = c;
            buf[buf_len + 1] = '\0';
            buf_len++;

            // Increment the number length.
            num_len++;

            // Load the next character before searching.
            EOF_buf = fgetc(ifp);
            if (EOF_buf != EOF)
                c = (char) EOF_buf;
            printf("%c", c);

            // Check to see if variable starting with number is forming.
            if(isalpha(c)) {
                printf("\n\nERROR: Variable cannot start with number.\n");
                return 1;
            }

            // Check for the rest of the string.
            while (isdigit(c) && num_len < 6) {
                
                // Add the character to the buffer.
                buf[buf_len] = c;
                buf[buf_len + 1] = '\0';
                buf_len++;

                // Increment the number length.
                num_len++;

                // Load the next character.
                EOF_buf = fgetc(ifp);
                if (EOF_buf != EOF)
                    c = (char) EOF_buf;
                printf("%c", c);

            }

            // If the number is too big...
            if (num_len > 5) {
                printf("\n\nERROR: Number is too large.\n");
                return 1;
            }

            cur_token = numbersym;

            // Add the results to the output.
            fprintf(ofp, "%d %s ", cur_token, buf);

            // Perform clean up for next round.
            strcpy(buf, "");
            buf_len = 0;
            buf[buf_len] = '\0';
            num_len = 0;
            continue;

        }

        // If character is a special symbol...
        else {

            // Add the character to the buffer.
            buf[buf_len] = c;
            buf[buf_len + 1] = '\0';
            buf_len++;

            // Load the next character.
            EOF_buf = fgetc(ifp);
            if (EOF_buf != EOF) {
                c = (char) EOF_buf;
            printf("%c", c);
            }
            
            // Check for comment.
            if (buf[0] == '/' && c == '*') {
                EOF_buf = fgetc(ifp);
                if (EOF_buf != EOF)
                    c = (char) EOF_buf;
                printf("%c", c);

                while (EOF_buf != EOF) {
                    if (c == '*') {
                        EOF_buf = fgetc(ifp);
                        if (EOF_buf != EOF)
                            c = (char) EOF_buf;
                        printf("%c", c);
                        if (c == '/') {
                            break;
                        }
                    }
                    else {
                        EOF_buf = fgetc(ifp);
                        if (EOF_buf != EOF)
                            c = (char) EOF_buf;
                        printf("%c", c);
                    }
                }
                if (EOF_buf == EOF) {
                    printf("\n\nERROR: Comment never closes.\n");
                    return 1;
                }

                strcpy(buf, "");
                EOF_buf = fgetc(ifp);
                if (EOF_buf != EOF)
                    c = (char) EOF_buf;
                printf("%c", c);
                buf_len = 0;
                buf[buf_len] = '\0';
                num_len = 0;
                continue;
            }

            // Check for which symbol it is.
            int valid_sym_flag = 0;
            for (int i = 0; i < 13; i++) {
                    if (strcmp(buf, special_symbols[i]) == 0)  {
                        valid_sym_flag = 1;

                        // Switch statement for correct token.
                        switch (i) {
                            case 0:
                                cur_token = plussym;
                                break;
                            case 1:
                                cur_token = minussym;
                                break;
                            case 2:
                                cur_token = multsym;
                                break;
                            case 3:
                                cur_token = slashsym;
                                break;
                            case 4:
                                cur_token = lparentsym;
                                break;
                            case 5:
                                cur_token = rparentsym;
                                break;
                            case 6:
                                cur_token = eqsym;
                                break;
                            case 7:
                                cur_token = commasym;
                                break;
                            case 8:
                                cur_token = periodsym;
                                break;
                            case 9:
                                if (c == '=') {
                                    buf[buf_len] = c;
                                    buf[buf_len + 1] = '\0';
                                    buf_len++;
                                    cur_token = leqsym;
                                    EOF_buf = fgetc(ifp);
                                    if (EOF_buf != EOF)
                                        c = (char) EOF_buf;
                                    printf("%c", c);
                                }
                                else if (c == '>') {
                                    buf[buf_len] = c;
                                    buf[buf_len + 1] = '\0';
                                    buf_len++;
                                    cur_token = neqsym;
                                    EOF_buf = fgetc(ifp);
                                    if (EOF_buf != EOF)
                                        c = (char) EOF_buf;
                                    printf("%c", c);
                                }
                                else {
                                    cur_token = lessym;
                                }
                                break;
                            case 10:
                                if (c == '=') {
                                    buf[buf_len] = c;
                                    buf[buf_len + 1] = '\0';
                                    buf_len++;
                                    cur_token = geqsym;
                                    EOF_buf = fgetc(ifp);
                                    if (EOF_buf != EOF)
                                        c = (char) EOF_buf;
                                    printf("%c", c);
                                }
                                else {
                                    cur_token = gtrsym;
                                }
                                break;
                            case 11:
                                cur_token = semicolonsym;
                                break;
                            case 12:
                                if (c == '=') {
                                    buf[buf_len] = c;
                                    buf[buf_len + 1] = '\0';
                                    buf_len++;
                                    cur_token = becomessym;
                                    EOF_buf = fgetc(ifp);
                                    if (EOF_buf != EOF)
                                        c = (char) EOF_buf;
                                    printf("%c", c);
                                }
                                else {
                                    printf("\n\nERROR: Improper use of colon.\n");
                                    return 1;
                                }
                                break;
                            default:
                                break;
                        }
                    }
            }
            if (valid_sym_flag == 0) {
                printf("\n\nERROR: '%s' is an unrecognized symbol.\n", buf);
                return 1;
            }

            // Add the results to the output.
            fprintf(ofp, "%d ", cur_token);

            // Perform clean up for next round.
            strcpy(buf, "");
            buf_len = 0;
            buf[buf_len] = '\0';
            continue;

        }

    }
    
    // Close file streams.
    fclose(ofp);
    fclose(ifp);

    printf("\n\n");

    return 0;
}
