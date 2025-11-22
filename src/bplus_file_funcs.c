#include "bplus_file_funcs.h"
#include "bplus_datanode.h"
#include "bplus_index_node.h"
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
  
  meta.file_type=2004; // αριθμός για b+ trees
  meta.root=-1;
  meta.height=0;
  meta.schema = *schema;

  void *data_ptr=BF_Block_GetData(block);
  memcpy(data_ptr, &meta,sizeof(BPlusMeta));
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
  BF_Block* block;
  CALL_BF(BF_OpenFile(fileName,&fd));
  BF_Block_Init(&block);
  CALL_BF(BF_GetBlock(fd, 0, block));

  void *data_ptr= BF_Block_GetData(block);
  *metadata=malloc(sizeof(BPlusMeta));
  if(*metadata==NULL){
    CALL_BF(BF_UnpinBlock(block));
    BF_Block_Destroy(&block);
    CALL_BF(BF_CloseFile(fd));
    return -1;
  }
  memcpy(*metadata,data_ptr,sizeof(BPlusMeta));
  if((*metadata)->file_type!=2004)//Δεν ειναι Β+
  {
    free(*metadata);
    *metadata=NULL;
    CALL_BF(BF_UnpinBlock(block));
    BF_Block_Destroy(&block);
    CALL_BF(BF_CloseFile(fd));
    return -1;
  }
  *file_desc=fd;
  CALL_BF(BF_UnpinBlock(block));
  BF_Block_Destroy(&block);
  return 0;

}

int bplus_close_file(const int file_desc, BPlusMeta *metadata)
{
  int fd=file_desc;
  BF_Block *block;
  BF_Block_Init(&block);
  CALL_BF(BF_GetBlock(fd,0,block));
  void* data_ptr = BF_Block_GetData(block);
  memcpy(data_ptr,metadata,sizeof(BPlusMeta));
  BF_Block_SetDirty(block);
  CALL_BF(BF_UnpinBlock(block));
  CALL_BF(BF_CloseFile(fd));
  BF_Block_Destroy(&block);
  free(metadata);
  return 0;
}

int bplus_record_insert(const int file_desc, BPlusMeta *metadata, const Record *record)
{
  BF_Block* block;
  BF_Block_Init(&block);

  if (metadata->root==-1){
    
    CALL_BF(BF_AllocateBlock(file_desc,block));
    int block_id=0;
    CALL_BF(BF_GetBlockCounter);
    int new_block_id=0;


    void *data_ptr=BF_Block_GetData(block);
    BPlusDataNode *leaf= (BPlusDataNode*) data_ptr;
    datanode_init(leaf);



  }
  
  


  return -1;
}

int bplus_record_find(const int file_desc, const BPlusMeta *metadata, const int key, Record **out_record)
{
  *out_record = NULL;
  return -1;
}
