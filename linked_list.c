// Create and destroy
struct linked_list* create_list() {
  struct linked_list* list = malloc(sizeof(struct linked_list));
  list->head = malloc(sizeof(struct linked_list_node));
  list->tail = malloc(sizeof(struct linked_list_node));
  list->head->value = NULL;
  list->tail->value = NULL;
  list->head->prev = NULL;
  list->head->next = list->tail;
  list->tail->prev = list->head;
  list->tail->next = NULL;
  list->count = 0;
  return list;
}

void destruct_list(struct linked_list* list) {
  struct linked_list_node* node = list->head;
  int i = 0;
  while (node->next) {
    node = node->next;
    if (node->prev->value)
      free(node->prev->value);
    free(node->prev);
  }
  if (list->tail->value)
    free(list->tail->value);
  free(list->tail);
  free(list);
}

int len(struct linked_list* list) {
  return list->count;
}

// Append and prepend
void append(struct linked_list* list, void* appendee) {
  struct linked_list_node* node = malloc(sizeof(struct linked_list_node));
  node->value = appendee;
  node->prev = list->tail->prev;
  node->next = list->tail;
  list->tail->prev->next = node;
  list->tail->prev = node;
  list->count++;
}

void prepend(struct linked_list* list, void* appendee) {
  struct linked_list_node* node = malloc(sizeof(struct linked_list_node));
  node->value = appendee;
  node->prev = list->head;
  node->next = list->head->next;
  list->head->next->prev = node;
  list->head->next = node;
  list->count++;
}

// Get and delete
void delete(struct linked_list* list, unsigned index) {
  struct linked_list_node* node = list->head->next;
  for (unsigned i=0; i<index; i++) {
    if (node->next == list->tail) {
      printf("Out of index.");
      return;
    }
    node = node->next;
  }
  node->prev->next = node->next;
  node->next->prev = node->prev;
  free(node);
  list->count--;
}

void* get(struct linked_list* list, unsigned index) {
  struct linked_list_node* node = list->head->next;
  for (unsigned i=0; i<index; i++) {
    if (node->next == list->tail) {
      printf("Out of index.");
      return 0;
    }
    node = node->next;
  }
  return node->value;
}
