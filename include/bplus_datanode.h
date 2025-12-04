#ifndef BP_DATANODE_H
#define BP_DATANODE_H
#include "record.h"
#include "bplus_file_structs.h"
#include"bplus_index_node.h"
#define PTR_SIZE sizeof(int)
#define MAX_DATA_KEYS ((BF_BLOCK_SIZE - 4*sizeof(int)) / sizeof(Record))
/* Στο αντίστοιχο αρχείο .h μπορείτε να δηλώσετε τις συναρτήσεις
 * και τις δομές δεδομένων που σχετίζονται με τους Κόμβους Δεδομένων.*/

typedef struct BPlusDataNode{
    int is_leaf;
    int num_keys;
    int parent;
    int next_leaf;
    Record record[MAX_DATA_KEYS]; 
}BPlusDataNode;


//αρχικοποιήση ενός datanode    
void datanode_init(BPlusDataNode* node);

//έυρεση του σώστού block/node για την εισαγωγή
int find_correct_node(BPlusMeta* metadata, int fd, int key,int *node_block_id);

// εισαγωγή σε node που έχει χώρο
int insert_record_in_node(BPlusDataNode* node, Record *record, TableSchema* schema);

//επιστρέφει το id του νέου μπλοκ δεδομένων και το key που θα γίνει το split ή -1 αν το key υπάρχει ήδη
int split_datanode(int file_desc, BPlusMeta * metadata, int old_node_id, const Record *records, int *split_key);

int get_parent(int file_desc, int child_id);
#endif
