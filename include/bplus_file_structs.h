//
// Created by theofilos on 11/4/25.
//

#ifndef BPLUS_BPLUS_FILE_STRUCTS_H
#define BPLUS_BPLUS_FILE_STRUCTS_H
#include "bf.h"
#include "bplus_datanode.h"
#include "bplus_index_node.h"
#include "record.h"
#include "bplus_file_structs.h"

typedef struct {
    int file_type;
    int root;
    int height;
    int block_type;
    int point_max;
    int key_max;

    TableSchema schema;

} BPlusMeta;

#endif //BPLUS_BPLUS_FILE_STRUCTS_H

