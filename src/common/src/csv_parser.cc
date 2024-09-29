#include "csv_parser.h"

#define CSVPARSER_VALUE_DELIMITER ","
#define CSVPARSER_ROW_DELIMITER "\n"

static std::vector<std::string> Split(const std::string &input_string, const std::string &delimiter);

CsvParser::CsvParser(void) {}

CsvParser::CsvParser(const std::string &input_string)
{
    Parse(input_string);
}

bool CsvParser::Parse(const std::string &input_string)
{
    bool parsed = true;

    std::vector<std::string> lines = Split(input_string, CSVPARSER_ROW_DELIMITER);

    for (std::string line : lines)
    {
        records_.push_back(Split(line, CSVPARSER_VALUE_DELIMITER));
    }

    const std::size_t size = records_[0].size();
    for (std::vector<std::string> record : records_)
    {
        if (record.size() != size)
        {
            parsed = false;
        }
    }

    if (!parsed)
    {
        Reset();
    }

    return parsed;
}

std::size_t CsvParser::GetSize(void) const
{
    return records_.size();
}

std::vector<std::string> CsvParser::GetRecord(const std::size_t index) const
{
    std::vector<std::string> record;

    if (index < records_.size())
    {
        record = records_[index];
    }

    return record;
}

std::vector<std::vector<std::string>> CsvParser::GetRecords(void) const
{
    return records_;
}

void CsvParser::Reset(void)
{
    records_.clear();
}

static std::vector<std::string> Split(const std::string &input_string, const std::string &delimiter)
{
    std::vector<std::string> tokens;
    size_t index = 0;
    size_t end = input_string.find(delimiter, index);

    while (std::string::npos != end)
    {
        tokens.push_back(input_string.substr(index, end - index));

        index = end + 1;
        end = input_string.find(delimiter, index);
    }
    tokens.push_back(input_string.substr(index, input_string.length() - index));

    return tokens;
}