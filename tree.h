// Search tree with insert child to node, delete, and search
#include <stdio.h>
#include <stdlib.h>

struct token {
  char* token_name;
  char* token_value;
};

// Create a tree node which has just a next value and append children to the next of next.
// Find parent by going all the way to front of the nexts. I suppose I will make a prev too??
struct node {
  void* value;
  struct linked_list* children;
};

// Create a binary tree that has a binary tree node called root.
struct tree {
  struct node* root;
};

// Initialize the tree by defining a root.
struct tree* create_tree() {
  struct tree* _tree = malloc(sizeof(struct tree));
  struct token* t = malloc(sizeof(struct token));
  t->token_name = "ROOT";
  t->token_value = "ROOT";
  _tree->root = malloc(sizeof(struct node));
  _tree->root->value = (void*) t;
  _tree->root->children = create_list();
  return _tree;
}

// To insert, we won't do any comparing of the key to make the 'search' part of the tree.
// We will just receive the parent node, and create a node and pop in the token.
struct node* insert(struct node* parent_node, struct token* token) {
  struct node* child_node = malloc(sizeof(struct node));
  child_node->value = (void*) token;
  child_node->children = create_list();
  append(parent_node->children, (void*) child_node);
  return child_node;
}

// Not sure we can really search because there is no key.
// char* search(struct node* bst_node, int key) {
//   if (bst_node->key == key) {
//     printf("%s\n", bst_node->value);
//     return bst_node->value;
//   } else if (key < bst_node->key) {
//     search(bst_node->left, key);
//   } else {
//     search(bst_node->right, key);
//   }
// }

// Traverse from the root.
void traverse(struct node* node, unsigned count) {
  for (unsigned x=0; x<count; x++) {
    printf("  ");
  }
  struct token* t = node->value;
  printf("%s, %s\n", t->token_name, t->token_value);
  struct linked_list* l = node->children;
  for (int i=0; i<len(l); i++) {
    struct node* n = get(l, i);
    traverse(n, count+1);
  }
}
