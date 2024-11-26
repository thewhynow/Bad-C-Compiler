#ifndef Parse_Expr_H
#define Parse_Expr_H

#include "../parser.h"
#include "../scope_info_map/scope_info/scope_info.h"
Node* parseExpr();
extern ScopeInfo currentScope;

#endif