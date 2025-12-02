// Μπορείτε να προσθέσετε εδώ βοηθητικές συναρτήσεις για την επεξεργασία Κόμβων toy Ευρετηρίου.
#include "../include/bplus_datanode.h"
#include "../include/bplus_file_structs.h"
#define CALL_BF(call)             \
    {                             \
        BF_ErrorCode code = call; \
        if (code != BF_OK)        \
        {                         \
            BF_PrintError(code);  \
            return -1;            \
        }                         \
    }
// WHATS GOING ON??????--->bplus_ERROR

void datanode_init(BPlusDataNode *node)
{
    node->is_leaf = 1;
    node->num_keys = 0;
    node->parent = -1;
    node->next_leaf = -1;
}

int find_correct_node(BPlusMeta *metadata, int fd, int key, int *node_block_id)
{
    if (metadata->root == -1)
    {
        return -1;
    }
    int current_block_id = metadata->root;
    BF_Block *block;
    BF_Block_Init(&block);

    while (1)
    {
        CALL_BF(BF_GetBlock(fd, current_block_id, block));

        void *data = BF_Block_GetData(block);
        if (*((int *)data) == 1)
        {
            *node_block_id = current_block_id;
            CALL_BF(BF_UnpinBlock(block));
            BF_Block_Destroy(&block);

            return 0;
        }

        BPlusIndexNode *index_node = (BPlusIndexNode *)data; // θέλει typecast?
        int next_node_block_id = choose_child(index_node, key);
        CALL_BF(BF_UnpinBlock(block));

        current_block_id = next_node_block_id;
    }
    BF_Block_Destroy(&block);
    return -1;
}

// όπως από τις διαφάνειες, εισαγωγή εγγραφής διατηρώντας τη ταξινόμηση
void insert_record_in_node(BPlusDataNode *node, Record *record, TableSchema *schema)
{
    int record_key = record_get_key(schema, record);
    int i = node->num_keys - 1;

    while (i >= 0)
    {
        int current_key = record_get_key(schema, &node->record[i]);
        if (current_key <= record_key)
        {
            break;
        }
        node->record[i + 1] = node->record[i];
        i--;
    }

    node->record[i + 1] = *record;
    node->num_keys++;
}

int split_datanode(int file_desc, BPlusMeta *metadata, int old_node_id, const Record *record, int *split_key)
{
    BF_Block *new_block,*block;
    int key=record_get_key(&metadata->schema,record);
    BF_Block_Init(&block);
    BF_Block_Init(&new_block);
    CALL_BF(BF_GetBlock(file_desc,old_node_id,block))
    BPlusDataNode *data = (BPlusDataNode *)BF_Block_GetData(block);
    CALL_BF(BF_AllocateBlock(file_desc, new_block));
    int new_block_id;
    CALL_BF(BF_GetBlockCounter(file_desc, &new_block_id));
    new_block_id--;

    BPlusDataNode *new_data = (BPlusDataNode *)BF_Block_GetData(new_block);
    datanode_init(new_data);
    // βάζω τα records σε ενα temporary array
    Record temp[MAX_DATA_KEYS + 1];
    int total_keys = ((BPlusDataNode *)data)->num_keys;

    int insert_flag = 0; // flag για αν βάλαμε το νεο record στη σωστή θέση
    for (int i = 0; i < total_keys; i++)
    {
        int current_key = record_get_key(&metadata->schema, &((BPlusDataNode *)data)->record[i]);
        //απορριψη σε περιπτωση που υπαρχει ηδη
        if(key==current_key)
            {return -1;}
        if (key < current_key && insert_flag == 0)
        {
            temp[i] = *record;
            insert_flag = 1;
            
        }
        temp[i + insert_flag] = ((BPlusDataNode *)data)->record[i]; // είτε στην ίδια θέση με πριν είτε σε μια πάν
    }
    // αν είναι ακόμα 0 σημαίνει είναι μεγαλύτερο απόλα
    if (insert_flag == 0)
    {
        temp[total_keys] = *record;
    }
    total_keys++;

    int split = (total_keys + 1) / 2;
    ((BPlusDataNode *)data)->num_keys = 0;
    for (int i = 0; i < split; i++)
    {
        ((BPlusDataNode *)data)->record[i] = temp[i];
        ((BPlusDataNode *)data)->num_keys++;
    }

    for (int i = split; i < total_keys; i++)
    {
        ((BPlusDataNode *)new_data)->record[new_data->num_keys] = temp[i];
        new_data->num_keys++;
    }

    // ναι βαρεθηκαμε να κανουμε typecast από πριν το ((BPlusDataNode *)data)
    new_data->next_leaf = ((BPlusDataNode *)data)->next_leaf;
    ((BPlusDataNode *)data)->next_leaf = new_block_id;

    BF_Block_SetDirty(block);
    BF_Block_SetDirty(new_block);
    CALL_BF(BF_UnpinBlock(block));
    CALL_BF(BF_UnpinBlock(new_block));
    BF_Block_Destroy(&block);
    BF_Block_Destroy(&new_block);

    *split_key = record_get_key(&metadata->schema, &new_data->record[0]);
    return new_block_id;
}