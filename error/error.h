#ifndef ERROR_H_
#define ERROR_H_

/*



*/

#include <stdlib.h>

#define ERROR_MASK   0x00000000
#define WARNING_MASK 0x80000000

typedef enum e_error_code error_code;
enum e_error_code
{
  // errors
  UNEXPECTED_TOKEN      = ERROR_MASK | 0x00000000,
  UNEXPECTED_EOF        = ERROR_MASK | 0x00000001,
  UNEXPECTED_EOL        = ERROR_MASK | 0x00000002,
  UNEXPECTED_CHAR       = ERROR_MASK | 0x00000003,
  UNEXPECTED_NUMBER     = ERROR_MASK | 0x00000004,
  UNEXPECTED_STRING     = ERROR_MASK | 0x00000005,
  UNEXPECTED_IDENTIFIER = ERROR_MASK | 0x00000006,
  UNEXPECTED_KEYWORD    = ERROR_MASK | 0x00000007,
  UNEXPECTED_OPERATOR   = ERROR_MASK | 0x00000008,
  UNEXPECTED_SYMBOL     = ERROR_MASK | 0x00000009,
  UNEXPECTED_SEMICOLON  = ERROR_MASK | 0x0000000a,
  MISSING_SEMICOLON     = ERROR_MASK | 0x0000000b,
  EXPECTED_SEMICOLON    = ERROR_MASK | 0x0000000c,

  // warnings
  MISSING_POINTER_CAST = WARNING_MASK | 0x00000000,
  MISSING_INTEGER_CAST = WARNING_MASK | 0x00000001,
};

typedef unsigned int line_t;
typedef unsigned int column_t;

void add_error(error_code code, line_t line, column_t column, int spans);
void print_errors(const char *const context);
void clear_errors();

size_t getcol(const char* buff, size_t i);
size_t getrow(const char* buff, size_t i);
#endif
