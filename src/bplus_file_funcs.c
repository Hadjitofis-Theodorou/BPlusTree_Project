#include "bplus_file_funcs.h"
#include <stdio.h>
#include <string.h>
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
  meta.key_max=2;
  meta.point_max=3;
  meta.block_type=0; //0=ευρετηρίου, 1= δεδομένων
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
  return -1;
}

int bplus_close_file(const int file_desc, BPlusMeta *metadata)
{
  return -1;
}

int bplus_record_insert(const int file_desc, BPlusMeta *metadata, const Record *record)
{
  return -1;
}

int bplus_record_find(const int file_desc, const BPlusMeta *metadata, const int key, Record **out_record)
{
  *out_record = NULL;
  return -1;
}
