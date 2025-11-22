// Μπορείτε να προσθέσετε εδώ βοηθητικές συναρτήσεις για την επεξεργασία Κόμβων toy Ευρετηρίου.
#include "../include/bplus_datanode.h"


//Αρχικοποίηση του node δεδομένων
void datanode_init(BPlusDataNode* node){
    node->is_leaf=1;
    node->num_keys=0;
    node->next_block=-1;
    memset(node->record, 0, sizeof(node->record));
}

//Εισαγωγή και ταξινόμηση νέας εγγραφής
int datanode_insert_record(BPlusDataNode *node, const Record *record, int max_keys){
    if(node->num_keys>=max_keys){
        return -1;
    }

    int key= record->values->int_value;
    int insert_position=0;
    for (int i = 0; i < node->num_keys; i++) {
    if (node->record[i].values->int_value == key) {
        return -2;  // Duplicate key error
    }
    }
    while (insert_position< node->num_keys&& node->record[insert_position].values->int_value < key) {
        insert_position++;
    }

    for(int i =node->num_keys; i>insert_position; i--){
        memcpy(&node->record[i],&node->record[i-1], sizeof(Record));
    }

    memcpy(&node->record[insert_position], record, sizeof(Record));
    node->num_keys++;
    return 0;
}


//Αναζήτηση data node
int datanode_search( BPlusDataNode *node, int key){
    for(int i=0; i<node->num_keys; i++){
        if (node->record[i].values->int_value==key){
            return i;
        }
    }
    return -1;
}

//split το Node αν χρειαστεί μετά από εισαγωγή
void datanode_split(BPlusDataNode *left, BPlusDataNode *right, int *middle_key,Record *new_record, int max_keys){
    Record temp[max_keys+1];
    int size=0;
    int new_key= new_record->values->int_value;
    int inserted=0;

    for (int i=0;i<left->num_keys;i++){
        if(!inserted && new_key<left->record[i].values->int_value){
            memcpy(&temp[size++],new_record,sizeof(Record));
            inserted=1;
        }
        memcpy(&temp[size++],&left->record[i],sizeof(Record));

    }
    if(!inserted){
        memcpy(&temp[size++],new_record,sizeof(Record));

    }
    int mid=size/2;
    left->num_keys=mid;
    memcpy(left->record,temp,mid*sizeof(Record));
    right->num_keys=size-mid;
    memcpy(right->record,&temp[mid],right->num_keys*sizeof(Record));

    *middle_key=right->record[0].values->int_value;

}