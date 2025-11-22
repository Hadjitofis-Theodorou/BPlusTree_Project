// Μπορείτε να προσθέσετε εδώ βοηθητικές συναρτήσεις για την επεξεργασία Κόμβων toy Ευρετηρίου.
#include "../include/bplus_datanode.h"



void datanode_init(BPlusDataNode* node){
    node->is_leaf=1;
    node->num_keys=0;
    node->parent=-1;
    node->next_leaf=-1;
}



int find_correct_node(BPlusMeta* metadata, int fd, int key,int *node_block_id){
    if (metadata->root==-1){
        return -1;
    }
    int current_block_id= metadata->root;
    BF_Block *block;
    BF_Block_Init(&block);

    while(1){
        CALL_BF(BF_GetBlock(fd,current_block_id,block));

        void* data= BF_Block_GetData(block);
        if (*((int *)data)==1){
            *node_block_id=current_block_id;
            CALL_BF( BF_UnpinBlock(block));
            BF_Block_Destroy(&block);

        
            return 0;

        }

        BPlusIndexNode* index_node= (BPlusIndexNode*)data; //θέλει typecast?
        int next_node_block_id=choose_child(index_node,key);
        CALL_BF( BF_UnpinBlock(block));
        
        current_block_id=next_node_block_id;
    }
    BF_Block_Destroy(&block);
    return-1;


}



//όπως από τις διαφάνειες, εισαγωγή εγγραφής διατηρώντας τη ταξινόμηση
void insert_record_in_node(BPlusDataNode* node, Record *record, TableSchema* schema){
    int record_key= record_get_key(schema,record);
    int i = node->num_keys-1;

    while(i>=0){
        int current_key= record_get_key(schema, &node->record[i]);
        if (current_key<=record_key){
            break;
        }
        node->record[i+1]=node->record[i];
        i--;
    }

    node->record[i+1]=*record;
    node->num_keys++;

}

