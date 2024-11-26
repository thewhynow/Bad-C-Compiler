#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>

#include "../error/error.h"

#ifndef Lexer_H
#define Lexer_H

typedef enum {
    Tok_Invalid,
    Tok_Integer, // <integer>
    Tok_Open_Brace, // {
    Tok_Close_Brace, // }
    Tok_Open_Paren, // (
    Tok_Close_Paren, // )
    Tok_Semicolon, // ;
    Tok_Identifier, // <identifier>
    Tok_Subtract, // -
    Tok_Add, // +
    Tok_Asterisk, // '*'
    Tok_Div, // /
    Tok_Modulo, // %
    Tok_Bitwise_Not, // ~
    Tok_Bitwise_And, // &
    Tok_Bitwise_Or, // |
    Tok_Bitwise_Xor, // ^
    Tok_ShiftLeft, // <<
    Tok_ShiftRight, // >>
    Tok_Logic_Not, // !
    Tok_Logic_And, // &&
    Tok_Logic_Or, // ||
    Tok_Equal_To, // ==
    Tok_NEqual_To, // !=
    Tok_LessThan, // <
    Tok_Less_Equal, // <=
    Tok_GreaterThan, // >
    Tok_Greater_Equal, // >=
    Tok_Comma, // ,
    Tok_Assign, // =
    Tok_Assign_Sum, // +=
    Tok_Assign_Diff, // -=
    Tok_Assign_Mult, // *=
    Tok_Assign_Div, // /=
    Tok_Assign_Mod, // %=
    Tok_Assign_LShift, // <<=
    Tok_Assign_RShift, // >>=
    Tok_Assign_BitAnd, // &=
    Tok_Assign_BitOr, // |=
    Tok_Assign_BitXor, // ^=
    Tok_Increment, // ++
    Tok_Decrement, // --
    Tok_Question, // ?
    Tok_Colon, // :
    Tok_End, // END OF TOKENS
    // Keywords
    TOK_KEYWORD_START, // all keywords will be greater than this
    Tok_KeyWord_int, // "int"
    Tok_Keyword_char, // "char"
    Tok_Keyword_unsigned, // "unsigned"
    Tok_Keyword_signed, // "signed"
    Tok_KeyWord_return, // "return"
    Tok_KeyWord_if, // "if"
    Tok_KeyWord_else, // "else"
    Tok_KeyWord_for, // "for"
    Tok_KeyWord_while, // "while"
    Tok_KeyWord_do, // "do"
    Tok_KeyWord_break, // "break"
    Tok_KeyWord_continue, // "continue"
    Tok_KeyWord_goto, // "goto"
    Tok_KeyWord_switch, // "switch"
    Tok_KeyWord_case, // "case"
    Tok_KeyWord_default, // "default"
    Tok_KeyWord_const, // "const"
    Tok_KeyWord_static, // "static"
    Tok_KeyWord_extern, // "extern"
} TokenType;

typedef struct Token Token;
struct Token {
    TokenType type;
    
    union {
        uint32_t value;
        char* str;
    };

    Token* next;

    /* for error messages */
    size_t character;
};


// returns a linked list of tokens
Token* lex(const char* buff);

void deltokens(Token* tokens);

#endif