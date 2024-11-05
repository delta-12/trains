/*****************************************************************************
* @file block_builder.h
*
* @brief Implements block builder to generate list of blocks from list of records.
*****************************************************************************/
#ifndef TRAINS_SRC_COMMON_INC_BLOCK_BUILDER_H
#define TRAINS_SRC_COMMON_INC_BLOCK_BUILDER_H

#include <string>
#include <vector>

#include "types.h"

enum class RecordType
{
    RECORDTYPE_NONE,
    RECORDTYPE_SCHEDULE,
    RECORDTYPE_TRACK_LAYOUT
};

class BlockBuilder
{
    public:
        BlockBuilder(const std::vector<std::vector<std::string>> &records, const RecordType record_type);
        std::size_t GetSize(void) const;
        types::Error GetBlock(const types::BlockId block_id, types::Block &block);
        std::vector<types::Block> GetBlocks(void) const;
        void Reset(void);

    private:
        void BuildBlocksFromSchedule(const std::vector<std::vector<std::string>> &records);
        void BuildBlocksFromTrackLayout(const std::vector<std::vector<std::string>> &records);

        std::vector<types::Block> blocks_;
};

#endif // TRAINS_SRC_COMMON_INC_BLOCK_BUILDER_H