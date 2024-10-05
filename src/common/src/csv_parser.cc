#include "csv_parser.h"

#include <fstream>

#define CSVPARSER_VALUE_DELIMITER ","
#define CSVPARSER_ROW_DELIMITER   "\n"

static std::vector<std::string> Split(const std::string &input_string, const std::string &delimiter);

CsvParser::CsvParser(void)
{
}

CsvParser::CsvParser(const std::filesystem::path &file_path)
{
    Parse(file_path);
}

CsvParser::CsvParser(const std::stringstream &string_stream)
{
    Parse(string_stream);
}

CsvParser::CsvParser(const std::string &input_string)
{
    Parse(input_string);
}

types::Error CsvParser::Parse(const std::filesystem::path &file_path)
{
    types::Error error = types::ERROR_IO;

    std::ifstream file(file_path);

    if (file.is_open())
    {
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        error = Parse(buffer);
    }

    return error;
}

types::Error CsvParser::Parse(const std::stringstream &string_stream)
{
    return Parse(string_stream.str());
}

types::Error CsvParser::Parse(const std::string &input_string)
{
    types::Error error = types::ERROR_NONE;

    Reset();

    std::vector<std::string> lines = Split(input_string, CSVPARSER_ROW_DELIMITER);

    for (const std::string &line : lines)
    {
        records_.push_back(Split(line, CSVPARSER_VALUE_DELIMITER));
    }

    const std::size_t size = records_[0].size();
    for (const std::vector<std::string> &record : records_)
    {
        if (record.size() != size)
        {
            error = types::ERROR_INVALID_FORMAT;
        }
    }

    if (types::ERROR_NONE != error)
    {
        Reset();
    }

    return error;
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

std::vector<std::vector<std::string> > CsvParser::GetRecords(void) const
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
    size_t                   index = 0;
    size_t                   end   = input_string.find(delimiter, index);

    while (std::string::npos != end)
    {
        tokens.push_back(input_string.substr(index, end - index));

        index = end + 1;
        end   = input_string.find(delimiter, index);
    }
    tokens.push_back(input_string.substr(index, input_string.length() - index));

    return tokens;
}