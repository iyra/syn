/* syn, a lisp
Copyright (C) iyra
License: CC0 1.0 Universal Public Domain Dedication
https://creativecommons.org/publicdomain/zero/1.0/
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
enum object_type {
     integer,
     decimal,
     rational,
     string,
     character,
     cons,
     boolean,
     misc,
     symbol,
     tree,
     hex,
};

struct rational {
     int numerator;
     int denominator;
};

struct object {
     enum object_type type;
     union {
	  int obj_int;
	  double obj_decimal;
	  struct rational obj_rational;
	  char *obj_string;
	  char *obj_character;
	  struct cons *obj_cons;
	  bool obj_boolean;
	  char *obj_misc;
	  char *obj_symbol;
	  struct ast *obj_tree;
	  int obj_hex;
     };
};

const char* object_names[] = {
     [integer] = "int",
     [decimal] = "dec",
     [rational] = "rat",
     [string] = "str",
     [character] = "chr",
     [cons] = "cons",
     [boolean] = "bool",
     [misc] = "misc",
     [symbol] = "sym",
     [tree] = "ast",
     [hex] = "hex",
};

enum progress {
     get_argument,
     get_symbol,
};

const char* progress_names[] = {
     [get_argument] = "get car",
     [get_symbol] = "get cdr",
};

struct cons {
     struct object car;
     struct object *cdr;
};


struct ast {
     struct object *args;
     struct ast *parent;
     size_t obj_count;
};

struct program {
     struct ast *ast_list;
};

/*enum progress parse(struct ast *ast,
  const char *in,
  size_t pos,
  enum progress p,
  size_t count,
  size_t argnum);*/
/* void parse(struct ast *ast, */
/*     const char *in, */
/*     size_t pos, */
/*     size_t count, */
/*     enum progress action); */

int
parse(struct ast *ast, const char *in, size_t pos, size_t count, enum progress action, bool seenspace){
     if(pos+1 > strlen(in)){
	  return 0;
     }
     char c = in[pos];
     switch(c){
     case '(':
	  //if(ast->obj_count == 0) { ast->obj_count++; }
	       ast->args = realloc(ast->args, (sizeof(struct object))*(ast->obj_count + 1));
	       ast->args[ast->obj_count] = (struct object) { .type = tree };
	       ast->args[ast->obj_count].obj_tree = malloc(sizeof(struct ast));
	       ast->args[ast->obj_count].obj_tree->args = NULL;
	       ast->args[ast->obj_count].obj_tree->parent = ast;
	       ast->args[ast->obj_count].obj_tree->obj_count = 0;
	       	  fprintf(stderr, "got ( and allocated new tree\n");
	       parse(ast->args[ast->obj_count].obj_tree, in, pos+1, 0, get_argument, 0);
	  break;
     case ')':
	  fprintf(stderr, "got )\n");
	  if(action==get_symbol){}
	  if(action==get_argument){}
	  if(count > 0 && ast->args[ast->obj_count].type == symbol) {
	       ast->args[ast->obj_count].obj_symbol = realloc(ast->args[ast->obj_count].obj_symbol, count+1);
	       ast->args[ast->obj_count].obj_symbol[count] = '\0';
	  }
	  
	  if(ast->parent)
	       parse(ast->parent, in, pos+1, 0, get_argument, 0);
	  break;
     case '\n':
     case '\t':
     case ' ':
	  if(action==get_symbol){}
	  if(action==get_argument){}
	  if(count > 0 && ast->args[ast->obj_count].type == symbol) {
	       ast->args[ast->obj_count].obj_symbol = realloc(ast->args[ast->obj_count].obj_symbol, count+1);
	       ast->args[ast->obj_count].obj_symbol[count] = '\0';
	  }
	  if(!seenspace)
	       /* only increase the argument number if this is the first space we see */
	       ast->obj_count++;
	  parse(ast, in, pos+1, 0, get_argument, 1);
	  break;
     default:
	  if(action==get_symbol){}
	  if(action==get_argument){}
	  ast->args = realloc(ast->args, (sizeof(struct object))*(ast->obj_count + 1));
	  if(!isdigit(c) && count==0){
	       fprintf(stderr, "allocating for initial symbol %c\n", c);
	       /* the start of an argument and it's not a digit, so assume it's a name or symbol */
	       //ast->args[ast->obj_count] = malloc(sizeof(struct object));
	       ast->args[ast->obj_count] = (struct object) { .type = symbol };
	  }
	  if(isdigit(c) && count==0){
	       //ast->args[ast->obj_count] = malloc(sizeof(struct object));
	       ast->args[ast->obj_count] = (struct object) { .type = integer };
	       ast->args[ast->obj_count].obj_int = 0;
	  }
	  
	  if(ast->args[ast->obj_count].type == symbol) {
	       ast->args[ast->obj_count].obj_symbol = realloc(ast->args[ast->obj_count].obj_symbol, count+1);
	       ast->args[ast->obj_count].obj_symbol[count] = c;
	       fprintf(stderr, "storing symbol %c at count %zu\n", c, count);

	  }
	  else if(ast->args[ast->obj_count].type == integer){
	       ast->args[ast->obj_count].obj_int *= 10;
	       ast->args[ast->obj_count].obj_int += c-'0';
	       fprintf(stderr, "storing number %d at count %zu\n", ast->args[ast->obj_count].obj_int, count);

	  }
	  fprintf(stderr, "argnum=%zu\n", ast->obj_count);
	  parse(ast, in, pos+1, count+1, get_argument, 0);
     }
}


void
print_ast(struct ast *ast){
     //printf("found ast\n");
     printf("(");
     //printf("ast obj_count is %zu\n", ast->obj_count);
     for(size_t p = 0; p <= ast->obj_count; p++){
	  if(ast->args[p].type == tree)
	       print_ast(ast->args[p].obj_tree);
	  else {
	       if(ast->args[p].type == integer)
		    printf("%d", ast->args[p].obj_int);
	       else if(ast->args[p].type == symbol)
		    printf("%s", ast->args[p].obj_symbol);
	  }
	  if(p != ast->obj_count)
	       printf(" ");
     }
     printf(")");
}

void
eval(struct ast *ast) {
     
}

int
main(void) {
     const char input[] = "((lambda (x) (+ x 5)) 2) (y 3)";
     size_t l = sizeof(input);
     struct ast root;
     root.args = NULL;
     root.obj_count = 0;
     root.parent = NULL;
     parse(&root, input, 0, 0, get_symbol, 0);
     print_ast(&root);
}

/* void parse(struct ast *ast, */
/*     const char *in, */
/*     size_t pos, */
/*     size_t count, */
/*     enum progress action){ */
/*     char c = in[pos]; */
/*     printf("c is %c\n", c); */

/*     switch(c) { */
/*     case '(': */
/* 	if (action == get_sym){ */
/* 	    printf("saw ( and get_sym\n"); */
/* 	    /\* write the following sym name into ast->sym *\/ */
/* 	    parse(ast, in, pos+1, 0, get_sym); */
/* 	    /\* current keeps looping around until it gets to a non-( character */
/* 	       in future will support ((get-func-name 3) 5) to call (x 5) *\/ */
/* 	    } */
/* 	if (action == get_argument){ */
/* 	    /\* treat this as one of the arguments *\/ */
/* 	    puts("ALLOCATING TREE"); */
/* 	    printf("arg is %zu sizeof is %zu\n", ast->obj_count, sizeof(struct object)); */
/* 	    ast->objs = realloc(ast->objs, (sizeof(struct object))*(ast->obj_count + 1)); */
/* 	    ast->objs[ast->obj_count] = (struct object) { .type = tree }; */
/* 	    ast->objs[ast->obj_count].obj_tree = malloc(sizeof(struct ast)); */
/* 	    ast->objs[ast->obj_count].obj_tree->sym = NULL; */
/* 	    ast->objs[ast->obj_count].obj_tree->objs = NULL; */
/* 	    ast->objs[ast->obj_count].obj_tree->obj_count = 0; */
/* 	    printf("new obj count is %zu\n", ast->obj_count); */
/* 	    parse(ast->objs[ast->obj_count].obj_tree, in, pos+1, 0, get_sym); */
/* 	    } */
/* 	break; */
/*     case ')': */
/* 	if (action == get_sym) { */
/* 	    /\* terminate the symbol *\/ */
/* 	    ast->sym = realloc(ast->sym, count+1); */
/* 	    ast->sym[count] = '\0'; */
/* 	    } */
/* 	if (action == get_argument) { */
/* 	    /\* terminate the argument's object value if necessary e.g string/char/misc *\/ */
/* 	    } */
/* 	break; */
/*     case ' ': */
/*     case '\n': */
/*     case '\t': */
/* 	if (action == get_argument){ */
/* 	    /\* move to collect the next argument *\/ */
/* #if 0 */
/* 	    if(!(ast->objs[ast->obj_count - 1])){ */
/* 		/\* the current argument isn't allocated */
/* 		   so let's do that instead of moving to */
/* 		   a next argument *\/ */
/* 		parse(ast, in, pos+1, 0, get_argument); */
/* 		} else { */
/* 		parse(ast, in, pos+1, 0, get_argument); */
/* 		} */
/* #endif */
/* 	    ast->obj_count += 1; */
/* 	    parse(ast, in, pos+1, 0, get_argument); */
/* 	    } */
/* 	if (action == get_sym){ */
/* 	    /\* move to collect the first argument by re-calling with get_argument *\/ */
/* 	    ast->sym = realloc(ast->sym, count+1); */
/* 	    ast->sym[count] = '\0'; */
/* 	    parse(ast, in, pos+1, 0, get_argument); */
/* 	    } */
/* 	break; */
/*     default: */
/* 	printf("def with %c and action is %s\n", c, progress_names[action]); */
/* 	if (action == get_sym){ */
/* 	    puts("m"); */
/* 	    ast->sym = realloc(ast->sym, count+1); */
/* 	    ast->sym[count] = c; */
/* 	    printf("setting ast->sym[%zu] = %c\n", count, c); */
/* 	    parse(ast, in, pos+1, count+1, get_sym); */
/* 	    } */
/* 	if (action == get_argument){ */
/* 	    if(isdigit(c)){ */
/* 		if(count == 0){ */
/* 		    ast->objs = realloc(ast->objs, (sizeof(struct object)) * (ast->obj_count+1)); */
/* 		    ast->objs[ast->obj_count].type = integer; */
/* 		    printf("new obj count is %zu\n", ast->obj_count); */
/* 		    ast->objs[ast->obj_count].obj_int = 0; */
/* 		    } */
/* 		ast->objs[ast->obj_count].obj_int *= 10; */
/* 		ast->objs[ast->obj_count].obj_int += c-'0'; */
/* 		} */
/* 	    parse(ast, in, pos+1, count+1, get_argument); */
/* 	    } */
/* 	break; */
/* 	} */
/*     } */
/* enum progress parse(struct ast *ast, */
/* 		    const char *in, */
/* 		    size_t pos, */
/* 		    enum progress p, */
/* 		    size_t count, */
/* 		    size_t argnum) */
/* { */
/*   char c = in[pos]; */
/*   bool new_ast = 0; */
/*   switch(c) { */
/*     /\*case '\0': */
/*     puts("finish"); */
/*     break;*\/ */
/*   case ')': */
/*     printf("found )\n"); */
/*     if(p == get_arg){ */
/*       /\* has the same effect as a space for terminating an argument */
/* 1	 except that the argnum doesn't increase *\/ */
/*       if(ast->objs[argnum].type == misc){ */
/* 	ast->objs[argnum].obj_misc = realloc(ast->objs[argnum].obj_misc, count+1); */
/* 	ast->objs[argnum].obj_misc[count] = '\0'; */
/* 	parse(ast, in, pos+1, none, 0, 0); */
/*       } else { puts("not misc ignoring"); } */
/*     } */
/*     if(p  == get_sym){ */
/*       printf("getting sym\n"); */
/*       /\* something like (abc) */
/* 	 no args, so argnum is 0; reset count *\/ */
/*       ast->sym = realloc(ast->sym, count+1); */
/*       ast->sym[count] = '\0'; */
/*       parse(ast, in, pos+1, none, 0, 0); */
/*     } */
/*     break; */

/*   default: */
/*     if(p == get_sym){ */
/*       if(!isspace(c)){ */
/* 	if(c != '('){ */
/* 	  ast->sym = realloc(ast->sym, count + 1); */
/* 	  fprintf(stderr, "gathering symbol: %c count=%zu\n", c, count); */
/* 	  ast->sym[count] = c; */
/* 	  /\* increase count beacuse we're gathering the symbol */
/* 	     name *\/ */
/* 	  parse(ast, in, pos+1, get_sym, count+1, 0); */
/* 	} else { */
/* 	  if(!ast->sym) { */
/* 	    /\* leave count as it is, because we've hit a ( and that's not */
/* 	       part of the symbol name anyway *\/ */
/* 	    parse(ast, in, pos+1, get_sym, 0, 0); */
/* 	  } */
/* 	} */
/*       } else { */
/* 	ast->sym = realloc(ast->sym, count+1); */
/* 	ast->sym[count] = '\0'; */
/* 	printf("the symbol gathered was: %s and count is %zu\n", ast->sym, count); */
/* 	parse(ast, in, pos+1, next_arg, 0, 0); */
/*       } */
/*     } */
/*     if(p == get_arg || p == next_arg){ */
/*       puts("getting arg"); */
/*       if(!isspace(c)){ */
/* 	if(p == next_arg){ */
/* 	  puts("next arg"); */
/* 	  ast->objs = realloc(ast->objs, (argnum+1)*sizeof(struct object)); */
/* 	  printf("obj_count is %zu\n", ast->obj_count); */
/* 	  if(c == '('){ */
/* 	    puts("found (, type=tree"); */
/* 	    ast->objs[argnum] = (struct object){ */
/* 	      .type = tree, */
/* 	    }; */
/* 	  } */
/* 	  else if(c == '"'){ */
/* 	    puts("found string"); */
/* 	    ast->objs[argnum] = (struct object){ */
/* 	      .type = string, */
/* 	    }; */
/* 	  } */
/* 	  else if(isdigit(c) && c != '0'){ */
/* 	    puts("found number"); */
/* 	    ast->objs[argnum] = (struct object){ */
/* 	      .type = integer, */
/* 	    }; */
/* 	    ast->objs[argnum].obj_int = 0; */
/* 	  } */
/* 	  else if(c == '0' && in[pos+1] == 'x'){ */
/* 	    ast->objs[argnum] = (struct object){ */
/* 	      .type = hex, */
/* 	    }; */
/* 	  } */
/* 	  else if(c == '\''){ */
/* 	    if(isdigit(in[pos+1])){ */
/* 	      ast->objs[argnum] = (struct object){ */
/* 		.type = integer, */
/* 	      }; */
/* 	    } else { */
/* 	      ast->objs[argnum] = (struct object){ */
/* 		.type = symbol, */
/* 	      }; */
/* 	    } */
/* 	  } */
/* 	  else { */
/* 	    ast->objs[argnum] = (struct object){ */
/* 		.type = misc, */
/* 	      };	   */
/* 	  } */
/* 	} */
/* 	ast->obj_count = (ast->obj_count == 0) ? 1 : (ast->obj_count)+1; */
	
/* 	printf("allocation stage, TYPE IN is %s\n", object_names[ast->objs[argnum].type]); */
/* 	printf("argnum is %zu\n", argnum); */
/* 	if(ast->objs[argnum].type == string){ */
/* 	  puts("allocating string"); */
/* 	  /\* string *\/ */
/* 	  ast->objs[argnum].obj_string = realloc(ast->objs[argnum].obj_string, count+1); */
/* 	  puts("after realloc"); */
/* 	  if(c != '"'){ */
/* 	    /\* don't put the start " and final " of the string into the string contents *\/ */
/* 	    ast->objs[argnum].obj_string[count] = c; */
/* 	    printf("adding %c to string\n", c); */
/* 	  } */
/* 	  else */
/* 	    ast->objs[argnum].obj_string[count] = '\0'; */
/* 	} else if(ast->objs[argnum].type == integer) { */
/* 	  /\* integer (actually could be a number but just int for now *\/ */
/* 	  if(c-'0' > 0){ */
/* 	    if(ast->objs[argnum].obj_int == 0){ */
/* 	      ast->objs[argnum].obj_int += c-'0'; */
/* 	    } else { */
/* 	      ast->objs[argnum].obj_int *= 10; */
/* 	      ast->objs[argnum].obj_int += c; */
/* 	    } */
/* 	  } */
/* 	  printf("putting number as %d\n", ast->objs[argnum].obj_int); */
/* 	} else if(ast->objs[argnum].type == symbol) { */
/* 	  /\* symbol *\/ */
/* 	  ast->objs[argnum].obj_symbol = realloc(ast->objs[argnum].obj_symbol, count+1); */
/* 	  if(c != '\'') */
/* 	    /\* don't put the ' in front of the symbol into the symbol name *\/ */
/* 	    ast->objs[argnum].obj_symbol[count] = c; */
/* 	} else if(ast->objs[argnum].type == hex) { */
/* 	  /\* hex.. not sure what to do with this thing *\/ */
/* 	} else if(ast->objs[argnum].type == misc) { */
/* 	  ast->objs[argnum].obj_misc = realloc(ast->objs[argnum].obj_misc, count+1); */
/* 	  ast->objs[argnum].obj_misc[count] = c; */
/* 	} else if(ast->objs[argnum].type == tree) { */
/* 	  puts("allocating tree"); */
/* 	  ast->objs[argnum].obj_tree = realloc(ast->objs[argnum].obj_tree, sizeof(struct ast)); */
/* 	  ast->objs[argnum].obj_tree->sym = NULL; */
/* 	  ast->objs[argnum].obj_tree->objs = NULL; */
/* 	  ast->objs[argnum].obj_tree->obj_count = 0; */

/* 	  new_ast = 1; */
/* 	} */

/* 	if(!new_ast){ */
/* 	  puts("---adding 1 to argnum"); */
/* 	  parse(ast, in, pos+1, next_arg, count+1, argnum); */
/* 	} */
/* 	else */
/* 	  parse(ast->objs[argnum].obj_tree, in, pos+1, get_sym, 0, 0); */
/*       } */
/*       else { */
/* 	printf("SPACE and argnum is %zu\n", argnum); */
/* 	/\* close symbol by putting a '\0' at the end *\/ */
/* 	if(ast->objs[argnum].type == symbol){ */
/* 	  ast->objs[argnum].obj_symbol = realloc(ast->objs[argnum].obj_symbol, count+1); */
/* 	  ast->objs[argnum].obj_symbol[count] = '\0'; */
/* 	} */
/* 	parse(ast, in, pos+1, next_arg, 0, argnum+1); /\* collect next arg *\/ */
/*       } */
/*     } */
/*   } */
/*   return 1; */
/* } */

