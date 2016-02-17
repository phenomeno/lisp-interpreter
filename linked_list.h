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
