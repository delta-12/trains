/*****************************************************************************
* @file ring_buffer_tests.cc
*
* @brief Unit testing for ring buffer.
*****************************************************************************/

#include <gtest/gtest.h>

#include "ring_buffer.h"

TEST(RingBufferTests, WriteTest)
{
    double                value   = 0.0;
    double                data[3] = {1.23, 4.56, 7.89};
    RingBuffer<double, 5> buffer;

    ASSERT_EQ(0, buffer.Write(nullptr, 1));

    ASSERT_EQ(1, buffer.Write(&value, 1));
    ASSERT_EQ(3, buffer.Write(data, 3));
    value = 0.0001;
    ASSERT_EQ(1, buffer.Write(&value, 1));
    ASSERT_EQ(0, buffer.Write(data, 1));

    buffer.Clear();
    ASSERT_EQ(3, buffer.Write(data, 3));
    ASSERT_EQ(1, buffer.Write(&value, 1));
    ASSERT_EQ(1, buffer.Write(&value, 1));
    ASSERT_EQ(0, buffer.Write(data, 1));

    buffer.Clear();
    ASSERT_EQ(1, buffer.Write(&value, 1));
    ASSERT_EQ(1, buffer.Write(&value, 1));
    ASSERT_EQ(1, buffer.Write(&value, 1));
    buffer.Read(&value, 1);
    ASSERT_EQ(1, buffer.Write(&value, 1));
    ASSERT_EQ(1, buffer.Write(&value, 1));
    ASSERT_EQ(1, buffer.Write(&value, 1));
    buffer.Read(&value, 1);
    ASSERT_EQ(1, buffer.Write(&value, 1));
}

TEST(RingBufferTests, ReadTest)
{
    double                value   = 0.0;
    double                data[5] = {1.23, 4.56, 7.89, 10.11, 12.13};
    double                copy[7] = {0.0};
    RingBuffer<double, 5> buffer;

    ASSERT_EQ(0, buffer.Read(&value, 1));
    buffer.Write(data, 5);
    for (const double &i : data)
    {
        ASSERT_EQ(1, buffer.Read(&value, 1));
        ASSERT_EQ(i, value);
    }

    ASSERT_EQ(0, buffer.Read(&value, 1));
    buffer.Write(data, 5);
    ASSERT_EQ(5, buffer.Read(copy, 7));
    for (size_t i = 0; i < 5; i++)
    {
        ASSERT_EQ(data[i], copy[i]);
    }

    buffer.Write(data, 1);
    ASSERT_EQ(0, buffer.Read(nullptr, 1));
    ASSERT_EQ(1, buffer.Read(&value, 1));
    ASSERT_EQ(data[0], value);
}

TEST(RingBufferTests, SizeTest)
{
    double                value;
    double                data[5] = {1.23, 4.56, 7.89, 10.11, 12.13};
    RingBuffer<double, 5> buffer;

    ASSERT_EQ(0, buffer.Size());
    buffer.Write(data, 1);
    ASSERT_EQ(1, buffer.Size());
    buffer.Write(data + 1, 4);
    ASSERT_EQ(5, buffer.Size());
    buffer.Read(&value, 1);
    ASSERT_EQ(4, buffer.Size());
    buffer.Clear();
    ASSERT_EQ(0, buffer.Size());

}

TEST(RingBufferTests, CapacityTest)
{
    RingBuffer<double, 5> buffer0;
    ASSERT_EQ(5, buffer0.Capacity());

    RingBuffer<double, 7> buffer1;
    ASSERT_EQ(7, buffer1.Capacity());

    double                 value = 0.0;
    RingBuffer<double, 10> buffer2;
    ASSERT_EQ(10, buffer2.Capacity());
    buffer2.Write(&value, 1);
    ASSERT_EQ(10, buffer2.Capacity());
    buffer2.Write(&value, 1);
    ASSERT_EQ(10, buffer2.Capacity());
    buffer2.Read(&value, 1);
    ASSERT_EQ(10, buffer2.Capacity());
}

TEST(RingBufferTests, ClearTest)
{
    double                data[5] = {1.23, 4.56, 7.89, 10.11, 12.13};
    RingBuffer<double, 5> buffer;

    ASSERT_EQ(5, buffer.Write(data, 5));
    ASSERT_EQ(0, buffer.Write(data, 5));
    buffer.Clear();
    ASSERT_EQ(0, buffer.Size());
    ASSERT_EQ(5, buffer.Write(data, 5));
}