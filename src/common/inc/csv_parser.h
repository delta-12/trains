/*****************************************************************************
 * @file csv_parser.h
 *
 * @brief Implements a basic CSV string parser.
 *****************************************************************************/
#ifndef TRAINS_SRC_COMMON_INC_CSV_PARSER_H
#define TRAINS_SRC_COMMON_INC_CSV_PARSER_H

#include <string>
#include <vector>

class CsvParser
{
public:
    CsvParser(void);
    CsvParser(const std::string &input_string);
    bool Parse(const std::string &input_string);
    std::size_t GetSize(void) const;
    std::vector<std::string> GetRecord(const std::size_t index) const;
    std::vector<std::vector<std::string>> GetRecords(void) const;
    void Reset(void);

private:
    std::vector<std::vector<std::string>> records_;
};

#endif // TRAINS_SRC_COMMON_INC_CSV_PARSER_H