#ifndef BP_DATANODE_H
#define BP_DATANODE_H
#include "record.h"
/* Στο αντίστοιχο αρχείο .h μπορείτε να δηλώσετε τις συναρτήσεις
 * και τις δομές δεδομένων που σχετίζονται με τους Κόμβους Δεδομένων.*/

typedef struct BPlusDataNode{
    int is_leaf;
    int num_keys;
    int next_block;
    Record record[2];
}BPlusDataNode;

void datanode_init(BPlusDataNode *node);

int datanode_insert_record(BPlusDataNode *node, Record *record, int max_keys);

int datanode_search( BPlusDataNode *node, int key);

void datanode_split(BPlusDataNode *left, BPlusDataNode *right, int *middle_key,Record *new_record, int max_keys);

#endif
