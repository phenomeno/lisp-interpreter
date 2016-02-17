// Standard input-output library and standard library for...??
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lisp_int.h"

// Prototypes
struct _value* interpret_list(struct node* _node, struct linked_list* context);
int check_list(struct node* _node, struct linked_list* tokens, int token_index);

/*
* LEXER - gets a string and chops them into tokens!! chop chop
*/

// LPAREN Automaton Rule. Also adds token to a list of tokens (side effect).
// Non-deterministic because it is using a pointer from scope outside of the fn
int lparen_rule(char* source_code, struct linked_list* tokens) {
  int pos = 0;
  if (source_code[0] == '(') {
    pos++;
    struct token* t = malloc(sizeof(struct token));
    t->token_value = "(";
    t->token_name = "LPAREN";
    append(tokens, t);
  }
  return pos;
}

// RPAREN Automaton Rule. Also adds token to a list of tokens (side effect).
// Non-deterministic because it is using a pointer from scope outside of the fn
int rparen_rule(char* source_code, struct linked_list* tokens) {
  int pos = 0;
  if (source_code[0] == ')') {
    pos++;
    struct token* t = malloc(sizeof(struct token));
    t->token_value = ")";
    t->token_name = "RPAREN";
    append(tokens, t);
  }
  return pos;
}

// IDENT Automaton Rule. Also adds token to a list of tokens (side effect).
// Non-deterministic because it is using a pointer from scope outside of the fn
int ident_rule(char* source_code, struct linked_list* tokens) {
  int pos = 0;
  // Literal that goes in at compile time. Space allocated then. It is constant.
  char* rule = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ+-/*=";
  while (strchr(rule, source_code[pos])) {
    pos++;
  }
  if (pos > 0) {
    struct token* t = malloc(sizeof(struct token));
    char* c = malloc(pos*sizeof(char)+1);
    strncpy(c, source_code, pos);
    t->token_value = c;
    t->token_name = "IDENT";
    append(tokens, t);
  }
  return pos;
}

// WHITESPACE Automaton Rule. Also adds token to a list of tokens (side effect).
// Non-deterministic because it is using a pointer from scope outside of the fn
int whitespace_rule(char* source_code, struct linked_list* tokens) {
  int pos = 0;

  while (source_code[pos] == ' ' ||
        source_code[pos] == '\t' ||
        source_code[pos] == '\n' ||
        source_code[pos] == '\r' ||
        source_code[pos] == '\0') {
    pos++;
  }

  return pos;
}

// NUMBER Automaton Rule. Also adds token to a list of tokens (side effect).
// Non-deterministic because it is using a pointer from scope outside of the fn
int number_rule(char* source_code, struct linked_list* tokens) {
  int pos = 0;
  char* rule = "0123456789";
  while(strchr(rule, source_code[pos])) {
    pos++;
  }
  if (pos > 0) {
    struct token* t = malloc(sizeof(struct token));
    char* c = malloc(pos*sizeof(char)+1);
    strncpy(c, source_code, pos);
    t->token_value = c;
    t->token_name = "NUMBER";
    append(tokens, t);
  }
  return pos;
}

/*
* UTILITY FUNCTIONS
*/
struct node* insert_token(struct node* _node, char* token_name, char* token_value) {
  struct token* t = malloc(sizeof(struct token));
  t->token_name = token_name;
  t->token_value = token_value;
  struct node* child_node = insert(_node, t);
  return child_node;
}

/*
* PARSER - takes tokens and moves them into an abstract syntax tree
*/
int check_expr(struct node* _node, struct linked_list* tokens, int token_index) {
  struct token* ct = get(tokens, token_index);

  char* options[4] = {"IDENT", "NUMBER", "STRING", "LIST"};
  for (int i=0;i<4;i++) {
    if (!strcmp(options[i], ct->token_name)) {
      struct node* expr_node = insert_token(_node, "EXPR", "EXPR");
      insert_token(expr_node, ct->token_name, ct->token_value);
      token_index++;
      return token_index;
    }
  }

  if (strcmp(ct->token_name, "LPAREN") == 0) {
    struct node* expr_node = insert_token(_node, "EXPR", "EXPR");
    token_index = check_list(expr_node, tokens, token_index);
    return token_index;
  }
  return 0;
}

int check_list(struct node* _node, struct linked_list* tokens, int token_index) {
  struct token* ct = get(tokens, token_index);
  if (strcmp(ct->token_name, "LPAREN") == 0) {
    struct node* list_node = insert_token(_node, "LIST", "LIST");
    insert_token(list_node, ct->token_name, ct->token_value);
    token_index++;
    while(1) {
      int check_expr_value = check_expr(list_node, tokens, token_index);
      if (check_expr_value == 0) break;
      token_index = check_expr_value;
    }
    struct token* nt = get(tokens, token_index);
    if (strcmp(nt->token_name, "RPAREN") == 0) {
      insert_token(list_node, nt->token_name, nt->token_value);
      token_index++;
      return token_index;
    }

  } else {
    // Not a tree.
    return NULL;
  }
}

struct tree* parse_tokens(struct linked_list* tokens) {
  struct tree* ast = create_tree();
  int token_index = 0;
  // Add the program node. This is not really a token. But oh well.
  struct node* child_node = insert_token(ast->root, "PROGRAM", "PROGRAM");
  while (len(tokens) > token_index) {
    token_index = check_list(child_node, tokens, token_index);
  }
  return ast;
}


// Utility functions for interpreter
struct node* get_next_node(struct node* _node, int child_no) {
  // This gets the i-th child of the node, then gets first children of that node and returns value.
  struct node* n = (struct node*) get(_node->children, child_no);
  return n;
}


/*
* Interpreter
*/
struct _value* interpret_expr(struct node* _node, struct hash_table* context) {
  // Function enters with the EXPR node. Get next node looks at the meaty token, not the virtual token.
  struct node* param_node = get_next_node(_node, 0);
  struct token* token = param_node->value;
  struct _value* value = malloc(sizeof(struct _value));

  // printf("Token name: %s\n", token->token_name);
  // printf("Token value: %s\n", token->token_value);

  if (strcmp(token->token_name, "NUMBER")==0) {
    value->kind = "INT";
    value->e._int = atoi(token->token_value);
  } else if (strcmp(token->token_name, "LIST")==0) {
    value = interpret_list(param_node, context);
  } else if (strcmp(token->token_name, "STRING")==0) {
    value->kind = "STRING";
    value->e._string = token->token_value;
  } else if (strcmp(token->token_name, "IDENT")==0) {
    // Check each hash table in context, starting from the last one.
    for (int i=len(context)-1; i>=0; i--) {
      struct hash_table* ht = get(context, i);
      // printf("token value key: %s\n", token->token_value);
      value = get_from_hash_table(ht, token->token_value);
      if (value != NULL) {
        break;
      }
    }
    if (value == NULL) {
      printf("No key available for this variable: %s.\n", token->token_value);
      exit(1);
    }
  }
  // printf("Value kind: %s\n", value->kind);

  return value;
}

struct _value* interpret_list(struct node* _node, struct linked_list* context) {
  // This function is first executed with the list node right under the program node.

  // Gets the second child in list node since first is LPAREN
  struct _value* value = malloc(sizeof(struct _value));
  int node_children_length = len(_node->children);
  struct node* n = get_next_node(_node, 1);
  struct token* t = n->value;
  // Checks if the node we got was EXPR
  if (strcmp(t->token_name, "EXPR")==0) {

    // Looks at token underneath EXPR. EXPR is just a virtual token.
    n = get_next_node(n, 0);
    t = n->value;

    // If token underneath is IDENT type and a "+" loop through tokens in the list (_node)'s children from 2 till length-1 to exclude the RPAREN.
    if (strcmp(t->token_name, "IDENT")==0 && strcmp(t->token_value, "+")==0) {
      int sum = 0;

      for (int i=2; i<node_children_length-1; i++) {
        struct node* param_node = get_next_node(_node, i);
        struct token* param_token = param_node->value;
        if (strcmp(param_token->token_name, "EXPR")==0) {
          struct _value* expr_return = interpret_expr(param_node, context);
          if (strcmp(expr_return->kind, "INT")==0) {
            sum += expr_return->e._int;
          }
        } else {
          printf("Expected expression.");
          exit(1);
        }
      }

      // Assign final value of list and return.
      value->kind = "INT";
      value->e._int = sum;
      return value;

      // If token underneath is IDENT and a "-", loop through tokens in the list (_node)'s children from 2 till length-1 to exclude the RPAREN.
    } else if (strcmp(t->token_name, "IDENT")==0 && strcmp(t->token_value, "-")==0) {
      int minus = 0;

      for (int i=2; i<node_children_length-1; i++) {
        struct node* param_node = get_next_node(_node, i);
        struct token* param_token = param_node->value;
        if (strcmp(param_token->token_name, "EXPR")==0) {
          struct _value* expr_return = interpret_expr(param_node, context);
          if (strcmp(expr_return->kind, "INT")==0) {
            if (i==2) {
              minus = expr_return->e._int;
            } else {
              minus -= expr_return->e._int;
            }
          }
        }
      }
      // Assign final value of list and return.
      value->kind = "INT";
      value->e._int = minus;
      return value;

      // If token underneath is IDENT and a "/", loop through tokens in the list (_node)'s children from 2 till length-1 to exclude the RPAREN.
    } else if (strcmp(t->token_name, "IDENT")==0 && strcmp(t->token_value, "/")==0) {
      int quotient = 0;

      for (int i=2; i<node_children_length-1; i++) {
        struct node* param_node = get_next_node(_node, i);
        struct token* param_token = param_node->value;
        if (strcmp(param_token->token_name, "EXPR")==0) {
          struct _value* expr_return = interpret_expr(param_node, context);
          if (strcmp(expr_return->kind, "INT")==0) {
            if (i==2) {
              quotient = expr_return->e._int;
            } else if(expr_return->e._int !=0) {
              quotient /= expr_return->e._int;
            } else {
              printf("Cannot divide by zero.\n");
              exit(1);
            }
          } else {
            printf("Cannot divide with a non-integer type.\n");
            exit(1);
          }
        }
      }

      // Assign final value of list and return.
      value->kind = "INT";
      value->e._int = quotient;
      return value;

      // If token underneath is IDENT and a "*", loop through tokens in the list (_node)'s children from 2 till length-1 to exclude the RPAREN.
    } else if (strcmp(t->token_name, "IDENT")==0 && strcmp(t->token_value, "*")==0) {
      int multiplied = 1;

      for (int i=2; i<node_children_length-1; i++) {
        struct node* param_node = get_next_node(_node, i);
        struct token* param_token = param_node->value;
        if (strcmp(param_token->token_name, "EXPR")==0) {
          struct _value* expr_return = interpret_expr(param_node, context);
          if (strcmp(expr_return->kind, "INT")==0) {
            multiplied *= expr_return->e._int;
          }
        }
      }

      // Assign final value of list and return.
      value->kind = "INT";
      value->e._int = multiplied;
      return value;

      // If token is IDENT and a "define": if next token is IDENT, then variable definition. If it is a list, it is a function defintion.
    } else if (strcmp(t->token_name, "IDENT")==0 && strcmp(t->token_value, "define")==0) {
      // Lookahead to next node, after skipping virtual node.
      struct node* second_index_in_list = get_next_node(_node, 2);
      struct node* child_of_second_index = get_next_node(second_index_in_list, 0);
      struct token* child_token = child_of_second_index->value;

      // If variable definition, the second index will be an ident. (define a 2) -> a = 2
      if (strcmp(child_token->token_name, "IDENT")==0) {
        char* variable_name;
        struct _value* variable_value;

        // Check second position of list for variable name.
        struct node* var_name_node = get_next_node(_node, 2);
        struct token* var_name_token = var_name_node->value;

        if (strcmp(var_name_token->token_name, "EXPR")==0) {
          // With a variable definition, cant have anything requiring interpreting as the variable name, so we will skip the interpret_expr step.
          struct node* child_node = get_next_node(var_name_node, 0);
          struct token* child_node_token = child_node->value;
          variable_name = child_node_token->token_value;
        }

        // Check third position of list for variable value.
        struct node* var_value_node = get_next_node(_node, 3);
        struct token* var_value_token = var_value_node->value;
        if (strcmp(var_value_token->token_name, "EXPR")==0) {
          variable_value = interpret_expr(var_value_node, context);
        }

        // Store in hash table. If it's in there already, write over it.
        // Will store variables in last ht of list.
        struct hash_table* last_ht = get(context, len(context)-1);
        set(last_ht, variable_name, variable_value);
        // Check that it set properly and set as value so it has a return type.
        value = get_from_hash_table(last_ht, variable_name);

        // If function definition, the second index will be a list.
      } else if (strcmp(child_token->token_name, "LIST")==0) {
        char* fn_name;
        // Holds _value types.
        struct linked_list* variables = create_list();
        // Holds tree node types.
        struct linked_list* body_lists = create_list();
        struct node* second_index_in_list = get_next_node(_node, 2);

        // List node
        struct node* child_of_second_index = get_next_node(second_index_in_list, 0);

        // Check 1st position in the list- skipping the LPAREN.
        struct node* function_name_node = get_next_node(child_of_second_index, 1);
        struct token* function_name_token = function_name_node->value;

        if (strcmp(function_name_token->token_name, "EXPR")==0) {
          struct node* token_node = get_next_node(function_name_node, 0);
          struct token* token_token = token_node->value;
          fn_name = token_token->token_value;
        }

        // Loop through rest of the positions in the list and add to the variables list.
        for (int i=2; i<len(child_of_second_index->children)-1;i++) {
          struct node* param = get_next_node(child_of_second_index, i);
          struct token* param_token = param->value;

          if (strcmp(param_token->token_name, "EXPR")==0) {
            // Parameters will never be expressions so dont need to run them through interpret expr.
            // Just going to look at what's under expr, which is gonna be an ident/string.
            struct node* param_child_node = get_next_node(param, 0);
            struct token* param_child_token = param_child_node->value;

            if (strcmp(param_child_token->token_name, "IDENT")==0) {
              struct _value* param_value = malloc(sizeof(struct _value));
              param_value->kind = "STRING";
              param_value->e._string = param_child_token->token_value;
              append(variables, (void*) param_value);
            }

          }
        }

        // Loop through next lists in the first list for the rest of the body of the function
        for (int i=3; i<len(_node->children)-1; i++) {
          // This is the 3rd index in the first list, the expr (which is a list) that comes after then fn and the variables list.
          struct node* list_node = get_next_node(_node, i);

          // Need to somehow save all these nodes into a list. I know how!!!
          append(body_lists, (void*) list_node);

        }
        //  Create func object and add to context. Hash key is fn name, and hash value is a _value type with kind func.
        struct func* new_func = malloc(sizeof(struct func));
        new_func->args = variables;
        new_func->body = body_lists;
        new_func->captured = make_hash_table();
        for (int i=0;i<len(context);i++) {
          struct hash_table* ht = get(context, i);
          extend_hash_table(new_func->captured, ht);
        }
        struct _value* func_value = malloc(sizeof(struct _value));
        func_value->kind = "FUNC";
        func_value->e._func = new_func;

        // Append func to last available ht in context.
        struct hash_table* last_ht = get(context, len(context)-1);
        set(last_ht, fn_name, func_value);
        return func_value;
      }

    // Case when we want to execute a function
    } else {
      // Going to put a bunch of _value types in here.
      struct linked_list* params = create_list();
      struct _value* fn_v;
      struct node* expr_node = get_next_node(_node, 1);

      // Depending on whether function name is simply in an IDENT or needs to be calculated in a list.
      fn_v = interpret_expr(expr_node, context);

      // if (strcmp(t->token_name, "IDENT")==0) {
      //   fn_v = interpret_expr(n, context);
      // } else if (strcmp(t->token_name, "LIST")==0) {
      //   fn_v = interpret_list(n, context);
      // }

      struct func* fn;
      if (strcmp(fn_v->kind, "FUNC")==0) {
        fn = fn_v->e._func;
        // Adding in "freezed" context from when we defined the function.
        append(context, (void*) fn->captured);
      } else {
        printf("not func type\n");
        exit(1);
      }

      // Rest of list is in _node. Has a bunch of expressions. and starts at 2.
      for (int i=2;i<node_children_length-1;i++) {
        struct node* next_node = get_next_node(_node, i);
        // Put into params list as a _value type after we interpret the expr.
        struct _value* param_value = interpret_expr(next_node, context);
        // Has all of the arguments. The naming is wrong.
        append(params, (void*) param_value);
      }

      // Create a new hash table for a new scope. We make this more recent than the frozen captured variables for closure.
      struct hash_table* new_ht_scope = make_hash_table();

      // Now we get the func from our fn and set up the variables to equal our params.
      for (int i=0; i<len(fn->args); i++) {
        // Assuming each item in args is going to be a string
        struct _value* v = get(fn->args, i);
        char* arg_name = v->e._string;
        // Now add each arg name and the match param into the context (hash table).
        set(new_ht_scope, arg_name, get(params, i));
      }

      // Now we add the hash table to our context.
      append(context, (void*) new_ht_scope);

      // Now we run body.
      for (int i=0; i<len(fn->body); i++) {
        struct node* bn = get(fn->body, i);
        value = interpret_expr(bn, context);
      }

      // Deletes local scope with all the args and matching variables that were passed along.
      delete(context, len(context)-1);
      // Deletes closure scope with variables defined during fn definition.
      delete(context, len(context)-1);
      /* End of else case where we execute fn */
    }
    return value;
  }
}

struct _value* interpret_program(struct node* _node, struct linked_list* context) {
  struct linked_list* children = _node->children;

  // Sends list node underneath program node to interpret_list.
  // program = list* (can be multiple lists)
  struct _value* v;
  for (int i=0; i<len(children); i++) {
    v = interpret_list(get(children, i), context);
  }
  return v;
}

void interpret_tree(struct tree* ast) {
  struct linked_list* children = ast->root->children;
  struct linked_list* context = create_list();
  struct hash_table* ht = make_hash_table();
  append(context, (void*) ht);


  // Sends the program node underneath root node to interpret_program.
  for (int i=0; i<len(children); i++) {
    struct node* _node = get(children, i);
    struct _value* value = interpret_program(_node, context);

    // Check final value- if kind is INT, print as int.
    if (strcmp(value->kind, "INT")==0) {
      printf("%d\n", value->e._int);
    } else if (strcmp(value->kind, "STRING")==0) {
      // Check final value- if kind is STRING, print as string.
      printf("%s\n", value->e._string);
    }
  }
}

/*
* Main
*/

int main(int argc, char** argv) {
  struct linked_list* tokens = create_list();
  FILE* fp;
  char* source_code = calloc(1024, sizeof(char));
  char* file_name = argv[1];

  fp = fopen(file_name, "r");

  if (fp == NULL) {
    printf("Error in opening file.\n");
    exit(1);
  }
  fread(source_code, sizeof(char), 1024, fp);
  fclose(fp);

  // Lexer
  int increased = 0;
  do {
    source_code += increased = lparen_rule(source_code, tokens);
    if(increased) continue;

    source_code += increased = ident_rule(source_code, tokens);
    if(increased) continue;

    source_code += increased = number_rule(source_code, tokens);
    if(increased) continue;

    source_code += increased = rparen_rule(source_code, tokens);
    if(increased) continue;

    source_code += increased = whitespace_rule(source_code, tokens);
    if(increased) continue;
  } while(increased != 0);

  // Print tokens.
  // for (int i=0; i<len(tokens); i++) {
  //   struct token* t = get(tokens, i);
  //   printf("Token %d: %s, %s\n", i, t->token_value, t->token_name);
  // }

  struct tree* ast = parse_tokens(tokens);
  // traverse(ast->root, 0);
  interpret_tree(ast);

  return 0;
}
