#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "error.h"
#include "error_priv.h"

#if defined _MSC_VER || defined __APPLE__
static char *strchrnul(const char *s, int c)
{
  while (*s && *s != c)
    ++s;
  return (char *)s;
}
#endif

#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))

#define ERROR_BUFFER_STARTING_CAPACITY 128

typedef union error_decomposer error_decomposer;
union error_decomposer
{
  error_code code;
  struct
  {
    unsigned int value : 31;
    unsigned int type  : 1;
  };
};

static void critical_error(const char *message)
{
  fprintf(stderr, "Critical error: %s\n", message);
  exit(EXIT_FAILURE);
}

static void release_error_mgr(void)
{
  error_mgr *manager = get_error_mgr();

  if (manager->errors != NULL)
  {
    free(manager->errors);
    manager->errors = NULL;
    manager->size = 0;
    manager->capacity = 0;
  }
}

static void expand_error_buffer(error_mgr *const manager, size_t new_capacity)
{
  if (manager->capacity >= new_capacity)
    return;

  manager->capacity = new_capacity;
  error *errors = malloc(sizeof(error) * new_capacity);

  if (errors == NULL)
    critical_error("Program ran out of memory while managing errors.");

  memcpy(errors, manager->errors, manager->size * sizeof(error));
  free(manager->errors);
  manager->errors = errors;
}


error_mgr *get_error_mgr()
{
  static error_mgr manager = { 0 };

  if (manager.errors == NULL)
  {
    expand_error_buffer(&manager, ERROR_BUFFER_STARTING_CAPACITY);
    atexit(release_error_mgr);
  }
  return &manager;
}

void add_error(error_code code, line_t line, column_t column, int spans)
{
  error_mgr *const manager = get_error_mgr();

  if (manager->size == manager->capacity)
    expand_error_buffer(manager, manager->capacity + manager->capacity / 2);

  error *err = &manager->errors[manager->size++];
  err->code = code;
  err->line = line;
  err->column = column;
  if (spans < 0)
    spans += 1;
  else if (spans > 0)
    spans -= 1;
  err->spans = spans;
}

static void print_error(error err, const char *const context)
{
  const int code = err.code & ~WARNING_MASK;

  const char *line_start = context;
  while (err.line)
  {
    line_start = strchr(line_start, '\n');
    if (!line_start)
      critical_error("Error line is out of context bounds");
    ++line_start;
    --err.line;
  }
  const uint64_t line_size = ((char *)strchrnul(line_start, '\n') - line_start);

  const error_decomposer decomposer = { .code = err.code };

  if (decomposer.type == 1)
    fprintf(stderr, "Warning: %s\n", warning_messages[code]);
  else
    fprintf(stderr, "Error: %s\n", error_messages[code]);

  if (context)
  {
    fprintf(stderr, "%.*s\n", (int)line_size, line_start);
    if (err.column)
    {
      fprintf(stderr, "%*c", err.column + (err.spans < 0 ? err.spans : 0), ' ');

      while (err.spans < 0)
      {
        int span = min(-err.spans, 20);
        fprintf(stderr, "%.*s", span, "~~~~~~~~~~~~~~~~~~~~");
        err.spans += span;
      }
      fprintf(stderr, "^");
      while (err.spans > 0)
      {
        int span = min(err.spans, 20);
        fprintf(stderr, "%.*s", span, "~~~~~~~~~~~~~~~~~~~~");
        err.spans -= span;
      }
      fprintf(stderr, "\n");
    }
  }
}

void print_errors(const char *const context)
{
  error_mgr *const manager = get_error_mgr();

  int i;
  for (i = 0; i < manager->size; ++i)
    print_error(manager->errors[i], context);

  manager->size = 0;
}

void clear_errors()
{
  error_mgr *const manager = get_error_mgr();
  manager->size = 0;
}


size_t getcol(const char* buff, size_t i){
    size_t col = 1;
    for (size_t count = 0; count <= i; ++count)
        if (buff[count] == '\n')
            col = 1;
        else
            ++col;

    return col;
}
size_t getrow(const char* buff, size_t i){
    size_t row = 1;
    for (; row <= i; ++row)
        if (buff[row] == '\n')
            ++row;

    return row;
}