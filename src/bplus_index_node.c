// Μπορείτε να προσθέσετε εδώ βοηθητικές συναρτήσεις για την επεξεργασία Κόμβων Δεδομένων.
#include "../include/bplus_index_node.h"
#include "string.h"

void indexnode_init(BPlusIndexNode *node){
    node->is_leaf=0;
    node->num_keys=0;
    memset(node->keys,0,sizeof(node->keys));
    memset(node->pointers,0,sizeof(node->pointers));
}

int indexnode_insert_key(BPlusIndexNode *node, int key, int pointer, int max_keys)
{
    if(node->num_keys>=max_keys){
        return -1;
    }
    int insert_position=0;
    while(insert_position < node->num_keys && node->keys[insert_position]<key){
        insert_position++;
    }
    for(int i =node->num_keys; i>insert_position; i--){
        node->keys[i]=node->keys[i-1];
    }
    for(int i =node->num_keys+1; i>insert_position+1; i--){
        node->pointers[i]=node->pointers[i-1];
    }
    node->keys[insert_position]=key;
    node->pointers[insert_position+1]=pointer;
    node->num_keys++;
    return 0;
}

int indexnode_find_child(const BPlusIndexNode *node, int key){
    int i=0;
    while (i<node->num_keys && key>=node->keys[i]){
        i++;
    }
    return node->pointers[i];
}

void indexnode_split(BPlusIndexNode *left, BPlusIndexNode *right, int *middle_key, int new_key, int new_pointer, int max_keys){
    int temp_keys[max_keys+1];
    int temp_pointers[max_keys+2];
    int size=0;
    int inserted=0;

    for(int i=0;i<left->num_keys;i++){
        if(!inserted&&new_key<left->keys[i]){
            temp_keys[size]=new_key;
            temp_pointers[size+1]=new_pointer;
            size++;
            inserted=1;
            
        }
        temp_keys[size]=left->keys[i];
        temp_pointers[size]=left->pointers[i];
        size++;
    }
    temp_pointers[size]=left->pointers[left->num_keys];
    if(!inserted){
        temp_keys[size]=new_key;
        temp_pointers[size+1]=new_pointer;
        size++;
    }
    int mid=size/2;
    *middle_key=temp_keys[mid];
    left->num_keys=mid;
    memcpy(left->keys,temp_keys,mid*sizeof(int));
    memcpy(left->pointers,temp_pointers,(mid+1)*sizeof(int));
    
    right->num_keys=size-mid-1;
    memcpy(right->keys,&temp_keys[mid+1],right->num_keys*sizeof(int));
    memcpy(right->pointers,&temp_pointers[mid+1],(right->num_keys+1)*sizeof(int));
}