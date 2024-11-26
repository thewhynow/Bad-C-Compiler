#include "parser.h"

Node* parseFac();
Node* parseTerm();
Node* parse_declaration();

Token* currentToken = NULL;
ScopeInfo currentScope;
static size_t currentOffset;
static size_t switchID = 0;
static Node nullNode;
static IntVector cases;
static char* buff;
ScopeInfo globalScope;


void advanceToken(){
    currentToken = currentToken->next;
}

static bool strless(const char* a, const char* b){
    uint64_t len = 0; for (; ('A' <= a[len] && a[len] <= 'Z') || ('a' <= a[len] && a[len] <= 'z'); ++len);
    int res = strncmp(a, b, len);
    return res < 0;
}

static bool strgreat(const char* a, const char* b){
    uint64_t len = 0; for (; ('A' <= a[len] && a[len] <= 'Z') || ('a' <= a[len] && a[len] <= 'z'); ++len);
    int res = strncmp(a, b, len);
    return res > 0;
}

ProgramNode* parse(Token* tokens, char* buff_){
    currentToken = tokens;
    buff = buff_;

    nullNode.type = Nod_Null;
    cases = IntVector_create();

    ProgramNode* AST = malloc(sizeof(ProgramNode));
    AST->type = Nod_Program;

    globalScope = BTree_make(strgreat, strless);

    /* parse top-level statements */
    NodeList* code = malloc(sizeof(NodeList));
    NodeList* line = code;

    while (currentToken->type != Tok_End){
        line->node = parse_declaration(); 
        if (currentToken->type == Tok_Semicolon || currentToken->type == Tok_Close_Brace)
            advanceToken();
        line->next = malloc(sizeof(NodeList));
        line = line->next;
    }

    line->node = &nullNode;
    line->next = NULL;

    AST->node = code;
    
    return AST;
}

Node* parse_declaration(){
    /* parse type and storage class specifiers */

    DataType dtype;
    StorageType stype;

    if (currentToken->type == Tok_KeyWord_static || currentToken->next->type == Tok_KeyWord_static){
        advanceToken();
        advanceToken();
        stype = St_static;
    } else
    if (currentToken->type == Tok_KeyWord_extern || currentToken->next->type == Tok_KeyWord_extern){
        advanceToken();
        advanceToken();
        stype = St_extern;
    } 
    else {
        advanceToken();
        stype = St_none;
    }

    /* no other types */
    dtype = Tp_int;

    if (currentToken->type == Tok_Identifier){
        Node* result;
        /* function definition */
        if (currentToken->next->type == Tok_Open_Paren){
            currentOffset = 0;

            FunctionDeclNode* node = malloc(sizeof(FunctionDeclNode)); node->type = Nod_Function_Decl; 
            node->name = currentToken->str; advanceToken();
            node->argc = 0;

            FunctionDeclaration* symbol = malloc(sizeof(FunctionDeclaration)); symbol->decltype = FUN_DECL;
            symbol->stype = stype;

            currentScope = BTree_make(strgreat, strless);
            BTree_insert(&globalScope, node->name, (ScopeDeclaration*)symbol);

            advanceToken();

            /* parse function arguments */

            while (currentToken->type != Tok_Close_Paren){
                /* will ALWAYS be nullnode ... if syntax is being followed! */
                parseFac();
                if (currentToken->type == Tok_Comma)
                    advanceToken();
                node->argc++;
            }

            advanceToken();

            if (currentToken->type == Tok_Semicolon)
                return &nullNode;

            /* parse a factor since compound statements are factors */
            node->body = parseFac();
            node->framesize = currentOffset;

            /* have main implicitly return 0 */
            if (strncmp("main", node->name, 4) == 0){
                NodeList* line = ((BlockNode*)node->body)->lines;
                
                for (; line->next != NULL; line = line->next);
                line->node = malloc(sizeof(ReturnNode)); line->node->type = Nod_Return;
                ReturnNode* returnNode = (ReturnNode*)line->node;
                returnNode->value = malloc(sizeof(IntegerNode)); returnNode->value->type = Nod_Integer;
                ((IntegerNode*)(returnNode->value))->value = 0;

                line->next = malloc(sizeof(NodeList)); line->next->node = malloc(sizeof(Node));
                line->next->node->type = Nod_Null;
                line->next->next = NULL;
            }

            result = (Node*) node;
        }
        /* global variable definition */
        else {
            printf("Expected function declaration or definition\n");
            exit(-1);
        }

        return result;

    } 
    else {
        printf("Expected Identfier\n");
        exit(-1);
    }
}

DataType parseType(){
    switch(currentToken->type){
        case Tok_KeyWord_int:
            advanceToken();
            return Tp_int;
        
        default:
            return Tp_Null;
    }
}

Node* parseFac(){
    DataType type = parseType();

    if (type != Tp_Null){ // symbol definition
        /* function definition / declaration */
        if (currentToken->next->type != Tok_End && currentToken->next->type == Tok_Open_Paren){ 
            
        }
        else { // variable definition - dont do anything (UNLESS IT IS INITIALIZED), just create the symbol in the scope map
            currentOffset += getTSize(type);
            VarDeclaration* symbol = malloc(sizeof(VarDeclaration)); symbol->decltype = VAR_DECL;
            symbol->offset = currentOffset;
            BTree_insert(&currentScope, currentToken->str, (ScopeDeclaration*)symbol);
            advanceToken();

            // initialization, for now just treat it as a seperate assignment
            if (currentToken->type == Tok_Assign){
                advanceToken();
                AssignNode* node = malloc(sizeof(AssignNode)); node->type = Nod_Assign;
                node->offset = currentOffset;
                node->value = parseExpr();

                return (Node*)node;
            }
            else
                return &nullNode;
        }
    }

    switch(currentToken->type){
        case Tok_Integer: {
            IntegerNode* result = malloc(sizeof(IntegerNode)); result->type = Nod_Integer;
            result->value = currentToken->value;
            advanceToken();

            return (Node*)result;
        }

        case Tok_KeyWord_return: {
            ReturnNode* result = malloc(sizeof(ReturnNode)); result->type = Nod_Return;
            advanceToken();
            result->value = parseExpr();
            if (currentToken->type != Tok_Semicolon){
                add_error(EXPECTED_SEMICOLON, getrow(buff, currentToken->character), getcol(buff, currentToken->character), 0);
            }
            return (Node*)result;
        }

        case Tok_Open_Brace: {
            advanceToken();
            
            NodeList* code = malloc(sizeof(NodeList));
            NodeList* line = code;

            

            while (currentToken->type != Tok_Close_Brace){
                line->node = parseExpr(); 
                if (currentToken->type == Tok_Semicolon || currentToken->type == Tok_Close_Brace)
                    advanceToken();
                line->next = malloc(sizeof(NodeList));
                line = line->next;
            }

            line->node = &nullNode;
            line->next = NULL;

            BlockNode* result = malloc(sizeof(BlockNode)); result->type = Nod_Block;
            result->lines = code;

            return (Node*)result;
        }

        case Tok_Subtract: {
            advanceToken();
            NegativeNode* result = malloc(sizeof(NegativeNode)); result->type = Nod_Negate;
            result->value = parseFac();
            
            return (Node*)result;
        }

        case Tok_Bitwise_Not: {
            advanceToken();
            BitwiseNotNode* result = malloc(sizeof(BitwiseNotNode)); result->type = Nod_Bitwise_Not;
            result->value = parseFac();
            
            return (Node*)result;
        }

        case Tok_Logic_Not: {
            advanceToken();
            LogicNotNode* result = malloc(sizeof(LogicNotNode)); result->type = Nod_LogicalNot;
            result->value = parseFac();
            
            return (Node*)result;
        }

        case Tok_Open_Paren: {
            advanceToken();
            Node* result = parseExpr();
            advanceToken();
            return result;
        }

        case Tok_Identifier: {
            /* label definition */
            if (currentToken->next->type == Tok_Colon){
                LabelNode* node = malloc(sizeof(LabelNode)); node->type = Nod_Label;
                node->label = currentToken->str;
                advanceToken();
                advanceToken();
                return (Node*) node;
            } else
            /* function call */
            if (currentToken->next->type == Tok_Open_Paren){
                FunctionCallNode* node = malloc(sizeof(FunctionCallNode)); node->type = Nod_Function_Call;
                node->identifier = currentToken->str;
                advanceToken();
                advanceToken();
                node->args = malloc(sizeof(NodeList));
                NodeList* tracker = node->args;
                while (currentToken->type != Tok_Close_Paren){
                    tracker->node = parseExpr();
                    if (currentToken->type == Tok_Comma)
                        advanceToken();
                    tracker = tracker->next = malloc(sizeof(NodeList));
                }
                tracker->node = &nullNode;
                tracker->next = NULL;

                advanceToken();

                return (Node*) node;
            }
            else {  
                VarAccessNode* node = malloc(sizeof(VarAccessNode)); node->type = Nod_Var_Access;
                VarDeclaration* symbol = (VarDeclaration*)*BTree_find(&currentScope, currentToken->str);
                node->offset = symbol->offset;
                advanceToken();

                if (currentToken->type == Tok_Increment){
                    PostIncrementNode* newnode = malloc(sizeof(PreIncrementNode)); newnode->type = Nod_Post_Inc;
                    advanceToken();
                    newnode->offset = node->offset;
                    free(node);

                    return (Node*)newnode;
                }
                if (currentToken->type == Tok_Decrement){
                    PostDecrementNode* newnode = malloc(sizeof(PostDecrementNode)); newnode->type = Nod_Post_Dec;
                    advanceToken();
                    newnode->offset = node->offset;
                    free(node);

                    return (Node*)newnode;
                }

                return (Node*)node;
            }
        }

        case Tok_Increment: {
            PreIncrementNode* node = malloc(sizeof(PreIncrementNode)); node->type = Nod_Pre_Inc;
            advanceToken();
            VarDeclaration* symbol = (VarDeclaration*)*BTree_find(&currentScope, currentToken->str);
            node->offset = symbol->offset;
            advanceToken();

            return (Node*)node;
        }

        case Tok_Decrement: {
            PreDecrementNode* node = malloc(sizeof(PreIncrementNode)); node->type = Nod_Pre_Dec;
            advanceToken();
            VarDeclaration* symbol = (VarDeclaration*)*BTree_find(&currentScope, currentToken->str);
            node->offset = symbol->offset;
            advanceToken();

            return (Node*)node;
        }

        case Tok_KeyWord_if: {
            advanceToken();
            IfNode* node = malloc(sizeof(IfNode)); node->type = Nod_If;
            node->condition = parseExpr();
            node->ifBlock = parseExpr();

            if (currentToken->next->type == Tok_KeyWord_else){
                advanceToken();
                advanceToken();
                node->elseBlock = parseExpr();
            }
            else
                node->elseBlock = NULL;

            return (Node*)node;
        }

        case Tok_KeyWord_while: {
            advanceToken();
            WhileNode*  node = malloc(sizeof(WhileNode)); node->type = Nod_While;
            node->condition = parseExpr();
            node->block = parseExpr();
            return (Node*) node;
        }

        case Tok_KeyWord_break: {
            advanceToken();
            BreakNode* node = malloc(sizeof(BreakNode)); node->type = Nod_Break;
            return node;
        }

        case Tok_KeyWord_continue: {
            advanceToken();
            ContinueNode* node = malloc(sizeof(ContinueNode)); node->type = Nod_Continue;
            return node;
        }

        case Tok_KeyWord_for: {
            advanceToken();
            advanceToken();
            ForNode* node = malloc(sizeof(ForNode)); node->type = Nod_For;
            node->init = parseExpr();
            advanceToken();
            node->condition = parseExpr();
            advanceToken();
            node->repeat = parseExpr();
            advanceToken();
            node->body = parseExpr();
            return (Node*) node;
        }

        case Tok_KeyWord_do: {
            advanceToken();
            DoWhileNode* node = malloc(sizeof(DoWhileNode)); node->type = Nod_DoWhile;
            node->block = parseExpr();
            advanceToken();
            advanceToken();
            node->condition = parseExpr();
            return (Node*) node;
        }

        case Tok_KeyWord_goto: {
            advanceToken();
            GoToNode* node = malloc(sizeof(GoToNode)); node->type = Nod_GoTo;
            node->label = currentToken->str;
            advanceToken();
            return (Node*) node;
        }

        case Tok_KeyWord_switch: {
            advanceToken();
            IntVector oldCases = cases;
            size_t oldID = switchID;
            cases = IntVector_create();

            SwitchNode* node = malloc(sizeof(SwitchNode)); node->type = Nod_Switch;
            node->id = switchID;
            node->value = parseExpr();
            node->body = parseExpr();
            node->cases = cases;

            cases = oldCases;
            switchID = oldID;

            ++switchID;

            return (Node*) node;
        }

        case Tok_KeyWord_case: {
            advanceToken();
            CaseNode* node = malloc(sizeof(CaseNode)); node->type = Nod_Case;
            node->id = switchID;
            node->value = parseExpr();
            
            /* assume that the expression is a single integer ... when constant folding gets added this will get evaluated at a later stage */
            IntVector_push(&cases, ((IntegerNode*)node->value)->value);

            advanceToken();

            return (Node*) node;
        }

        case Tok_KeyWord_default: {
            advanceToken();
            advanceToken();
            DefaultCaseNode* node = malloc(sizeof(DefaultCaseNode)); node->type = Nod_Default_Case;
            node->id = switchID;

            return (Node*) node;
        }

        case Tok_KeyWord_static: {

        }

        default:
            return &nullNode;
    }
}

Node* parseTerm(){
    Node* result = parseFac();
    while (true){
        switch(currentToken->type){
            case Tok_Asterisk: {
                advanceToken();
                MulNode* mulNode = malloc(sizeof(MulNode)); mulNode->type = Nod_Mul;
                mulNode->a = result;
                mulNode->b = parseFac();
                result = (Node*)mulNode;

                break;
            }
            
            case Tok_Div: {
                advanceToken();
                DivNode* divNode = malloc(sizeof(DivNode)); divNode->type = Nod_Div;
                divNode->a = result;
                divNode->b = parseFac();
                result = (Node*)divNode;

                break;
            }

            case Tok_Modulo: {
                advanceToken();
                ModNode* modNode = malloc(sizeof(ModNode)); modNode->type = Nod_Mod;
                modNode->a = result;
                modNode->b = parseFac();
                result = (Node*)modNode;

                break;
            }   
            default:
                return result;
        }
    }
}