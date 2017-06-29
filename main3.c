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
  bool is_root;
};
enum type {
  integer,
  symbol,
  ast,
};
struct object {
  enum type type;
  union {
    int obj_int;
    char *obj_symbol;
    struct ast *obj_ast;
  }
};

int main(void){
  const char in[] = "(+ 2 3) ((lambda (x) (+ x 5)) 3) (3 4) '(2 1)";
  size_t l = sizeof(in);
  size_t o_paren_n = 0;
  for(int i = 0; i < l; i++)
    if(in[i] == '(')
      o_paren_n++;
  struct ast roots[o_paren_n];
  for(i = 0; i < o_paren_n; i++){
    roots[i].head = NULL;
    roots[i].tail = NULL;
    roots[i].tail_n = 0;
    roots[i].parent = NULL;
    roots[i].is_root = 1;
  }
}

int
parse(struct ast *roots, size_t root_n,
      const char *input, size_t pos,
      enum action action, size_t charcount) {
  char c = input[pos];
  switch(c){
  case '(':
    if(action == action_begin_root) {
      /* if we're beginning a root then don't start a new
	 sub-ast because we haven't gathered anything yet */
      parse(roots, root_n, input, pos+1, action_get_head, 0);
    }
    if(action == action_get_head){
      /* ^((
	  ^ */
      roots[root_n].head = malloc(sizeof(struct object));
      roots[root_n].head = (struct object) {
	.type = ast
      };
      roots[root_n].head.obj_ast = malloc(sizeof(struct ast));
      roots[root_n].head.obj_ast->head = NULL;
      roots[root_n].head.obj_ast->tail = NULL;
      roots[root_n].head.obj_ast->tail_n = 0;
      roots[root_n].head.obj_ast->parent = roots[root_n];
      roots[root_n].head.obj_ast->roots[i].is_root = 0;
      parse((struct ast [])roots[root_n].head.obj_ast, 0, input, pos+1, action_get_head, 0);
    }
    if(action == action_get_tail){
      /* ^(head (
	        ^ */
      roots[root_n].tail[roots[root_n].tail_n] = malloc(sizeof(struct object));
      roots[root_n].tail[roots[root_n].tail_n] = (struct object) {
	.type = ast
      };
      roots[root_n].tail[roots[root_n].tail_n].obj_ast = malloc(sizeof(struct ast));
      roots[root_n].tail[roots[root_n].tail_n].obj_ast->head = NULL;
      roots[root_n].tail[roots[root_n].tail_n].obj_ast->tail = NULL;
      roots[root_n].tail[roots[root_n].tail_n].obj_ast->tail_n = 0;
      roots[root_n].tail[roots[root_n].tail_n].obj_ast->parent = roots[root_n];
      roots[root_n].tail[roots[root_n].tail_n].obj_ast->roots[i].is_root = 0;
      parse((struct ast [])roots[root_n].tail[roots[root_n].tail_n].obj_ast, root_n, input, pos+1, action_get_head, 0);

    }
    break;
  case ' ':
    if(action == action_get_head) {
      if(charcount > 0) {
	if(roots[root_n].head.type == symbol){
	  roots[root_n].head->obj_symbol = realloc(roots[root_n].head->obj_symbol, charcount+1);
	  roots[root_n].head->obj_symbol = '\0';
	}
	parse(roots, root_n, input, pos+1, action_get_tail, 0);
      } else {
	parse(roots, root_n, input, pos+1, action_get_head, 0);
      }
    }
    
    if(action == action_get_tail) {
      parse(roots, root_n, input, pos+1, action_get_tail, 0);
    }
    break;
  case ')':
    if(action == action_get_head){
      if(roots[root_n].head.type == ast)
	parse(roots[root_n]->parent, 0, input, pos+1, action_get_head, 0);
      if(roots[root_n].head.type == symbol){
	fprintf(stderr, "unexpected ), expected symbol separator or symbol character\n");
	return 0;
      }
    }
    if(action == action_get_tail){
      if(roots[root_n].tail[roots[root_n].tail_n-1].type == ast){
	roots[root_n].tail_n += 1;
	parse(roots[root_n], 0, input, pos+1, action_get_tail, 0);
      }
      if(roots[root_n].tail[roots[root_n].tail_n-1].type == symbol){
	fprintf(stderr, "unexpected ), expected symbol separator or symbol character\n");
      }
    }
    break;
  default:
    if(action == action_begin_root){
      fprintf(stderr, "unexpected character, expected (\n");
      return 0;
    }
    if(action == action_get_head){
      if(charcount==0){
	roots[root_n].head = malloc(struct object);
	roots[root_n].head = (struct object) {
	  .type = symbol
	};
      }
      roots[root_n].head->obj_symbol = realloc(roots[root_n].head->obj_symbol, charcount+1);
      roots[root_n].head->obj_symbol = c;
      parse(roots, root_n, input, pos+1, action_get_head, charcount+1);
    }
  }
}
