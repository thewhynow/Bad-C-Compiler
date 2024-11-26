#ifndef I_R_H
#define I_R_H

#include "../parser/parser.h"
/*
    Essentially linearizes the AST in place
    creates new variables for intermediates, 
    register allocation eventually
    TACI -> Three Address Coding Interface
    TN -> TACI Node
*/

/*
    <NULL>          -> NULL
    <VALUE>         -> Stack Offset or U32 imm or Register
    <DST>           -> Stack Offset or Register
    <LABEL>         -> U64 value, in ASM will look like L<U64>
    <STR>           -> char*
*/
typedef enum {              //  OP1 / SRC  OP2    DST / OP3
    TN_NOP,                 //   <NULL>,  <NULL>,  <NULL>
    TN_RETURN,              //   <VALUE>, <NULL>,  <NULL>
    TN_NEGATE,              //   <VALUE>, <NULL>,  <DST>
    TN_LOGIC_NOT,           //   <VALUE>, <NULL>,  <DST>
    TN_ADD,                 //   <VALUE>, <VALUE>, <DST>
    TN_SUB,                 //   <VALUE>, <VALUE>, <DST>
    TN_MUL,                 //   <VALUE>, <VALUE>, <DST>
    TN_DIV,                 //   <VALUE>, <VALUE>, <DST>
    TN_MOD,                 //   <VALUE>, <VALUE>, <DST>
    TN_BITWISE_NOT,         //   <VALUE>, <VALUE>, <DST>
    TN_BITWISE_AND,         //   <VALUE>, <VALUE>, <DST>
    TN_BITWISE_OR,          //   <VALUE>, <VALUE>, <DST>
    TN_BITWISE_XOR,         //   <VALUE>, <VALUE>, <DST>
    TN_LEFT_SHIFT,          //   <VALUE>, <VALUE>, <DST>
    TN_RIGHT_SHIFT,         //   <VALUE>, <VALUE>, <DST>
    TN_LOGICAL_AND,         //   <VALUE>, <VALUE>, <DST>
    TN_LOGICAL_OR,          //   <VALUE>, <VALUE>, <DST>
    TN_LESS,                //   <VALUE>, <VALUE>, <DST>
    TN_GREAT,               //   <VALUE>, <VALUE>, <DST>
    TN_LESS_EQUAL,          //   <VALUE>, <VALUE>, <DST>
    TN_GREAT_EQUAL,         //   <VALUE>, <VALUE>, <DST>
    TN_EQUAL,               //   <VALUE>, <VALUE>, <DST>
    TN_NOT_EQUAL,           //   <VALUE>, <VALUE>, <DST>
    TN_ASSIGN,              //   <VALUE>, <NULL>,  <DST>
    TN_ADD_ASSIGN,          //   <VALUE>, <NULL>,  <DST>
    TN_SUB_ASSIGN,          //   <VALUE>, <NULL>,  <DST>
    TN_MUL_ASSIGN,          //   <VALUE>, <NULL>,  <DST>
    TN_DIV_ASSIGN,          //   <VALUE>, <NULL>,  <DST>
    TN_MOD_ASSIGN,          //   <VALUE>, <NULL>,  <DST>
    TN_AND_ASSIGN,          //   <VALUE>, <NULL>,  <DST>
    TN_ORR_ASSIGN,          //   <VALUE>, <NULL>,  <DST>
    TN_XOR_ASSIGN,          //   <VALUE>, <NULL>,  <DST>
    TN_LSHIFT_ASSIGN,       //   <VALUE>, <NULL>,  <DST>
    TN_RSHIFT_ASSIGN,       //   <VALUE>, <NULL>,  <DST>
    TN_INC,                 //   <VALUE>, <NULL>,  <NULL>
    TN_DEC,                 //   <VALUE>, <NULL>,  <NULL>
    TN_MOVE,                //   <VALUE>, <NULL>,  <DST>
    TN_LABEL,               //   <NULL>,  <NULL>,  <LABEL>
    TN_JMP,                 //   <NULL>,  <NULL>,  <LABEL>
    TN_IF_ELSE,             //   <COND>,  <NULL>,  <ELSE_L>
    TN_LOOP,                //   <COND>,  <NULL>,  <END_L>
    TN_USR_LABEL,           //   <NULL>,  <NULL>,  <STR>
    TN_USR_JMP,             //   <NULL>,  <NULL>,  <STR>
    TN_SWITCH_JMP,          //   <IMM>,   <VALUE>, <ID>
    TN_SWITCH_CASE,         //   <IMM>,   <NULL>,  <ID>
    TN_DEFAULT_JMP,         //   <NULL>,  <NULL>,  <ID>
    TN_DEFAULT_CASE,        //   <NULL>,  <NULL>,  <ID>
    TN_FUNCTION_CALL,       //   <NULL>,  <NULL>,  <NAME>
    TN_FUNCTION_ARG_CALL,   //   <VALUE>, <NULL>,  <NUMBER>
    TN_FUNCTION_ARG_DEF,    //   <NULL>,  <NULL>,  <NUMBER>
    TN_PUSH,                //   <VALUE>, <NULL>,  <NULL>
    TN_POP_FDEC,            //   <NULL>,  <NULL>,  <DST>
    TN_STACK_DEALLOCATE,    //   <IMM>,   <NULL>,  <NULL>
} Instruction;

typedef struct TNode TNode;

typedef enum {
    Val_Register,
    Val_Immediate,
    Val_Offset,
} ValueType;

typedef enum {
    Reg_AX, /* return from a function - callee saved */
    Reg_BX, /* callee saved */
    Reg_CX, /* arg 4 - caller saved */
    Reg_DX, /* arg 3 - caller saved */
    Reg_SI, /* arg 2 - caller saved */
    Reg_DI, /* arg 1 - caller saved */
    Reg_BP, /* frame pointer */
    Reg_SP, /* stack pointer */
    Reg_R8, /* arg 5 - caller saved */
    Reg_R9, /* arg 6 - caller saved */
    Reg_R10, /* caller saved */
    Reg_R11, /* caller saved */
    Reg_R12, /* callee saved */
    Reg_R13, /* callee saved */
    Reg_R14, /* callee saved */
    Reg_R15, /* callee saved */
    Reg_NULL,/* for exceptions*/
} Register;

typedef struct {
    uint8_t type;
    union {
        uint32_t immediate;
        uint64_t label;
        size_t offset;
        Register reg;
    };
} Value;

struct TNode {
    Instruction inst;
    Value op1;
    Value op2;
    union {
        size_t dst;
        uint64_t label;
        char* str;
        size_t number;
    };
    TNode* next;
};

typedef struct Function Function;
struct Function {
    TNode* body;
    const char* name;
    size_t framesize;
    Function* next;
};

typedef struct {
    Function* functions;
    uint64_t labels;
} Program;

Program* linearize(ProgramNode* AST);

#define NODE_SIZE sizeof(TNode);

#endif