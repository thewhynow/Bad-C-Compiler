#include "codegen.h"
void codegen_(TNode* node);
void codegenfunc(Function* func);

static uint64_t label = 0;
static FILE* file;
static char valstr[100];

extern Register getregfromargnum(uint8_t arg);

#ifdef __linux__
    #define FUNCTION_PREFIX ""
#else
    #define FUNCTION_PREFIX "_"
#endif

void codegen(Program* node, const char* f){
    file = fopen(f, "w");
    memset(valstr, 0, 100);
    #ifdef __linux__
        printf(file, ".section .text\n");
    #endif
    label = node->labels;
    for (Function* func = node->functions; func->next != NULL; func = func->next)
        codegenfunc(func);

    #ifdef __linux__
        fprintf(file, ".section .note.GNU-stack\n");
    #endif
    fclose(file);
}

void codegenfunc(Function* func){
    const char* name = func->name;
    int namelen = 0; for (; ('A' <= name[namelen] && name[namelen] <= 'Z') || ('a' <= name[namelen] && name[namelen] <= 'z'); ++namelen);

    fprintf(file,
    ".globl " FUNCTION_PREFIX "%.*s\n"
    FUNCTION_PREFIX "%.*s:\n"
    "pushq %%rbp\n"
    "movq %%rsp, %%rbp\n"
    "subq $%lu, %%rsp\n",
    namelen, name, namelen, name, func->framesize);

    for (TNode* line = func->body; line->inst != TN_NOP; line = line->next)
        codegen_(line);
    
    fprintf(file,
    "movq %%rbp, %%rsp\n"
    "popq %%rbp\n"
    "ret\n");
}

char* codegenvalue(Value val){
    memset(valstr, 0, 100);
    if (val.type == Val_Immediate)
        sprintf(valstr, "$%u", val.immediate); else
    if (val.type == Val_Register){
        char* reg_str = "";
        switch(val.reg){
            case Reg_AX:
                reg_str = "eax";
                break;
            case Reg_BX:
                reg_str = "ebx";
                break;
            case Reg_CX:
                reg_str = "ecx";
                break;
            case Reg_DX:
                reg_str = "edx";
                break;
            case Reg_SI:
                reg_str = "esi";
                break;
            case Reg_DI:
                reg_str = "edi";
                break;
            case Reg_BP:
                reg_str = "rbp";
                break;
            case Reg_SP:
                reg_str = "rsp";
                break;
            case Reg_R8:
                reg_str = "r8d";
                break;
            case Reg_R9:
                reg_str = "r9d";
                break;
            case Reg_R10:
                reg_str = "r10d";
                break;
            case Reg_R11:
                reg_str = "r11d";
                break;
            case Reg_R12:
                reg_str = "r12d";
                break;
            case Reg_R13:
                reg_str = "r13d";
                break;
            case Reg_R14:
                reg_str = "r14d";
                break;
            case Reg_R15:
                reg_str = "r15d";
                break;
        }
        sprintf(valstr, "%%%s", reg_str);
    }
    else
        sprintf(valstr, "-%lu(%%rbp)", val.offset);

    return valstr;
}

void codegenmove(Value src, size_t dst){
    if (src.type != Val_Offset)
        fprintf(file, "movl %s, -%lu(%%rbp)\n", codegenvalue(src), dst);
    else
        fprintf(file, "movl -%lu(%%rbp), %%eax\n"
                      "movl %%eax, -%lu(%%rbp)\n", src.offset, dst);
}

void codegenbinary(TNode* node, const char* inst){
    codegenmove(node->op1, node->dst);
    if (node->op2.type != Val_Offset)
        fprintf(file, "%s %s, -%lu(%%rbp)\n", inst, codegenvalue(node->op2), node->dst);
    else
        fprintf(file, "movl -%lu(%%rbp), %%eax\n"
                      "%s %%eax, -%lu(%%rbp)\n", node->op2.offset, inst, node->dst);
}

void codegencomparison(TNode* node, const char* inst){
    codegenmove(node->op1, node->dst);
    if (node->op2.type != Val_Offset)
        fprintf(file, "cmpl %s, -%lu(%%rbp)\n"
                      "%s -%lu(%%rbp)\n", codegenvalue(node->op2), node->dst, inst, node->dst);
    else
        fprintf(file, "movl -%lu(%%rbp), %%eax\n"
                      "cmpl %%eax, -%lu(%%rbp)\n"
                      "%s -%lu(%%rbp)\n", node->op2.offset, node->dst, inst, node->dst);
}

void codegenoperatorassignment(TNode* node, const char* inst){
    if (node->op1.type != Val_Offset)
        fprintf(file, "%s %s, -%lu(%%rbp)\n", inst, codegenvalue(node->op1), node->dst);
    else 
        fprintf(file, "movl -%lu(%%rbp), %%eax\n"
                      "%s %%eax, -%lu(%%rbp)\n", node->op1.offset, inst, node->dst);
}

void codegen_(TNode* node){
    switch(node->inst){
        case TN_RETURN: {
            fprintf(file, "movl %s, %%eax\n"
                          "movq %%rbp, %%rsp\n"
                          "popq %%rbp\n"
                          "ret\n", codegenvalue(node->op1));
            break;
        }

        case TN_NEGATE: {
            codegenmove(node->op1, node->dst);
            fprintf(file, "negl -%lu(%%rbp)\n", node->dst);
            break;
        }

        case TN_BITWISE_NOT: {
            codegenmove(node->op1, node->dst);
            fprintf(file, "notl -%lu(%%rbp)\n", node->dst);
            break;
        }

        case TN_LOGIC_NOT: {
            fprintf(file, "movl %s, %%eax\n"
                          "cmpl $0, %%eax\n"
                          "movl $0, %%eax\n"
                          "setz %%al\n"
                          "movb %%al, -%lu(%%rbp)\n"
                          , codegenvalue(node->op1), node->dst);
            break;
        }

        case TN_ADD: {
            codegenbinary(node, "addl");
            break;
        }

        case TN_SUB: {
            codegenbinary(node, "subl");
            break;
        }

        case TN_MUL: {
            if (node->op1.type != Val_Offset){
                codegenmove(node->op1, node->dst);
                node->op1.offset = node->dst;
            }
            fprintf(file, "movl %s, %%eax\n"
                          "imull -%lu(%%rbp), %%eax\n"
                          "movl %%eax, -%lu(%%rbp)\n", codegenvalue(node->op2), node->op1.offset, node->dst);

            break;
        }

        case TN_DIV: {
            codegenmove(node->op2, node->dst);
            fprintf(file, "movq %s, %%rax\n"
                          "cdq\n"
                          "idivl -%lu(%%rbp)\n"
                          "movl %%eax, -%lu(%%rbp)\n"
                          , codegenvalue(node->op1), node->dst, node->dst);
            break;
        }

        case TN_MOD: {
            codegenmove(node->op2, node->dst);
            fprintf(file, "movq %s, %%rax\n"
                          "cdq\n"
                          "idivl -%lu(%%rbp)\n"
                          "movl %%edx, -%lu(%%rbp)\n"
                          , codegenvalue(node->op1), node->dst, node->dst);
            break;
        }

        case TN_BITWISE_AND: {
            codegenbinary(node, "andl");
            break;
        }

        case TN_BITWISE_OR: {
            codegenbinary(node, "orl");
            break;
        }

        case TN_BITWISE_XOR: {
            codegenbinary(node, "xorl");
            break;
        }

        case TN_LEFT_SHIFT: {
            codegenbinary(node, "shll");
            break;
        }

        case TN_RIGHT_SHIFT: {
            codegenbinary(node, "shrl");
            break;
        }

        case TN_EQUAL: {
            codegencomparison(node, "sete");
            break;
        }

        case TN_NOT_EQUAL: {
            codegencomparison(node, "setne");
            break;
        }

        case TN_LESS: {
            codegencomparison(node, "setl");
            break;
        }

        case TN_GREAT: {
            codegencomparison(node, "setg");
            break;
        }
        
        case TN_LESS_EQUAL: {
            codegencomparison(node, "setle");
            break;
        }

        case TN_GREAT_EQUAL: {
            codegencomparison(node, "setge");
            break;
        }

        case TN_LOGICAL_AND: {
            uint64_t L0 = label++;
            uint64_t L1 = label++;

            codegenmove(node->op2, node->dst);
            fprintf(file, "cmpl $0, -%lu(%%rbp)\n"
                          "movl $0, -%lu(%%rbp)\n"
                          "jne L%llu\n"
                          "jmp L%llu\n"
                          "L%llu:\n", node->dst, node->dst, L0, L1, L0);
            codegenmove(node->op1, node->dst);
            fprintf(file, "cmpl $0, -%lu(%%rbp)\n"
                          "movl $0, -%lu(%%rbp)\n"
                          "setne -%lu(%%rbp)\n"
                          "L%llu:\n", node->dst, node->dst, node->dst, L1);
            break;
        }

        case TN_LOGICAL_OR: {
            uint64_t L0 = label++;
            uint64_t L1 = label++;

            codegenmove(node->op2, node->dst);
            fprintf(file, "cmpl $0, -%lu(%%rbp)\n"
                          "movl $0, -%lu(%%rbp)\n"
                          "je L%llu\n"
                          "movl $1, -%lu(%%rbp)\n"
                          "jmp L%llu\n"
                          "L%llu:\n", node->dst, node->dst, L0, node->dst, L1, L0);
            codegenmove(node->op1, node->dst);
            fprintf(file, "cmpl $0, -%lu(%%rbp)\n"
                          "movl $0, -%lu(%%rbp)\n"
                          "setne -%lu(%%rbp)\n"
                          "L%llu:\n", node->dst, node->dst, node->dst, L1);
            break;
        }

        case TN_ASSIGN: {
            return codegenmove(node->op1, node->dst);
        }

        case TN_ADD_ASSIGN: {
            return codegenoperatorassignment(node, "addl");
        }

        case TN_SUB_ASSIGN: {
            return codegenoperatorassignment(node, "subl");
        }

        case TN_MUL_ASSIGN: {
            fprintf(file, "movl %s, %%eax\n"
                          "imull -%lu(%%rbp), %%eax\n"
                          "movl %%eax, -%lu(%%rbp)\n", codegenvalue(node->op1), node->dst, node->dst);
             break;
        }

        case TN_DIV_ASSIGN: {
            if (node->op1.type != Val_Offset)
                fprintf(file, "movl -%lu(%%rbp), %%eax\n"
                              "cdq\n"
                              "movl %s, %%r10d\n"
                              "idivl %%r10d\n"
                              "movl %%eax, -%lu(%%rbp)\n", node->dst, codegenvalue(node->op1), node->dst);
            else 
                fprintf(file, "movl -%lu(%%rbp), %%eax\n"
                              "cdq\n"
                              "idivl -%lu(%%rbp)\n"
                              "movl %%eax, -%lu(%%rbp)\n", node->dst, node->op1.offset, node->dst);
            break;
        }
        
        case TN_MOD_ASSIGN: {
            if (node->op1.type != Val_Offset)
                fprintf(file, "movl -%lu(%%rbp), %%eax\n"
                              "cdq\n"
                              "movl %s, %%r10d\n"
                              "idivl %%r10d\n"
                              "movl %%edx, -%lu(%%rbp)\n", node->dst, codegenvalue(node->op1), node->dst);
            else 
                fprintf(file, "movl -%lu(%%rbp), %%eax\n"
                              "cdq\n"
                              "idivl -%lu(%%rbp)\n"
                              "movl %%edx, -%lu(%%rbp)\n", node->dst, node->op1.offset, node->dst);
            break;
        }

        case TN_AND_ASSIGN: {
            return codegenoperatorassignment(node, "andl");
        }

        case TN_ORR_ASSIGN: {
            return codegenoperatorassignment(node, "orl");
        }

        case TN_XOR_ASSIGN: {
            return codegenoperatorassignment(node, "xorl");
        }

        case TN_LSHIFT_ASSIGN: {
            return codegenoperatorassignment(node, "shll");
        }

        case TN_RSHIFT_ASSIGN: {
            return codegenoperatorassignment(node, "shrl");
        }

        case TN_MOVE: {
            return codegenmove(node->op1, node->dst);
        }

        case TN_INC: {
            fprintf(file, "incl -%lu(%%rbp)\n", node->op1.offset);
            return;
        }

        case TN_DEC: {
            fprintf(file, "decl -%lu(%%rbp)\n", node->op1.offset);
            return;
        }

        case TN_LABEL: {
            fprintf(file, "L%llu:\n", node->label);
            return;
        }

        case TN_JMP: {
            fprintf(file, "jmp L%llu\n", node->label);
            return;
        }

        case TN_IF_ELSE: {
            if (node->op1.type != Val_Offset)
                fprintf(file, "movl %s, %%eax\n"
                              "cmpl $0, %%eax\n"
                              "je L%llu\n", codegenvalue(node->op1), node->label);
            else 
                fprintf(file, "cmpl $0, -%lu(%%rbp)\n"
                              "je L%llu\n", node->op1.offset, node->label);

            return;
        }

        case TN_LOOP: {
            if (node->op1.type != Val_Offset)
                fprintf(file, "movl %s, %%eax\n"
                              "cmpl $0, %%eax\n"
                              "je L%llu\n", codegenvalue(node->op1), node->label);
            else
                fprintf(file, "cmpl $0, -%lu(%%rbp)\n"
                              "je L%llu\n", node->op1.offset, node->label);
            return;
        }

        case TN_USR_LABEL: {
            size_t len = 0;
            for (; node->str[len] != ':'; ++len);
            fprintf(file, "usr_%u_%.*s:\n", (int)len, fileno(file), node->str);
            return;
        }

        case TN_USR_JMP: {
            size_t len = 0;
            for (; node->str[len] != ';'; ++len);
            fprintf(file, "jmp usr_%u_%.*s\n", (int)len, fileno(file), node->str);
            return;
        }

        case TN_SWITCH_JMP: {
            fprintf(file, "cmpl $%u, %s\n"
                          "je switch_%u_%u_%llu\n", node->op1.immediate, codegenvalue(node->op2), fileno(file), node->op1.immediate, node->label);
            return;
        }

        case TN_SWITCH_CASE: {
            fprintf(file, "switch_%u_%u_%llu:\n", fileno(file), node->op1.immediate, node->label);
            return;
        }

        case TN_DEFAULT_CASE: {
            fprintf(file, "switch_default_%llu:\n", node->label);
            return;
        }

        case TN_DEFAULT_JMP: {
            fprintf(file, "jmp switch_default_%llu\n", node->label);
            return;
        }

        case TN_FUNCTION_CALL: {
            size_t len = 0;
            for (; node->str[len] != '('; ++len);
            fprintf(file, "call " FUNCTION_PREFIX "%.*s\n", (int)len, node->str);
            return;
        }

        case TN_FUNCTION_ARG_CALL: {
            char val1[100] = {0};
            strncpy(val1, codegenvalue(node->op1), 100);

            fprintf(file, "movl %s, %s\n", val1, codegenvalue((Value){.type = Val_Register, .reg = getregfromargnum(node->number)}));
            return;
        }

        case TN_FUNCTION_ARG_DEF: {
            fprintf(file, "movl %s, -%lu(%%rbp)\n", codegenvalue((Value){.type = Val_Register, .reg = getregfromargnum(node->number)}), (size_t)(node->number * 4));
            return;
        }
        case TN_STACK_DEALLOCATE: {
            fprintf(file, "subq %s, %%rsp\n", codegenvalue(node->op1));
            break;
        }

        case TN_PUSH: {
            fprintf(file, "pushq %s\n", codegenvalue(node->op1));
            break;
        }

        case TN_NOP: {
            break;
        }
    }
}