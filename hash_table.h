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
