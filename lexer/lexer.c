#include "lexer.h"
// returns a linked list of tokens



Token* lex(const char* buff){
    uint64_t i = 0;
    Token* head = malloc(sizeof(Token));
    Token* currentToken = head;

    for(; buff[i]; ++i){
        char c = buff[i];
        switch(buff[i]){
            case '{':
                currentToken->type = Tok_Open_Brace;
                break;
            case '}':
                currentToken->type = Tok_Close_Brace;
                break;
            case '(':
                currentToken->type = Tok_Open_Paren;
                break;
            case ')':
                currentToken->type = Tok_Close_Paren;
                break;
            case ';':
                currentToken->type = Tok_Semicolon;
                break;
            case '-':
                if (buff[i + 1] == '='){
                    currentToken->type = Tok_Assign_Diff;
                    ++i;
                }
                if (buff[i + 1] == '-'){
                    currentToken->type = Tok_Decrement;
                    ++i;
                }
                else
                    currentToken->type = Tok_Subtract;
                break;
            case '+':
                if (buff[i + 1] == '='){
                    currentToken->type = Tok_Assign_Sum;
                    ++i;
                }
                if (buff[i + 1] == '+'){
                    currentToken->type = Tok_Increment;
                    ++i;
                }
                else
                    currentToken->type = Tok_Add;
                break;
            case '*':
                if (buff[i + 1] == '='){
                    currentToken->type = Tok_Assign_Mult;
                    ++i;
                }
                else
                    currentToken->type = Tok_Asterisk;
                break;
            case '%':
                if (buff[i + 1] == '='){
                    currentToken->type = Tok_Assign_Mod;
                    ++i;
                }
                else
                    currentToken->type = Tok_Modulo;
                break;
            case '/':
                if (buff[i + 1] == '='){
                    currentToken->type = Tok_Assign_Div;
                    ++i;
                }
                else
                    currentToken->type = Tok_Div;
                break;
            case '~':
                currentToken->type = Tok_Bitwise_Not;
                break;
            case '!':
                if (buff[i + 1] == '='){
                    currentToken->type = Tok_NEqual_To;
                    ++i;
                    break;
                } else {
                    currentToken->type = Tok_Logic_Not;
                    break;
                }
            case '&':
                if (buff[i + 1] == '&'){
                    currentToken->type = Tok_Logic_And;
                    ++i;
                    break;
                } else
                if (buff[i + 1] == '='){
                    currentToken->type = Tok_Assign_BitAnd;
                    ++i;
                    break;
                }
                else {
                    currentToken->type = Tok_Bitwise_And;
                    break;
                }
            case '|':
                if (buff[i + 1] == '|'){
                    currentToken->type = Tok_Logic_Or;
                    ++i;
                    break;
                } else
                if (buff[i + 1] == '='){
                    currentToken->type = Tok_Assign_BitOr;
                    ++i;
                    break;
                }
                else {
                    currentToken->type = Tok_Bitwise_Or;
                    break;
                }
            case '^':
                if (buff[i + 1] == '='){
                    currentToken->type = Tok_Assign_BitXor;
                    ++i;
                }
                else
                    currentToken->type = Tok_Bitwise_Xor;
                break;
            case '=':
                if (buff[i + 1] == '='){
                    currentToken->type = Tok_Equal_To;
                    ++i;
                    break;
                }
                else {
                    currentToken->type = Tok_Assign;
                    break;
                }
            case '<':
                if (buff[i + 1] == '='){
                    currentToken->type = Tok_Less_Equal;
                    ++i;
                    break;
                } else 
                if (buff[i + 1] == '<'){
                    if (buff[i + 2] == '='){
                        currentToken->type = Tok_Assign_LShift;
                        i += 2;
                    } 
                    else {
                        currentToken->type = Tok_ShiftLeft;
                        ++i;
                    }
                    break;
                } else {
                    currentToken->type = Tok_LessThan;
                    break;
                }
            case '>':
                if (buff[i + 1] == '='){
                    currentToken->type = Tok_Greater_Equal;
                    ++i;
                    break;
                } else 
                if (buff[i + 1] == '>'){
                    if (buff[i + 2] == '='){
                        currentToken->type = Tok_Assign_RShift;
                        i += 2;
                    }
                    else {
                        currentToken->type = Tok_ShiftRight;
                        ++i;
                    }
                    break;
                } else {
                    currentToken->type = Tok_GreaterThan;
                    break;
                }
            case ',':
                currentToken->type = Tok_Comma;
                break;
            case '?':
                currentToken->type = Tok_Question;
                break;
            case ':':
                currentToken->type = Tok_Colon;
                break;
            case '\'':
                currentToken->type = Tok_Integer;
                if (buff[i + 1] == '\\'){
                    switch(buff[i + 2]){
                        case 'n':
                            currentToken->value = '\n';
                            break;
                        case 't':
                            currentToken->value = '\t';
                            break;
                        case '0':
                            currentToken->value = '\0';
                            break;
                        case '\\':
                            currentToken->value = '\\';
                            break;
                    }
                    i += 3;
                } 
                else {
                    currentToken->value = buff[i + 1];
                    i += 2;
                }
                break;
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                currentToken->type = Tok_Integer;
                currentToken->value = atoi(buff + i);
                if (currentToken->value != 0)
                    i += log10(currentToken->value);
                break;

            case ' ': case '\t': case '\n':
                continue;

            default: // a keyword or identifier ... or some bullshit
                if (('A' <= buff[i] && buff[i] <= 'Z') || ('a' <= buff[i] && buff[i] <= 'z')){
                    if (!strncmp(buff + i, "int", 3)){
                        currentToken->type = Tok_KeyWord_int;
                        i += 2;
                    } else
                    if (!strncmp(buff + i, "return", 6)){
                        currentToken->type = Tok_KeyWord_return;
                        i += 5;
                    } else
                    if (!strncmp(buff + i, "if", 2)){
                        currentToken->type = Tok_KeyWord_if;
                        ++i;
                    } else
                    if (!strncmp(buff + i, "else", 4)){
                        currentToken->type = Tok_KeyWord_else;
                        i += 3;
                    } else
                    if (!strncmp(buff + i, "for", 3)){
                        currentToken->type = Tok_KeyWord_for;
                        i += 2;
                    } else
                    if (!strncmp(buff + i, "while", 5)){
                        currentToken->type = Tok_KeyWord_while;
                        i += 4;
                    } else
                    if (!strncmp(buff + i, "do", 2)){
                        currentToken->type = Tok_KeyWord_do;
                        ++i;
                    } else
                    if (!strncmp(buff + i, "break", 5)){
                        currentToken->type = Tok_KeyWord_break;
                        i += 4;
                    } else
                    if (!strncmp(buff + i, "continue", 8)){
                        currentToken->type = Tok_KeyWord_continue;
                        i += 7;
                    } else
                    if (!strncmp(buff + i, "goto", 4)){
                        currentToken->type = Tok_KeyWord_goto;
                        i += 3;
                    } else
                    if (!strncmp(buff + i, "switch", 6)){
                        currentToken->type = Tok_KeyWord_switch;
                        i += 5;
                    } else
                    if (!strncmp(buff + i, "case", 4)){
                        currentToken->type = Tok_KeyWord_case;
                        i += 3;
                    } else
                    if (!strncmp(buff + i, "default", 7)){
                        currentToken->type = Tok_KeyWord_default;
                        i += 6;
                    } else
                    if (!strncmp(buff + i, "const", 5)){
                        currentToken->type = Tok_KeyWord_const;
                        i += 4;
                    } else
                    if (!strncmp(buff + i, "static", 6)){
                        currentToken->type = Tok_KeyWord_static;
                        i += 5;
                    } else
                    if (!strncmp(buff + i, "extern", 6)){
                        currentToken->type = Tok_KeyWord_extern;
                        i += 5;
                    }
                    
                    
                    else {
                        currentToken->type = Tok_Identifier;
                        currentToken->str = (char*)(buff + i);
                        
                        for (; ('A' <= buff[i] && buff[i] <= 'Z') || ('a' <= buff[i] && buff[i] <= 'z') ; ++i);
                        --i;
                    }
                }
                else{
                    add_error(UNEXPECTED_CHAR, getrow(buff, i), getcol(buff, i), 1);
                    currentToken->type = Tok_Invalid;
                }
        }

        currentToken->character = i;
        currentToken->next = malloc(sizeof(Token));
        currentToken = currentToken->next;
    }

    currentToken->type = Tok_End;

    return head;
}

void deltokens(Token* token){
    if (token->type == Tok_End)
        free(token);
    else{
        deltokens(token->next);
        free(token);
    }
}