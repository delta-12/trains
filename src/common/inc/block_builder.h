/*****************************************************************************
* @file block_builder.h
*
* @brief Implements block builder to generate list of blocks from list of records.
*****************************************************************************/
#ifndef TRAINS_SRC_COMMON_INC_BLOCK_BUILDER_H
#define TRAINS_SRC_COMMON_INC_BLOCK_BUILDER_H

#include <string>

#include "types.h"

#define BLOCK_BUILDER_CSV_FIELD_LINE                 0
#define BLOCK_BUILDER_CSV_FIELD_SECTION              1
#define BLOCK_BUILDER_CSV_FIELD_BLOCK_NUMBER         2
#define BLOCK_BUILDER_CSV_FIELD_BLOCK_LENGTH         3
#define BLOCK_BUILDER_CSV_FIELD_BLOCK_GRADE          4
#define BLOCK_BUILDER_CSV_FIELD_SPEED_LIMIT          5
#define BLOCK_BUILDER_CSV_FIELD_INFRASTRUCTURE       6
#define BLOCK_BUILDER_CSV_FIELD_STATION_SIDE         7
#define BLOCK_BUILDER_CSV_FIELD_ELEVATION            8
#define BLOCK_BUILDER_CSV_FIELD_CUMULATIVE_ELEVATION 9

class BlockBuilder
{
    public:
        BlockBuilder(void);
        BlockBuilder(const std::vector<std::vector<std::string>> &records);
        std::size_t GetSize(void) const;
        void AssignBlockInfrastructure(types::Block &block, const std::string &input_string);
        types::Block ConvertRecordToBlock(const std::vector<std::string> &record);
        types::Error GetBlock(const types::BlockId block_id, types::Block &block);
        std::vector<types::Block> GetBlocks(void) const;

        void Reset(void);

    private:
        std::vector<types::Block> blocks_;
};

#endif // TRAINS_SRC_COMMON_INC_BLOCK_BUILDER_H