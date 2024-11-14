/*****************************************************************************
* @file lookup_table.h
*
* @brief Implements a 2-way lookup table.
*****************************************************************************/
#ifndef TRAINS_SRC_COMMON_INC_LOOKUP_TABLE_H
#define TRAINS_SRC_COMMON_INC_LOOKUP_TABLE_H

#include <array>
#include <cstddef>
#include <ranges>
#include <unordered_map>
#include <vector>

template <typename Value, size_t columns>
class Row
{
    public:
        void Insert(const Value &value);
        void Set(const Value &value, const size_t column);
        bool Contains(const Value &value) const;
        void GetValues(std::vector<Value> &values) const;

    private:
        std::array<Value, columns> values_{};
        size_t count_ = 0;
};

template <typename Key, typename Value, size_t columns>
class LookupTable
{
    public:
        void Insert(const Key &key, const Value &value);
        bool GetKey(const Value &value, Key &key) const;
        bool GetValues(const Key &key, std::vector<Value> &values) const;
        bool Contains(const Key &key) const;

    private:
        std::unordered_map<Key, Row<Value, columns>> table_;
};

template <typename Value, size_t columns>
void Row<Value, columns>::Insert(const Value &value)
{
    if (!Contains(value))
    {
        values_[count_] = value;
        count_++;
    }
}

template <typename Value, size_t columns>
void Row<Value, columns>::Set(const Value &value, const size_t column)
{
    if (column < columns)
    {
        values_[column] = value;
    }
}

template <typename Value, size_t columns>
bool Row<Value, columns>::Contains(const Value &value) const
{
    return values_.end() != std::ranges::find(values_, value);
}

template <typename Value, size_t columns>
void Row<Value, columns>::GetValues(std::vector<Value> &values) const
{
    values.clear();
    values.reserve(columns);
    values.insert(values.begin(), values_.begin(), values_.begin() + count_);
}

template <typename Key, typename Value, size_t columns>
void LookupTable<Key, Value, columns>::Insert(const Key &key, const Value &value)
{
    table_[key].Insert(value);
}

template <typename Key, typename Value, size_t columns>
bool LookupTable<Key, Value, columns>::GetKey(const Value &value, Key &key) const
{
    bool key_found = false;

    for (const std::pair<const Key, Row<Value, columns>> &values : table_)
    {
        if (values.second.Contains(value))
        {
            key       = values.first;
            key_found = true;
            break;
        }
    }

    return key_found;
}

template <typename Key, typename Value, size_t columns>
bool LookupTable<Key, Value, columns>::GetValues(const Key &key, std::vector<Value> &values) const
{
    bool values_found = false;

    if (table_.contains(key))
    {
        table_.find(key)->second.GetValues(values);

        values_found = true;
    }

    return values_found;
}

template <typename Key, typename Value, size_t columns>
bool LookupTable<Key, Value, columns>::Contains(const Key &key) const
{
    return table_.contains(key);
}

#endif // TRAINS_SRC_COMMON_INC_LOOKUP_TABLE_H