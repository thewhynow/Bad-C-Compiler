#include "intermediate_rep_gen.h"

static TNode* line;
static TNode* oldblock;
static Function* function;
static Program* program;
static Value val;
static uint64_t label = 0;
static uint64_t breakLabel;
static uint64_t continueLabel;
static FunctionDeclNode* currentFunction;
static size_t currentCaseID;
static bool defaultCaseDefined;
static uint16_t framesize;
static size_t argc;

#define saveLabels() uint64_t oldBreak = breakLabel; \
                     uint64_t oldContinue = continueLabel;

#define resetLabels() breakLabel = oldBreak; \
                      continueLabel = oldContinue;

#define saveCaseID() size_t oldCaseID = currentCaseID; \
                     bool oldDefaultCaseDefined = defaultCaseDefined; \
                     defaultCaseDefined = false

#define resetCaseID() currentCaseID = oldCaseID; \
                      defaultCaseDefined = oldDefaultCaseDefined

void linearize_(Node* AST);

Program* linearize(ProgramNode* AST){
    program = malloc(sizeof(Program));
    program->functions = malloc(sizeof(Function));
    program->functions->body = malloc(sizeof(TNode));
    label = program->labels;
    function = program->functions;
    line = program->functions->body;
    for (NodeList* tracker = AST->node; tracker->next != NULL; tracker = tracker->next)
        linearize_(tracker->node);

    program->labels = label;
    return program;
}

void nextline(){
    line->next = malloc(sizeof(TNode));
    line = line->next;
    line->inst = TN_NOP;
}

void nextfunc(){
    function->next = malloc(sizeof(Function));
    function = function->next;
    function->next = NULL;
}

TNode* newblock(){
    TNode* old = line;
    line = malloc(sizeof(TNode));
    return old;
}

void linearizeunary(Node* value, Instruction inst){
    linearize_(value);
    framesize += 4;
    line->inst = inst;
    line->op1  = val;
    line->dst  = framesize;
    val.type  = Val_Offset;
    val.offset = framesize;
    return nextline();
}

void linearizebinary(BinaryOpNode* node, Instruction inst){
    linearize_(node->a);
    Value a = val;    
    linearize_(node->b);
    framesize += 4;
    line->inst = inst;
    line->op1 = a;
    line->op2 = val;
    line->dst = framesize;

    val.type = Val_Offset;
    val.offset = framesize;

    return nextline();
}

void linearizeoperatorassignment(AssignNode* node, Instruction inst){
    linearize_(node->value);
    line->inst = inst;
    line->op1 = val;
    line->dst = node->offset;
    return nextline();
}

void linearizeswitchcomp(int value, Value val, size_t ID){
    line->inst = TN_SWITCH_JMP;
    line->op1.type = Val_Immediate;
    line->op1.immediate = value;
    line->op2 = val;
    line->label = ID;
    return nextline();
}

void linearizefunctioncallargsonstack(NodeList* args){
    ++argc;
    if (args->node->type != Nod_Null)
        linearizefunctioncallargsonstack(args->next);
    linearize_(args->node);
    line->inst = TN_PUSH;
    line->op1 = val;
    nextline();
}

Register getregfromargnum(uint8_t arg){
    switch(arg){
        case 1:
            return Reg_DI;
        case 2:
            return Reg_SI;
        case 3:
            return Reg_DX;
        case 4:
            return Reg_CX;
        case 5:
            return Reg_R8;
        case 6:
            return Reg_R9;
        default:
            return Reg_NULL;
    }
}

void linearize_(Node* AST){
    switch(AST->type){
        case Nod_Block: {
            BlockNode* node = (BlockNode*) AST;
            for (NodeList* tracker = node->lines; tracker->next != NULL; tracker = tracker->next)
                linearize_(tracker->node);
            return;
        }

        case Nod_Function_Decl: {
            FunctionDeclNode* node = (FunctionDeclNode*) AST;
            currentFunction = node;
            function->name = node->name;
            framesize = node->framesize;
            TNode* old = newblock();
            TNode* code = line;

            for (size_t args = 1; args <= node->argc;){
                line->inst = TN_MOVE;
                line->op1.type = Val_Register;
                line->op1.reg = getregfromargnum(args);
                line->dst = args++ * 4;
                nextline();

                if (args == 6){
                    for (; args < node->argc; ++args){
                        line->inst = TN_POP_FDEC;
                        line->op1.offset = 16 + ((args - 6) * 8);
                        line->dst = args++ * 4;
                        nextline();
                    }
                    break;
                }
            }

            linearize_(node->body);
            function->framesize = framesize;
            function->body = code;
            line = old;
            return nextfunc();
        }

        case Nod_Return: {
            ReturnNode* node = (ReturnNode*) AST;
            linearize_(node->value);
            line->inst = TN_RETURN;
            line->op1 = val;
            return nextline();
        }

        case Nod_Integer: {
            IntegerNode* node = (IntegerNode*) AST;
            val.type = Val_Immediate;
            val.immediate = node->value;
            return;
        }

        case Nod_Negate: {
            NegativeNode* node = (NegativeNode*) AST;
            return linearizeunary(node->value, TN_NEGATE);
        }

        case Nod_LogicalNot: {
            LogicNotNode* node = (LogicNotNode*) AST;
            return linearizeunary(node->value, TN_LOGIC_NOT);
        }

        case Nod_Bitwise_Not: {
            BitwiseNotNode* node = (BitwiseNotNode*) AST;
            return linearizeunary(node->value, TN_BITWISE_NOT);
        }

        case Nod_Add: {
            return linearizebinary((BinaryOpNode*)AST, TN_ADD);
        }

        case Nod_Sub: {
            return linearizebinary((BinaryOpNode*)AST, TN_SUB);
        }
        
        case Nod_Mul: {
            return linearizebinary((BinaryOpNode*)AST, TN_MUL);
        }

        case Nod_Div: {
            return linearizebinary((BinaryOpNode*)AST, TN_DIV);
        }

        case Nod_Mod: {
            return linearizebinary((BinaryOpNode*)AST, TN_MOD);
        }

        case Nod_Bitwise_And: {
            return linearizebinary((BinaryOpNode*)AST, TN_BITWISE_AND);
        }
        
        case Nod_Bitwise_Or: {
            return linearizebinary((BinaryOpNode*)AST, TN_BITWISE_OR);
        }

        case Nod_Bitwise_Xor: {
            return linearizebinary((BinaryOpNode*)AST, TN_BITWISE_XOR);
        }

        case Nod_LShift: {
            return linearizebinary((BinaryOpNode*)AST, TN_LEFT_SHIFT);
        }

        case Nod_RShift: {
            return linearizebinary((BinaryOpNode*)AST, TN_RIGHT_SHIFT);
        }

        case Nod_LessThan: {
            return linearizebinary((BinaryOpNode*)AST, TN_LESS);
        }

        case Nod_GreaterThan: {
            return linearizebinary((BinaryOpNode*)AST, TN_GREAT);
        }

        case Nod_LessThan_Equal: {
            return linearizebinary((BinaryOpNode*)AST, TN_LESS_EQUAL);
        }

        case Nod_GreaterThan_Equal: {
            return linearizebinary((BinaryOpNode*)AST, TN_GREAT_EQUAL);
        }

        case Nod_EqualTo: {
            return linearizebinary((BinaryOpNode*)AST, TN_EQUAL);
        }

        case Nod_NEqualTo: {
            return linearizebinary((BinaryOpNode*)AST, TN_NOT_EQUAL);
        }

        case Nod_LogicalAnd: {
            return linearizebinary((BinaryOpNode*)AST, TN_LOGICAL_AND);
        }

        case Nod_LogicalOr: {
            return linearizebinary((BinaryOpNode*)AST, TN_LOGICAL_OR);
        }

        case Nod_Var_Access: {
            val.type = Val_Offset;
            val.offset = ((VarAccessNode*)AST)->offset;
            return;
        }

        case Nod_Assign: {
            AssignNode* node = (AssignNode*)AST;
            linearize_(node->value);
            line->inst = TN_ASSIGN;
            line->op1  = val;
            line->dst  = node->offset;
            val.type  = Val_Offset;
            val.offset = node->offset;
            return nextline();
        }

        case Nod_Add_Assign: {
            return linearizeoperatorassignment((AssignNode*)AST, TN_ADD_ASSIGN);
        }
        
        case Nod_Sub_Assign: {
            return linearizeoperatorassignment((AssignNode*)AST, TN_SUB_ASSIGN);
        }

        case Nod_Mul_Assign: {
            return linearizeoperatorassignment((AssignNode*)AST, TN_MUL_ASSIGN);
        }

        case Nod_Div_Assign: {
            return linearizeoperatorassignment((AssignNode*)AST, TN_DIV_ASSIGN);
        }

        case Nod_Mod_Assign: {
            return linearizeoperatorassignment((AssignNode*)AST, TN_MOD_ASSIGN);
        }

        case Nod_BitAnd_Assign: {
            return linearizeoperatorassignment((AssignNode*)AST, TN_AND_ASSIGN);
        }
        
        case Nod_BitOr_Assign: {
            return linearizeoperatorassignment((AssignNode*)AST, TN_ORR_ASSIGN);
        }

        case Nod_BitXor_Assign: {
            return linearizeoperatorassignment((AssignNode*)AST, TN_XOR_ASSIGN);
        }

        case Nod_LShift_Assign: {
            return linearizeoperatorassignment((AssignNode*)AST, TN_LSHIFT_ASSIGN);
        }

        case Nod_RShift_Assign: {
            return linearizeoperatorassignment((AssignNode*)AST, TN_RSHIFT_ASSIGN);
        }

        case Nod_Pre_Inc: {
            line->inst = TN_INC;
            line->op1.type = Val_Offset;
            line->op1.offset = ((PreIncrementNode*)AST)->offset;

            val.offset = line->op1.offset;
            val.type = Val_Offset;
            return nextline();
        }
        
        case Nod_Pre_Dec: {
            line->inst = TN_DEC;
            line->op1.type = Val_Offset;
            line->op1.offset = ((PreDecrementNode*)AST)->offset;

            val.offset = line->op1.offset;
            val.type = Val_Offset;
            return nextline();
        }

        case Nod_Post_Inc: {
            PostIncrementNode* node = (PostIncrementNode*) AST;
            framesize += 4;
            line->inst = TN_MOVE;
            line->op1.type = Val_Offset;
            line->op1.offset = node->offset;
            line->dst = framesize;
            nextline();
            line->inst = TN_INC;
            line->op1.type = Val_Offset;
            line->op1.offset = node->offset;
            val.offset = framesize;
            val.type = Val_Offset;
            return nextline();
        }

        case Nod_Post_Dec: {
            PostDecrementNode* node = (PostDecrementNode*) AST;
            framesize += 4;
            line->inst = TN_MOVE;
            line->op1.type = Val_Offset;;
            line->op1.offset = node->offset;
            line->dst = framesize;
            nextline();
            line->inst = TN_DEC;
            line->op1.type = Val_Offset;;
            line->op1.offset = node->offset;
            val.offset = framesize;
            val.type = Val_Offset;;
            return nextline();
        }

        case Nod_If: {
            IfNode* node = (IfNode*) AST;
            if (node->elseBlock != NULL){
                uint64_t elseLabel = label++;
                uint64_t endLabel = label++;
                
                linearize_(node->condition);
                line->inst = TN_IF_ELSE;
                line->op1 = val;
                line->label = elseLabel;
                nextline();


                linearize_(node->ifBlock);
                
                line->inst = TN_JMP;
                line->label = endLabel;
                nextline();
                
                line->inst = TN_LABEL;
                line->label = elseLabel;
                nextline();
                linearize_(node->elseBlock);

                line->inst = TN_LABEL;
                line->label = endLabel;
            }
            else {
                uint64_t endLabel = label++;
                linearize_(node->condition);
                line->inst = TN_IF_ELSE;
                line->op1 = val;
                line->label = endLabel;
                nextline();

                linearize_(node->ifBlock);

                line->inst = TN_LABEL;
                line->label = endLabel;
            }

            return nextline();
        }

        case Nod_Ternary: {
            TernaryNode* node = (TernaryNode*) AST;
            framesize += 4;
            uint64_t elseLabel = label++;
            uint64_t endLabel = label++;

            linearize_(node->condition);
            line->inst = TN_IF_ELSE;
            line->op1 = val;
            line->label = elseLabel;
            nextline();
            
            linearize_(node->ifBlock);

            line->inst = TN_MOVE;
            line->op1 = val;
            line->dst = framesize;
            nextline();

            line->inst = TN_JMP;
            line->label = endLabel;
            nextline();

            line->inst = TN_LABEL;
            line->label = elseLabel;
            nextline();
            
            linearize_(node->elseBlock);
            
            line->inst = TN_MOVE;
            line->op1 = val;
            line->dst = framesize;
            nextline();

            line->inst = TN_LABEL;
            line->label = endLabel;

            val.offset = framesize;
            val.type = Val_Offset;;

            return nextline();
        }

        case Nod_While: {
            WhileNode* node = (WhileNode*) AST;
            
            saveLabels();

            uint64_t startLabel = label++;
            uint64_t endLabel = label++;

            breakLabel = endLabel;
            continueLabel = startLabel;

            line->inst = TN_LABEL;
            line->label = startLabel;
            nextline();

            linearize_(node->condition);

            line->inst = TN_LOOP;
            line->op1 = val;
            line->label = endLabel;
            nextline();

            linearize_(node->block);

            line->inst = TN_JMP;
            line->label = startLabel;
            nextline();

            line->inst = TN_LABEL;  
            line->label = endLabel;

            resetLabels();
            
            return nextline();
        }

        case Nod_Break: {
            line->inst = TN_JMP;
            line->label = breakLabel;
            return nextline();
        }

        case Nod_Continue: {
            line->inst = TN_JMP;
            line->label = continueLabel;
            return nextline();
        }
    
        case Nod_For: {
            ForNode* node = (ForNode*) AST;

            saveLabels();

            uint64_t startLabel = label++;
            uint64_t endLabel = label++;

            breakLabel = endLabel;
            continueLabel = startLabel;

            linearize_(node->init);

            line->inst = TN_LABEL;
            line->label = startLabel;
            nextline();

            linearize_(node->condition);

            line->inst = TN_LOOP;
            line->op1 = val;
            line->label = endLabel;
            nextline();

            linearize_(node->repeat);

            linearize_(node->body);

            line->inst = TN_JMP;
            line->label = startLabel;
            nextline();

            line->inst = TN_LABEL;
            line->label = endLabel;
            
            resetLabels();
            
            return nextline();
        }

        case Nod_DoWhile: {
            DoWhileNode* node = (DoWhileNode*) AST;
            
            saveLabels();

            uint64_t startLabel = label++;
            uint64_t endLabel = label++;

            breakLabel = endLabel;
            continueLabel = startLabel;
            
            linearize_(node->block);
            linearize_(node->condition);

            line->inst = TN_LABEL;
            line->label = startLabel;
            nextline();

            line->inst = TN_LOOP;
            line->op1 = val;
            line->label = endLabel;
            nextline();

            line->inst = TN_LABEL;
            line->label = endLabel;
            
            resetLabels();

            return nextline();
        }

        case Nod_Label: {
            LabelNode* node = (LabelNode*) AST;
            line->inst = TN_USR_LABEL;
            line->str = node->label;
            return nextline();
        }

        case Nod_GoTo: {
            GoToNode* node = (GoToNode*) AST;
            line->inst = TN_USR_JMP;
            line->str = node->label;
            return nextline();
        }

        case Nod_Switch: {
            SwitchNode* node = (SwitchNode*) AST;

            saveCaseID();
            saveLabels();
            currentCaseID = node->id;

            breakLabel = label++;

            linearize_(node->value);
            Value switchVal = val;

            for (size_t i = 0; i < node->cases.size; ++i){
                line->inst = TN_SWITCH_JMP;
                line->op1.type = Val_Immediate;
                line->op1.immediate = node->cases.buff[i];
                line->op2 = switchVal;
                line->label = currentCaseID;
                nextline();
            }

            line->inst = TN_DEFAULT_JMP;
            line->label = node->id;
            nextline();

            linearize_(node->body);

            /* implicit "default: break" */
            if (!defaultCaseDefined){
                line->inst = TN_JMP;
                line->label = breakLabel;
                nextline();
            }

            line->inst = TN_LABEL;
            line->label = breakLabel;
            
            resetLabels();
            resetCaseID();

            return nextline();
        }

        case Nod_Case: {
            CaseNode* node = (CaseNode*) AST;
            line->inst = TN_SWITCH_CASE;
            line->op1.type = Val_Immediate;
            line->op1.immediate = ((IntegerNode*)node->value)->value;
            line->label = currentCaseID;
            
            return nextline();
        }

        case Nod_Default_Case: {
            DefaultCaseNode* node = (DefaultCaseNode*) AST;
            line->inst = TN_DEFAULT_CASE;
            line->label = node->id;
            defaultCaseDefined = true;
            return nextline();
        }
 
        case Nod_Function_Call: {
            FunctionCallNode* node = (FunctionCallNode*) AST;
            NodeList* tracker = node->args;

            for (size_t args = 1; tracker->node->type != Nod_Null; tracker = tracker->next){
                linearize_(tracker->node);
                line->inst = TN_FUNCTION_ARG_CALL;
                line->op1 = val;
                line->number = args++;
                nextline();

                if (args == 6){
                    argc = 0;
                    linearizefunctioncallargsonstack(tracker);
                    line->inst = TN_STACK_DEALLOCATE;
                    line->op1.immediate = argc * 8;
                    line->op1.type = Val_Immediate;
                    break;
                }
            }

            /* call the function AFTER arguments are moved into registers */
            line->inst = TN_FUNCTION_CALL;
            line->str = node->identifier;

            val.type = Val_Register;
            val.reg = Reg_AX;

            return nextline();
        }  
    
        case Nod_Program: {
            puts("Achievement Get! How Did We Get Here?");
            exit(-1);
        }
        
        case Nod_Comma_Expr: {
            puts("This has been deprecated because it is too much of a pain in the ass to get working :)");
            exit(-1);
        }

        case Nod_Null: {
            return;
        }
    }
}