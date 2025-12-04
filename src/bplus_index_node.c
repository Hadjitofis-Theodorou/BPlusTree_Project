// Μπορείτε να προσθέσετε εδώ βοηθητικές συναρτήσεις για την επεξεργασία Κόμβων Δεδομένων.
#include "../include/bplus_index_node.h"
#include "string.h"

#define CALL_BF(call)             \
    {                             \
        BF_ErrorCode code = call; \
        if (code != BF_OK)        \
        {                         \
            BF_PrintError(code);  \
            return -1;            \
        }                         \
    }


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

int split_indexnode(int file_desc, int old_index_id,int new_key, int new_right_ptr, int *promoted_key, int *new_block_id){
    BF_Block *old_block, *new_block;
    BF_Block_Init(&old_block);
    BF_Block_Init(&new_block);

    CALL_BF(BF_GetBlock(file_desc,old_index_id, old_block));
    BPlusIndexNode *old_data =(BPlusIndexNode *)BF_Block_GetData(old_block);

    int total_keys=old_data->num_keys+1;
    int temp_keys[MAX_INDEX_KEYS+1];
    int temp_pointers[MAX_INDEX_KEYS+2];
    for(int i=0;i<=old_data->num_keys;i++){
        temp_pointers[i]=old_data->pointers[i];
    }
    for(int i=0;i<old_data->num_keys;i++){
        temp_keys[i]=old_data->keys[i];
    }

    int pos=0;
    while(pos<old_data->num_keys && temp_keys[pos]<new_key){
        pos++;
    }
    for(int i=total_keys-2;i>=pos;i--){
        temp_keys[i+1]=temp_keys[i];

    }
    for(int i=total_keys-1;i>pos;i--){
        temp_pointers[i+1]=temp_pointers[i];
    }
    temp_keys[pos]=new_key;
    temp_pointers[pos+1]=new_right_ptr;

    CALL_BF(BF_AllocateBlock(file_desc,new_block));
    int n_b_id;
    CALL_BF(BF_GetBlockCounter(file_desc,&n_b_id));
    n_b_id--;

    BPlusIndexNode *new_node=(BPlusIndexNode *)BF_Block_GetData(new_block);
    indexnode_init(new_node);
    new_node->parent=old_data->parent;
    
    int promote_index=total_keys/ 2;
    *promoted_key=temp_keys[promote_index];
    old_data->num_keys=0;
    for(int i=0; i<promote_index;i++){
        old_data->keys[i]=temp_keys[i];
        old_data->num_keys++;
    }
    for(int i= 0;i<=promote_index;i++){
        old_data->pointers[i]=temp_pointers[i];
    }
    
    new_node->num_keys= 0;
    int idx=0;
    for (int i=promote_index+1;i<total_keys;i++){
        new_node->keys[idx++]=temp_keys[i];
        new_node->num_keys++;
    }
    idx=0;
    for (int i=promote_index+1;i<=total_keys;i++){
        new_node->pointers[idx++]=temp_pointers[i];
    }
    // Note: we've placed total_keys - (promote_index+1) + 1 = total_keys - promote_index pointers
    // which equals num_keys + 1 for new_node (correct)

    for(int i=0;i<=new_node->num_keys;i++){
        int child_block_id=new_node->pointers[i];
        BF_Block *child_block;
        BF_Block_Init(&child_block);
        CALL_BF(BF_GetBlock(file_desc, child_block_id, child_block));
        void * child_data=BF_Block_GetData(child_block);
        if(((BPlusDataNode*)child_data)->is_leaf){
            ((BPlusDataNode*)child_data)->parent=n_b_id;

        }
        else{
            ((BPlusIndexNode *)child_data)->parent=n_b_id;

        }
        BF_Block_SetDirty(child_block);
        CALL_BF(BF_UnpinBlock(child_block));
        BF_Block_Destroy(&child_block);
    }

    for(int i=0;i<=old_data->num_keys;i++){
        int child_block_id=old_data->pointers[i];
        BF_Block *child_block;
        BF_Block_Init(&child_block);
        CALL_BF(BF_GetBlock(file_desc,child_block_id, child_block));
        void *child_data=BF_Block_GetData(child_block);
        if(((BPlusDataNode*)child_data)->is_leaf){
            ((BPlusDataNode*)child_data)->parent=old_index_id;
        }
        else{
            ((BPlusIndexNode*)child_data)->parent=old_index_id;
        }
        BF_Block_SetDirty(child_block);
        CALL_BF(BF_UnpinBlock(child_block));
        BF_Block_Destroy(&child_block);
    }

    BF_Block_SetDirty(old_block);
    BF_Block_SetDirty(new_block);
    CALL_BF(BF_UnpinBlock(old_block));
    CALL_BF(BF_UnpinBlock(new_block));
    BF_Block_Destroy(&old_block);
    BF_Block_Destroy(&new_block);
    *new_block_id=n_b_id;
    return 0;



}

int insert_into_parent(int file_desc, BPlusMeta *metadata, int left_id, int right_id, int key)
{
    BF_Block *lblock = NULL;
    BF_Block_Init(&lblock);
    CALL_BF(BF_GetBlock(file_desc, left_id, lblock));
    void *ldata = BF_Block_GetData(lblock);

    int parent_id;
    if (((BPlusDataNode *)ldata)->is_leaf) parent_id = ((BPlusDataNode *)ldata)->parent;
    else parent_id = ((BPlusIndexNode *)ldata)->parent;

    CALL_BF(BF_UnpinBlock(lblock));
    BF_Block_Destroy(&lblock);

    
    if (parent_id == -1) {
        BF_Block *root_block = NULL;
        BF_Block_Init(&root_block);
        CALL_BF(BF_AllocateBlock(file_desc, root_block));
        int bc;
        CALL_BF(BF_GetBlockCounter(file_desc, &bc));
        int new_root_id = bc - 1;

        BPlusIndexNode *root = (BPlusIndexNode *)BF_Block_GetData(root_block);
        indexnode_init(root);
        root->num_keys = 1;
        root->keys[0] = key;
        root->pointers[0] = left_id;
        root->pointers[1] = right_id;
        root->parent = -1;

        
        BF_Block *cblock = NULL;
        BF_Block_Init(&cblock);
        CALL_BF(BF_GetBlock(file_desc, left_id, cblock));
        void *cdata = BF_Block_GetData(cblock);
        if (((BPlusDataNode *)cdata)->is_leaf) ((BPlusDataNode *)cdata)->parent = new_root_id;
        else ((BPlusIndexNode *)cdata)->parent = new_root_id;
        BF_Block_SetDirty(cblock);
        CALL_BF(BF_UnpinBlock(cblock));
        BF_Block_Destroy(&cblock);

        BF_Block_Init(&cblock);
        CALL_BF(BF_GetBlock(file_desc, right_id, cblock));
        cdata = BF_Block_GetData(cblock);
        if (((BPlusDataNode *)cdata)->is_leaf) ((BPlusDataNode *)cdata)->parent = new_root_id;
        else ((BPlusIndexNode *)cdata)->parent = new_root_id;
        BF_Block_SetDirty(cblock);
        CALL_BF(BF_UnpinBlock(cblock));
        BF_Block_Destroy(&cblock);

        BF_Block_SetDirty(root_block);
        CALL_BF(BF_UnpinBlock(root_block));
        BF_Block_Destroy(&root_block);

        metadata->root = new_root_id;
        metadata->height++;
        return 0;
    }

    BF_Block *parent_block = NULL;
    BF_Block_Init(&parent_block);
    CALL_BF(BF_GetBlock(file_desc, parent_id, parent_block));
    BPlusIndexNode *parent = (BPlusIndexNode *)BF_Block_GetData(parent_block);

    if (parent->num_keys < MAX_INDEX_KEYS) {
        int i = parent->num_keys - 1;
        while (i >= 0 && parent->keys[i] > key) {
            parent->keys[i + 1] = parent->keys[i];
            parent->pointers[i + 2] = parent->pointers[i + 1];
            i--;
        }
        parent->keys[i + 1] = key;
        int pos = 0;
        while (pos <= parent->num_keys && parent->pointers[pos] != left_id) pos++;
        if (pos <= parent->num_keys) parent->pointers[pos + 1] = right_id;
        else parent->pointers[i + 2] = right_id;
        parent->num_keys++;
        BF_Block_SetDirty(parent_block);
        CALL_BF(BF_UnpinBlock(parent_block));
        BF_Block_Destroy(&parent_block);

        BF_Block *cblock = NULL;
        BF_Block_Init(&cblock);
        CALL_BF(BF_GetBlock(file_desc, right_id, cblock));
        void *cdata = BF_Block_GetData(cblock);
        if (((BPlusDataNode *)cdata)->is_leaf) ((BPlusDataNode *)cdata)->parent = parent_id;
        else ((BPlusIndexNode *)cdata)->parent = parent_id;
        BF_Block_SetDirty(cblock);
        CALL_BF(BF_UnpinBlock(cblock));
        BF_Block_Destroy(&cblock);

        return 0;
    } else {
 
        int promoted;
        int new_index_block;
        if (split_indexnode(file_desc, parent_id, key, right_id, &promoted, &new_index_block) != 0) {
            CALL_BF(BF_UnpinBlock(parent_block));
            BF_Block_Destroy(&parent_block);
            return -1;
        }
        CALL_BF(BF_UnpinBlock(parent_block));
        BF_Block_Destroy(&parent_block);
        return insert_into_parent(file_desc, metadata, parent_id, new_index_block, promoted);
    }
}