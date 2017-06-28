#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
struct ast {
  /* ast := (head tail[0] tail[1] ... tail[tail_n-1])
                   |
                   |> parent = &ast */
  struct object head;
  struct object *tail;
  size_t tail_n;
  struct ast *parent;
};
enum type {
  integer,
  symbol,
};
struct object {
  enum type type;
  union {
    int obj_int;
    char *obj_symbol;
    struct ast *obj_ast;
  }
}
