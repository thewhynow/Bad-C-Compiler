#ifndef Scope_Info_H
#define Scope_Info_H

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdint.h>

#define VAR_DECL ((uint8_t)1)
#define FUN_DECL ((uint8_t)2)

typedef enum {
    Tp_Null,
    Tp_int,
} DataType;

typedef enum {
    St_static,
    St_extern,
    St_none,
} StorageType;

size_t getTSize(DataType type);

typedef struct {
    uint8_t type;
} ScopeDeclaration;

typedef struct {
    uint8_t decltype;
    size_t offset;
    StorageType stype;
} VarDeclaration;

typedef struct {
    uint8_t decltype;
    StorageType stype;
} FunctionDeclaration;

#endif