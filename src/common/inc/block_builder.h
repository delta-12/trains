/*****************************************************************************
* @file block_builder.h
*
* @brief Implements block builder to generate list of blocks from list of records.
*****************************************************************************/
#ifndef TRAINS_SRC_COMMON_INC_BLOCK_BUILDER_H
#define TRAINS_SRC_COMMON_INC_BLOCK_BUILDER_H

#include <string>

#include "types.h"

class BlockBuilder
{
    public:
        BlockBuilder(void);
        BlockBuilder(const std::vector<std::vector<std::string> > &records);
        std::size_t GetSize(void) const;
        types::Block ConvertRecordToBlock(const std::vector<std::string> &record);
        types::Block GetBlock(const types::BlockId block_id);
        std::vector<types::Block> GetBlocks(void) const;
        void Reset(void);

    private:
        std::vector<types::Block> blocks_;
};

#endif // TRAINS_SRC_COMMON_INC_BLOCK_BUILDER_H