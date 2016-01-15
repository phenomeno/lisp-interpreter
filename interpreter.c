// Making a doubly-linked list with -append, -prepend, -search, -delete, -construct, -destruct, length of list
// gcc linked_list.c -g, lldb a.out, r, thread return, bt, print <c code>

// Standard input-output library and standard library for...??
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked_list.h"
#include "tree.h"

/*
*
* LEXER - gets a string and chops them into tokens!! chop chop
*
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
  char* rule = " \t\n";
  while (strchr(rule, source_code[pos])) {
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
*
* PARSER - takes tokens and moves them into an abstract syntax tree
*
*/
int check_expr(struct node* _node, struct linked_list* tokens, int token_index) {
  struct token* ct = get(tokens, token_index);

  char* options[4] = {"IDENT", "NUMBER", "STRING", "LIST"};
  for (int i=0;i<4;i++) {
    if (!strcmp(options[i], ct->token_name)) {
      struct node* expr_node = insert_token(_node, "EXPR", "EXPR");
      insert_token(expr_node, ct->token_name, ct->token_value);
      token_index++;
      printf("token index in expr %d\n", token_index);
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
      printf("Token index: %d\n", token_index);
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

// struct tree* check_program(struct linked_list* tokens) {
//   while (tokens->next) {
//     tokens = check_list(tokens);
//   }
// }

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

int main() {
  struct linked_list* tokens = create_list();
  char* source_code = "(+ 2(+ 1 3)) (aa 1)";
  // char* source_code = "(+ 2 2)";

  // Lexer
  while (*source_code != NULL) {
    source_code += lparen_rule(source_code, tokens);
    source_code += ident_rule(source_code, tokens);
    source_code += whitespace_rule(source_code, tokens);
    source_code += number_rule(source_code, tokens);
    source_code += rparen_rule(source_code, tokens);
  }

  // Parser


  // Print tokens.
  for (int i=0; i<len(tokens); i++) {
    struct token* t = get(tokens, i);
    printf("Token %d: %s, %s\n", i, t->token_value, t->token_name);
  }

  struct tree* ast = parse_tokens(tokens);
  traverse(ast->root, 0);
  return 0;
}
