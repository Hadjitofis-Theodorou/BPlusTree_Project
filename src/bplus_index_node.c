// Μπορείτε να προσθέσετε εδώ βοηθητικές συναρτήσεις για την επεξεργασία Κόμβων Δεδομένων.
#include "../include/bplus_index_node.h"
#include "string.h"


void indexnode_init(BPlusIndexNode *node){
    node->is_leaf=0;
    node->num_keys=0;
    node->parent= -1;

}


int choose_child(BPlusIndexNode *node, int key){
    int i=0;
    while(i<node->num_keys && key>= node->keys[i]){
        i++;
    }
    return node->pointers[i];

}