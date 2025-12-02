#ifndef BP_INDEX_NODE_H
#define BP_INDEX_NODE_H
#include "bf.h"
#define PTR_SIZE sizeof(int)
#define KEY_SIZE sizeof(int)
#define MAX_INDEX_KEYS ((BF_BLOCK_SIZE - sizeof(int)) / (KEY_SIZE +PTR_SIZE))
/* Στο αντίστοιχο αρχείο .h μπορείτε να δηλώσετε τις συναρτήσεις
 * και τις δομές δεδομένων που σχετίζονται με τους Κόμβους Δεδομένων.*/
typedef struct BPlusIndexNode {
    int is_leaf;
    int num_keys;
    int parent;
    //blocksize-ptr_size/ptr_size+key_size
    int keys[MAX_INDEX_KEYS];
    int pointers[MAX_INDEX_KEYS+1];
}BPlusIndexNode;

void indexnode_init(BPlusIndexNode* node);
int choose_child(BPlusIndexNode* node, int key);
#endif