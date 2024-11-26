#ifndef ERROR_PRIV_H_
#define ERROR_PRIV_H_
#include "error.h"

static const char *const error_messages[] =
{
  "Unexpected token",
  "Unexpected end of file",
  "Unexpected end of line",
  "Unexpected character",
  "Unexpected number",
  "Unexpected string",
  "Unexpected identifier",
  "Unexpected keyword",
  "Unexpected operator",
  "Unexpected symbol",
  "Unexpected semicolon",
  "Missing semicolon",
  "Expected semicolon",
};

static const char *const warning_messages[] =
{
  "Missing pointer cast",
  "Missing integer cast",
};

typedef struct s_error error;
struct s_error
{
  error_code code;
  line_t     line;
  column_t   column;
  int        spans;
};

typedef struct s_error_mgr error_mgr;
struct s_error_mgr
{
  error  *errors;
  size_t  size;
  size_t  capacity;
};

error_mgr *get_error_mgr();

#endif
