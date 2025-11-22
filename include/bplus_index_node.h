#ifndef BP_INDEX_NODE_H
#define BP_INDEX_NODE_H
#include "bf.h"
#define MAX_INDEX_KEYS 2
#define MAX_INDEX_POINTERS 3
/* Στο αντίστοιχο αρχείο .h μπορείτε να δηλώσετε τις συναρτήσεις
 * και τις δομές δεδομένων που σχετίζονται με τους Κόμβους Δεδομένων.*/
typedef struct BPlusIndexNode {
    int is_leaf;
    int num_keys;
    int keys[MAX_INDEX_KEYS];
    int pointers[MAX_INDEX_POINTERS];
}BPlusIndexNode;

void indexnode_init(BPlusIndexNode *node);
int indexnode_insert_key(BPlusIndexNode *node, int key, int pointer, int max_keys);
int indexnode_find_child(const BPlusIndexNode *node, int key);
void indexnode_split(BPlusIndexNode *left, BPlusIndexNode *right, int *middle_key, int new_key, int new_pointer, int max_keys);

#endif