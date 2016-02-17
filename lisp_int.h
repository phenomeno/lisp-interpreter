// Structs and prototypes for linked_list
struct linked_list_node {
  void* value;
  struct linked_list_node* prev;
  struct linked_list_node* next;
};

struct linked_list {
  struct linked_list_node* head;
  struct linked_list_node* tail;
  unsigned count;
};

struct linked_list* create_list();
void destruct_list(struct linked_list* list);
int len(struct linked_list* list);
void append(struct linked_list* list, void* appendee);
void prepend(struct linked_list* list, void* appendee);
void delete(struct linked_list* list, unsigned index);
void* get(struct linked_list* list, unsigned index);

// for hash table
#define SIZE_BUCKETS 100

struct linked_list_node_hash_table {
  char* key;
  struct _value* value;
  struct linked_list_node_hash_table* next;
  struct linked_list_node_hash_table* prev;
};

struct linked_list_hash_table {
  struct linked_list_node_hash_table* head;
  struct linked_list_node_hash_table* tail;
};

struct hash_table {
  struct linked_list_hash_table* ht[SIZE_BUCKETS];
};

struct linked_list_hash_table* make_list();
struct hash_table* make_hash_table();
int hash(char* key);
void append_to_hash_table(struct linked_list_hash_table* list, char* key, struct _value* value);
void set(struct hash_table* hash_table, char* key, struct _value* value);
struct _value* get_from_hash_table(struct hash_table* hash_table, char* key);
void delete_hash_table(struct hash_table* hash_table, char* key);
void extend_hash_table(struct hash_table* hash_table, struct hash_table* appendee);

// for tree
struct token {
  char* token_name;
  char* token_value;
};

struct func {
  struct linked_list* args;
  struct linked_list* body;
  struct hash_table* captured;
};

/* Union struct */
struct _value {
  char* kind;
  union _expression {
    int _int;
    char* _string;
    struct func* _func;
  } e;
};

struct node {
  void* value;
  struct linked_list* children;
};

struct tree {
  struct node* root;
};

struct tree* create_tree();
struct node* insert(struct node* parent_node, struct token* token);
void traverse(struct node* node, unsigned count);
