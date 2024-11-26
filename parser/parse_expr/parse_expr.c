#include "parse_expr.h"

Node* parseTerm();
void advanceToken();
extern Token* currentToken;
extern void advanceToken();
Node* parseExpr12();

Node* parseExpr(){
    return parseExpr12();
}

Node* parseExpr1(){
    Node* result = parseTerm();
    while (true){
        switch(currentToken->type){
            case Tok_Add: {
                advanceToken();
                AddNode* addNode = malloc(sizeof(AddNode)); addNode->type = Nod_Add;
                addNode->a = result;
                addNode->b = parseTerm();
                result = (Node*)addNode;
                break;
            }

            case Tok_Subtract: {
                advanceToken();
                SubNode* subNode = malloc(sizeof(SubNode)); subNode->type = Nod_Sub;
                subNode->a = result;
                subNode->b = parseTerm();
                result = (Node*)subNode;
                break;
            }

            default:
                return result;
        }
    }
}

Node* parseExpr2(){
    Node* result = parseExpr1();
    while (true){
        switch(currentToken->type){
            case Tok_ShiftLeft: {
                advanceToken();
                LShiftNode* node = malloc(sizeof(LShiftNode)); node->type = Nod_LShift;
                node->a = result;
                node->b = parseExpr1();
                result = (Node*)node;
                break;
            }

            case Tok_ShiftRight: {
                advanceToken();
                RShiftNode* node = malloc(sizeof(RShiftNode)); node->type = Nod_RShift;
                node->a = result;
                node->b = parseExpr1();
                result = (Node*)node;
                break;
            }

            default: 
                return result;
        }
    }
}

Node* parseExpr3(){
    Node* result = parseExpr2();
    while(true){
        switch(currentToken->type){
            case Tok_LessThan: {
                advanceToken();
                LessThanNode* node = malloc(sizeof(LessThanNode)); node->type = Nod_LessThan;
                node->a = result;
                node->b = parseExpr2();
                result = (Node*)node;
                break;
            }
            case Tok_Less_Equal: {
                advanceToken();
                LessThan_EqualNode* node = malloc(sizeof(LessThan_EqualNode)); node->type = Nod_LessThan_Equal;
                node->a = result;
                node->b = parseExpr2();
                result = (Node*)node;
                break;
            }
            case Tok_GreaterThan: {
                advanceToken();
                GreaterThanNode* node = malloc(sizeof(GreaterThanNode)); node->type = Nod_GreaterThan;
                node->a = result;
                node->b = parseExpr2();
                result = (Node*)node;
                break;
            }
            case Tok_Greater_Equal: {
                advanceToken();
                GreaterThan_EqualNode* node = malloc(sizeof(GreaterThan_EqualNode)); node->type = Nod_GreaterThan_Equal;
                node->a = result;
                node->b = parseExpr2();
                result = (Node*)node;
                break;
            }
            default:
                return result;
        }
    }
}

Node* parseExpr4(){
    Node* result = parseExpr3();
    while (true){
        switch(currentToken->type){
            case Tok_Equal_To: {
                advanceToken();
                EqualToNode* node = malloc(sizeof(EqualToNode)); node->type = Nod_EqualTo;
                node->a = result;
                node->b = parseExpr3();
                result = (Node*)node;
                break;
            }
            case Tok_NEqual_To: {
                advanceToken();
                NEqualToNode* node = malloc(sizeof(NEqualToNode)); node->type = Nod_NEqualTo;
                node->a = result;
                node->b = parseExpr3();
                result = (Node*)node;
                break;
            }
            default:
                return result;
        }
    }
}

Node* parseExpr5(){
    Node* result = parseExpr4();
    while (true){
        switch(currentToken->type){
            case Tok_Bitwise_And: {
                advanceToken();
                BitwiseAndNode* node = malloc(sizeof(BitwiseAndNode)); node->type = Nod_Bitwise_And;
                node->a = result;
                node->b = parseExpr4();
                result = (Node*)node;
                break;
            }

            default: 
                return result;
        }
    }
}

Node* parseExpr6(){
    Node* result = parseExpr5();
    while (true){
        switch(currentToken->type){
            case Tok_Bitwise_Xor: {
                advanceToken();
                BitwiseXorNode* node = malloc(sizeof(BitwiseXorNode)); node->type = Nod_Bitwise_Xor;
                node->a = result;
                node->b = parseExpr5();
                result = (Node*)node;
                break;
            }

            default: 
                return result;
        }
    }
}

Node* parseExpr7(){
    Node* result = parseExpr6();
    while (true){
        switch(currentToken->type){
            case Tok_Bitwise_Or: {
                advanceToken();
                BitwiseOrNode* node = malloc(sizeof(BitwiseOrNode)); node->type = Nod_Bitwise_Or;
                node->a = result;
                node->b = parseExpr6();
                result = (Node*)node;
                break;
            }

            default: 
                return result;
        }
    }
}

Node* parseExpr8(){
    Node* result = parseExpr7();
    while (true){
        switch(currentToken->type){
            case Tok_Logic_And: {
                advanceToken();
                LogicAndNode* node = malloc(sizeof(LogicAndNode)); node->type = Nod_LogicalAnd;
                node->a = result;
                node->b = parseExpr7();
                result = (Node*)node;
                break;
            }

            default: 
                return result;
        }
    }
}

Node* parseExpr9(){
    Node* result = parseExpr8();
    while (true){
        switch(currentToken->type){
            case Tok_Logic_Or: {
                advanceToken();
                LogicOrNode* node = malloc(sizeof(LogicOrNode)); node->type = Nod_LogicalOr;
                node->a = result;
                node->b = parseExpr8();
                result = (Node*)node;
                break;
            }

            default: 
                return result;
        }
    }
}

// ternary
Node* parseExpr10(){
    Node* result = parseExpr9();
    while (true){
        switch(currentToken->type){
            case Tok_Question: {
                advanceToken();
                TernaryNode* node = malloc(sizeof(TernaryNode)); node->type = Nod_Ternary;
                node->condition = result;
                /* refer to https://en.cppreference.com/w/c/language/operator_precedence note 3 */
                node->ifBlock = parseExpr();
                advanceToken();
                node->elseBlock = parseExpr9();

                result = (Node*) node;
                break;
            }
            default: 
                return result;
        }
    }
}

// assignment
Node* parseExpr11(){
    Node* result = parseExpr10();
    while (true){
        switch(currentToken->type){
            case Tok_Assign: {
                if (result->type == Nod_Var_Access){
                    VarAccessNode* varNode = (VarAccessNode*)result;
                    AssignNode* node = malloc(sizeof(AssignNode)); node->type = Nod_Assign;
                    
                    // if (!varNode->mutable){
                    //     printf("constant variables cannot be modified:\n");
                    //     exit(-1);
                    // }
                    
                    node->offset = varNode->offset;
                    advanceToken();
                    node->value = parseExpr10();
                    result = (Node*)node;
                    break;
                }
            }

            case Tok_Assign_Sum: {
                if (result->type == Nod_Var_Access){
                    VarAccessNode* varNode = (VarAccessNode*)result;
                    AssignAddNode* node = malloc(sizeof(AssignAddNode)); node->type = Nod_Add_Assign;
                    node->offset = varNode->offset;
                    advanceToken();
                    node->value = parseExpr10();
                    result = (Node*)node;
                    break;
                }
            }

            case Tok_Assign_Diff: {
                if (result->type == Nod_Var_Access){
                    VarAccessNode* varNode = (VarAccessNode*)result;
                    AssignSubNode* node = malloc(sizeof(AssignSubNode)); node->type = Nod_Sub_Assign;
                    node->offset = varNode->offset;
                    advanceToken();
                    node->value = parseExpr10();
                    result = (Node*)node;
                    break;
                }
            }

            case Tok_Assign_Mult: {
                if (result->type == Nod_Var_Access){
                    VarAccessNode* varNode = (VarAccessNode*)result;
                    AssignMulNode* node = malloc(sizeof(AssignMulNode)); node->type = Nod_Mul_Assign;
                    node->offset = varNode->offset;
                    advanceToken();
                    node->value = parseExpr10();
                    result = (Node*)node;
                    break;
                }
            }
            
            case Tok_Assign_Div: {
                if (result->type == Nod_Var_Access){
                    VarAccessNode* varNode = (VarAccessNode*)result;
                    AssignDivNode* node = malloc(sizeof(AssignDivNode)); node->type = Nod_Div_Assign;
                    node->offset = varNode->offset;
                    advanceToken();
                    node->value = parseExpr10();
                    result = (Node*)node;
                    break;
                }
            }

            case Tok_Assign_Mod: {
                if (result->type == Nod_Var_Access){
                    VarAccessNode* varNode = (VarAccessNode*)result;
                    AssignModNode* node = malloc(sizeof(AssignModNode)); node->type = Nod_Mod_Assign;
                    node->offset = varNode->offset;
                    advanceToken();
                    node->value = parseExpr10();
                    result = (Node*)node;
                    break;
                }
            }

            case Tok_Assign_LShift: {
                if (result->type == Nod_Var_Access){
                    VarAccessNode* varNode = (VarAccessNode*)result;
                    AssignLShiftNode* node = malloc(sizeof(AssignLShiftNode)); node->type = Nod_LShift_Assign;
                    node->offset = varNode->offset;
                    advanceToken();
                    node->value = parseExpr10();
                    result = (Node*)node;
                    break;
                }
            }
            
            case Tok_Assign_RShift: {
                if (result->type == Nod_Var_Access){
                    VarAccessNode* varNode = (VarAccessNode*)result;
                    AssignRShiftNode* node = malloc(sizeof(AssignRShiftNode)); node->type = Nod_RShift_Assign;
                    node->offset = varNode->offset;
                    advanceToken();
                    node->value = parseExpr10();
                    result = (Node*)node;
                    break;
                }
            }

            case Tok_Assign_BitAnd: {
                if (result->type == Nod_Var_Access){
                    VarAccessNode* varNode = (VarAccessNode*)result;
                    AssignBitAndNode* node = malloc(sizeof(AssignBitAndNode)); node->type = Nod_BitAnd_Assign;
                    node->offset = varNode->offset;
                    advanceToken();
                    node->value = parseExpr10();
                    result = (Node*)node;
                    break;
                }
            }

            case Tok_Assign_BitOr: {
                if (result->type == Nod_Var_Access){
                    VarAccessNode* varNode = (VarAccessNode*)result;
                    AssignBitOrNode* node = malloc(sizeof(AssignBitOrNode)); node->type = Nod_BitOr_Assign;
                    node->offset = varNode->offset;
                    advanceToken();
                    node->value = parseExpr10();
                    result = (Node*)node;
                    break;
                }
            }

            case Tok_Assign_BitXor: {
                if (result->type == Nod_Var_Access){
                    VarAccessNode* varNode = (VarAccessNode*)result;
                    AssignBitXorNode* node = malloc(sizeof(AssignBitXorNode)); node->type = Nod_BitXor_Assign;
                    node->offset = varNode->offset;
                    advanceToken();
                    node->value = parseExpr10();
                    result = (Node*)node;
                    break;
                }
            }

            default: 
                return result;
        }
    }
}

Node* parseExpr12(){
    Node* result = parseExpr11();
    while (true){
        switch(currentToken->type){
            /* i would implement this but it would be a pain in the ___ to get it to work with function calls */
            /* ill keep the code here, which kinda works, for now */
            /*
            case Tok_Comma: {
                advanceToken();
                CommaExpression* node = malloc(sizeof(CommaExpression)); node->type = Nod_Comma_Expr;
                node->a = result;
                node->b = parseExpr11();
                result = (Node*)node;
                break;
            }
            */
            default: 
                return result;
        }
    }
}