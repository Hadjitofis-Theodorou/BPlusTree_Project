#include "../include/bplus_file_funcs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define CALL_BF(call)         \
  {                           \
    BF_ErrorCode code = call; \
    if (code != BF_OK)        \
    {                         \
      BF_PrintError(code);    \
      return -1;              \
    }                         \
  }
// WHATS GOING ON??????--->bplus_ERROR

int bplus_create_file(const TableSchema *schema, const char *fileName)
{
  if (schema == NULL || fileName == NULL)
    return -1;
  int fd;
  BF_Block *block;
  CALL_BF(BF_CreateFile(fileName));
  CALL_BF(BF_OpenFile(fileName, &fd));
  BF_Block_Init(&block);
  CALL_BF(BF_AllocateBlock(fd, block));

  BPlusMeta meta;

  meta.file_type = 2004; // αριθμός για b+ trees
  meta.root = -1;
  meta.height = 0;
  meta.schema = *schema;

  void *data_ptr = BF_Block_GetData(block);
  memcpy(data_ptr, &meta, sizeof(BPlusMeta));
  BF_Block_SetDirty(block);
  CALL_BF(BF_UnpinBlock(block));

  CALL_BF(BF_CloseFile(fd));
  BF_Block_Destroy(&block);

  return 0;
}

int bplus_open_file(const char *fileName, int *file_desc, BPlusMeta **metadata)
{
  if (fileName == NULL || file_desc == NULL || metadata == NULL)
    return -1;
  int fd;
  BF_Block *block;
  CALL_BF(BF_OpenFile(fileName, &fd));
  BF_Block_Init(&block);
  CALL_BF(BF_GetBlock(fd, 0, block));

  void *data_ptr = BF_Block_GetData(block);
  *metadata = malloc(sizeof(BPlusMeta));
  if (*metadata == NULL)
  {
    CALL_BF(BF_UnpinBlock(block));
    BF_Block_Destroy(&block);
    CALL_BF(BF_CloseFile(fd));
    return -1;
  }
  memcpy(*metadata, data_ptr, sizeof(BPlusMeta));
  if ((*metadata)->file_type != 2004) // Δεν ειναι Β+
  {
    free(*metadata);
    *metadata = NULL;
    CALL_BF(BF_UnpinBlock(block));
    BF_Block_Destroy(&block);
    CALL_BF(BF_CloseFile(fd));
    return -1;
  }
  *file_desc = fd;
  CALL_BF(BF_UnpinBlock(block));
  BF_Block_Destroy(&block);
  return 0;
}

int bplus_close_file(const int file_desc, BPlusMeta *metadata)
{
  int fd = file_desc;
  BF_Block *block;
  BF_Block_Init(&block);
  CALL_BF(BF_GetBlock(fd, 0, block));
  void *data_ptr = BF_Block_GetData(block);
  memcpy(data_ptr, metadata, sizeof(BPlusMeta));
  BF_Block_SetDirty(block);
  CALL_BF(BF_UnpinBlock(block));
  CALL_BF(BF_CloseFile(fd));
  BF_Block_Destroy(&block);
  free(metadata);
  return 0;
}

int bplus_record_insert(const int file_desc, BPlusMeta *metadata, const Record *record)
{

  // άδειο δέντρο
  if (metadata->root == -1)
  {
    BF_Block *block;
    BF_Block_Init(&block);
    CALL_BF(BF_AllocateBlock(file_desc, block));

    int block_count;
    CALL_BF(BF_GetBlockCounter(file_desc, &block_count));
    int new_node_id = block_count - 1;

    void *data = BF_Block_GetData(block);
    datanode_init(data);
    insert_record_in_node(data, (Record *)record, &metadata->schema);

    metadata->root = new_node_id;
    metadata->height = 1;
    BF_Block_SetDirty(block);
    CALL_BF(BF_UnpinBlock(block));
    BF_Block_Destroy(&block);

    return 0;
  }

  // βρίσκω το σωστο block/node
  int correct_node_id;
  int key = record_get_key(&metadata->schema, record);
  if (find_correct_node(metadata, file_desc, key, &correct_node_id) != 0)
  {
    return -1;
  }

  BF_Block *block;
  BF_Block_Init(&block);
  CALL_BF(BF_GetBlock(file_desc, correct_node_id, block));
  void *data = BF_Block_GetData(block);

  if (!((BPlusDataNode *)data)->is_leaf)
  {
    CALL_BF(BF_UnpinBlock(block));
    BF_Block_Destroy(&block);
    return -1;
  }

  // αν έχουμε χώρο
  if (((BPlusDataNode *)data)->num_keys < MAX_DATA_KEYS)
  {
    insert_record_in_node(data, (Record *)record, &metadata->schema);
    BF_Block_SetDirty(block);
    CALL_BF(BF_UnpinBlock(block));
    BF_Block_Destroy(&block);
    return 0;
  }

  // αν δεν εχουμε χώρο
  return 0;
}

int bplus_record_find(const int file_desc, const BPlusMeta *metadata, const int key, Record **out_record)
{
  *out_record = NULL;
  // Άδειο δέντρο
  if (metadata->root == -1)
  {
    return -1;
  }

  int leaf_block_id;

  //Δεν μπορεσαμε να βρουμε φύλλο
  if (find_correct_node((BPlusMeta *)metadata, file_desc, key, &leaf_block_id) != 0)
  {
    return -1;
  }
  BF_Block *block;
  BF_Block_Init(&block);
  
  CALL_BF(BF_GetBlock(file_desc, leaf_block_id, block));
  void *data=BF_Block_GetData(block);
  BPlusDataNode* node = (BPlusDataNode*)data;
  
  //has to be a leaf
  if(!node->is_leaf){
    CALL_BF(BF_UnpinBlock(block));
    BF_Block_Destroy(&block);
    return -1;
  }
  for(int i=0;i<node->num_keys;i++){
    int curr_key=record_get_key(&metadata->schema,&node->record[i]);
    
    //found the key
    if(curr_key==key){
      Record *ret=malloc(sizeof(Record));
      //malloc failed
      if(ret==NULL){
        CALL_BF(BF_UnpinBlock(block));
        BF_Block_Destroy(&block);
        return -1;
      }

      *ret=node->record[i];
      *out_record=ret;
      CALL_BF(BF_UnpinBlock(block));
      BF_Block_Destroy(&block);
      return 0;
    }
    //break if we are past the possible position
    if(curr_key>key){
      break;
    }
  }
    //not found:(
  CALL_BF(BF_UnpinBlock(block));
  BF_Block_Destroy(&block);
  *out_record=NULL;
  return -1;
  
}
