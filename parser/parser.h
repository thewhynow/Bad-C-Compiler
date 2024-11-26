#ifndef Parser_H
#define Parser_H

/*
    TODO: ADD A "DESTRUCTOR" FOR ALL POSSIBLE NODES. PLEASE.
*/

#include "../lexer/lexer.h"
#include "scope_info_map/scope_info/scope_info.h"
#include "scope_info_map/include/BST.h"
#include "int_vector/int_vector.h"
#include <stdbool.h>
typedef BinaryTree ScopeInfo;

typedef enum {
    Nod_Null,
    Nod_Program,
    Nod_Function_Decl,
    Nod_Return,
    Nod_Block,
    Nod_Integer,
    Nod_Negate,
    Nod_Bitwise_Not,
    Nod_LogicalNot,
    Nod_Add,
    Nod_Sub,
    Nod_Mul,
    Nod_Div,
    Nod_Mod,
    Nod_LogicalAnd,
    Nod_LogicalOr,
    Nod_EqualTo,
    Nod_NEqualTo,
    Nod_GreaterThan,
    Nod_GreaterThan_Equal,
    Nod_LessThan,
    Nod_LessThan_Equal,
    Nod_LShift,
    Nod_RShift,
    Nod_Bitwise_And,
    Nod_Bitwise_Or,
    Nod_Bitwise_Xor,
    Nod_Comma_Expr,
    Nod_Var_Access,
    Nod_Assign,
    Nod_Add_Assign,
    Nod_Sub_Assign,
    Nod_Mul_Assign,
    Nod_Div_Assign,
    Nod_Mod_Assign,
    Nod_LShift_Assign,
    Nod_RShift_Assign,
    Nod_BitAnd_Assign,
    Nod_BitOr_Assign,
    Nod_BitXor_Assign,
    Nod_Pre_Inc,
    Nod_Post_Inc,
    Nod_Pre_Dec,
    Nod_Post_Dec,
    Nod_If,
    Nod_Ternary,
    Nod_While,
    Nod_DoWhile,
    Nod_For,
    Nod_Break,
    Nod_Continue,
    Nod_GoTo,
    Nod_Label,
    Nod_Switch,
    Nod_Case,
    Nod_Default_Case,
    Nod_Function_Call,
} NodeType;

typedef struct {
    NodeType type;
} Node;

typedef struct NodeList NodeList;
struct NodeList {
    Node* node;
    NodeList* next;
};

typedef struct {
    NodeType type;
    NodeList* lines;
} BlockNode;

typedef struct {
    NodeType type;
    NodeList* node;
    uint64_t labels;
} ProgramNode;

typedef struct {
    NodeType type;
    char* name;
    Node* body;
    size_t framesize;
    size_t argc;
} FunctionDeclNode;

typedef struct {
    NodeType type;
    uint32_t value;
} IntegerNode;

typedef struct {
    NodeType type;
    Node* value;
} ReturnNode;

typedef struct {
    NodeType type;
    Node* value;
} NegativeNode;

typedef struct {
    NodeType type;
    Node* value;
} BitwiseNotNode;

typedef struct {
    NodeType type;
    Node* value;
} LogicNotNode;

typedef struct {
    NodeType type;
    Node* a;
    Node* b;
} BinaryOpNode;

typedef BinaryOpNode AddNode;
typedef BinaryOpNode SubNode;
typedef BinaryOpNode MulNode;
typedef BinaryOpNode DivNode;
typedef BinaryOpNode ModNode;

typedef BinaryOpNode LogicAndNode;
typedef BinaryOpNode LogicOrNode;

typedef BinaryOpNode EqualToNode;
typedef BinaryOpNode NEqualToNode;
typedef BinaryOpNode GreaterThanNode;
typedef BinaryOpNode GreaterThan_EqualNode;
typedef BinaryOpNode LessThanNode;
typedef BinaryOpNode LessThan_EqualNode;

typedef BinaryOpNode BitwiseAndNode;
typedef BinaryOpNode BitwiseOrNode;
typedef BinaryOpNode BitwiseXorNode;
typedef BinaryOpNode LShiftNode;
typedef BinaryOpNode RShiftNode;

typedef BinaryOpNode CommaExpression;

ProgramNode* parse(Token* tokens, char* buff_);

#include "parse_expr/parse_expr.h"

typedef struct {
    NodeType type;
    bool mutable;
    size_t offset;
} VarAccessNode;

typedef struct {
    NodeType type;
    size_t offset;
    Node* value;
} AssignNode;

typedef AssignNode AssignAddNode;
typedef AssignNode AssignSubNode;
typedef AssignNode AssignMulNode;
typedef AssignNode AssignDivNode;
typedef AssignNode AssignModNode;
typedef AssignNode AssignLShiftNode;
typedef AssignNode AssignRShiftNode;
typedef AssignNode AssignBitAndNode;
typedef AssignNode AssignBitOrNode;
typedef AssignNode AssignBitXorNode;

typedef struct {
    NodeType type;
    uint16_t offset;
} UnaryNode;

typedef UnaryNode PostIncrementNode;
typedef UnaryNode PreIncrementNode;
typedef UnaryNode PostDecrementNode;
typedef UnaryNode PreDecrementNode;


typedef struct {
    NodeType type;
    Node* condition;
    Node* ifBlock;
    Node* elseBlock;
} IfNode;

typedef IfNode TernaryNode;

typedef struct {
    NodeType type;
    Node* condition;
    Node* block;
} WhileNode;

typedef WhileNode DoWhileNode;

typedef struct {
    NodeType type;
    Node* init;
    Node* condition;
    Node* repeat;
    Node* body;
} ForNode;

typedef Node BreakNode;
typedef Node ContinueNode;

typedef struct {
    NodeType type;
    char* label;
} LabelNode;

typedef struct {
    NodeType type;
    char* label;
} GoToNode;

typedef struct {
    NodeType type;
    size_t id;
    Node* value;
    Node* body;
    IntVector cases;
} SwitchNode;

typedef struct {
    NodeType type;
    size_t id;
    Node* value;
} CaseNode;

typedef struct {
    NodeType type;
    size_t id;
} DefaultCaseNode;

typedef struct {
    NodeType type;
    char* identifier;
    NodeList* args;
} FunctionCallNode;

#endif