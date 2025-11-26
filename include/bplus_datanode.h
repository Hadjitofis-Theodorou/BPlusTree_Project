#ifndef BP_DATANODE_H
#define BP_DATANODE_H
#include "record.h"
#include "bplus_file_structs.h"

#define MAX_DATA_KEYS 2
/* Στο αντίστοιχο αρχείο .h μπορείτε να δηλώσετε τις συναρτήσεις
 * και τις δομές δεδομένων που σχετίζονται με τους Κόμβους Δεδομένων.*/

typedef struct BPlusDataNode{
    int is_leaf;
    int num_keys;
    int parent;
    int next_leaf;
    Record record[MAX_DATA_KEYS]; //κάθε μπλοκ χωράει 2 εγγραφές
}BPlusDataNode;


//αρχικοποιήση ενός datanode    
void datanode_init(BPlusDataNode* node);

//έυρεση του σώστού block/node για την εισαγωγή
int find_correct_node(BPlusMeta* metadata, int fd, int key,int *node_block_id);

// εισαγωγή σε node που έχει χώρο
void insert_record_in_node(BPlusDataNode* node, Record *record, TableSchema* schema);
#endif
