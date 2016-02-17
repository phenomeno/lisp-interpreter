// Hash table- Need to make a function that maps keys to a list index
// Need to make a bunch of lists and put it in an array
// Need to make a data structure that holds a key and a value

// this is a variable that is preprocessed - processor replaces the variable name with the value
// but it doesn't occupy memory
// #define SIZE_BUCKETS 100

// Our basic linked_list_node_hash_table will be like the one in the linked list, but it has a key/value thing now
// struct linked_list_node_hash_table {
//   char* key;
//   struct _value* value;
//   struct linked_list_node_hash_table* next;
//   struct linked_list_node_hash_table* prev;
// };
//
// // It is not necessary to use a list. In theoretical terms, this would be a bucket.
// struct linked_list_hash_table {
//   struct linked_list_node_hash_table* head;
//   struct linked_list_node_hash_table* tail;
// };
//
// // This structure contains all of the buckets. We call this a hash table.
// // Our hash table has an array that contains a bunch of integer addresses for a struct linked_list* type element.
// struct hash_table {
//   struct linked_list_hash_table* ht[SIZE_BUCKETS];
// };

// Create a list (bucket), one of many. It is the same as the linked_list example, but it is
// important to set the key to an empty string, or later when it tries to dereference the key value,
// it will make an error since you can't dereference a null value.
struct linked_list_hash_table* make_list() {
  struct linked_list_hash_table* list = malloc(sizeof(struct linked_list_hash_table));
  list->head = malloc(sizeof(struct linked_list_node_hash_table));
  list->tail = malloc(sizeof(struct linked_list_node_hash_table));
  list->head->prev = NULL;
  list->head->next = list->tail;
  list->head->key = "";
  list->tail->prev = list->head;
  list->tail->next = NULL;
  list->tail->key = "";
  return list;
}

// Making the hash table. Initialize list for each bucket.
struct hash_table* make_hash_table() {
  struct hash_table* hash_table = malloc(sizeof(struct hash_table));
  for (unsigned i=0; i<SIZE_BUCKETS; i++) {
    hash_table->ht[i] = make_list();
  }
  return hash_table;
}

// Hash function. Takes the key string and for the length of string,
// we force it into an int type to get its ASCII value and add it to sum.
// We take the mod value of that sum to determine which bucket to go to.
int hash(char* key) {
  int sum = 0;
  int len = strlen(key);
  for (int i=0; i<len; i++) {
    sum += (int) key[i];
  }
  return sum % SIZE_BUCKETS;
}

// To append to hash table, create the node and add it to the list that has been passed to the fn.
// This is a subfunction of set- DO NOT USE. DO NOT USE!! DO NOT USE!!!! A LIST MUST GO IN HERE.
// THIS IS A UTILITY FUNCTION. DO! NOT! USE!!
void append_to_hash_table(struct linked_list_hash_table* list, char* key, struct _value* value) {
  struct linked_list_node_hash_table* node = malloc(sizeof(struct linked_list_node_hash_table));
  node->key = key;
  node->value = value;
  node->next = list->tail;
  node->prev = list->tail->prev;
  list->tail->prev->next = node;
  list->tail->prev = node;
}

// Find the hash value of the key, then pass the hash value as the index to the correct bucket.
void set(struct hash_table* hash_table, char* key, struct _value* value) {
  int hash_value = hash(key);
  append_to_hash_table(hash_table->ht[hash_value], key, value);
}

// To get a value using the key, we hash the key and look it up in the hash_table.
// If the key matches the key in the node, then return the value.
// To compare strings, we use the function strcmp(node->key, key) which returns zero
// if there's a match.
struct _value* get_from_hash_table(struct hash_table* hash_table, char* key) {
  int hash_value = hash(key);
  struct linked_list_hash_table* list = hash_table->ht[hash_value];
  struct linked_list_node_hash_table* node = list->head;
  while (node->next) {
    if (strcmp(node->key, key) == 0) {
      return node->value;
    }
    node = node->next;
  }
  return NULL;
}

// Same idea.
void delete_hash_table(struct hash_table* hash_table, char* key) {
  int hash_value = hash(key);
  struct linked_list_hash_table* list = hash_table->ht[hash_value];
  struct linked_list_node_hash_table* node = list->head;
  while (node->next) {
    if (strcmp(node->key, key) == 0) {
      node->prev->next = node->next;
      node->next->prev = node->prev;
      free(node);
      return;
    }
    node = node->next;
  }
}

void extend_hash_table(struct hash_table* hash_table, struct hash_table* appendee) {
  // Need to copy over each list and append to the first table.
  for (int i=0; i<SIZE_BUCKETS; i++) {
    // Start at head and loop through and append to hash_table.
    struct linked_list_hash_table* llht = appendee->ht[i];
    struct linked_list_node_hash_table* node = llht->head;
    while(node->next) {

      set(hash_table, node->key, node->value);
      node = node->next;

    }
  }

  // return hash_table;
  return;
}
