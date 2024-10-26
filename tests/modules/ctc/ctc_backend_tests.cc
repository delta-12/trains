/*****************************************************************************
* @file ctc_backend_tests.cc
*
* @brief Unit testing for Ctc class.
*****************************************************************************/

#include <gtest/gtest.h>

#include "ctc.h"

TEST(CtcBackEndTest, SetTrackLayout)
{
    ctc::Ctc              ctc;
    std::filesystem::path base_path = std::filesystem::current_path();
    std::filesystem::path path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line.csv";
    ctc.SetScheduleFilePath(path);
    ctc.SetTrackLayout();

    ASSERT_EQ(ctc.GetBlockById(1).block, 1);
    ASSERT_EQ(ctc.GetBlockById(2).block, 2);
    ASSERT_EQ(ctc.GetBlockById(3).block, 3);
    ASSERT_EQ(ctc.GetBlockById(149).block, 149);
    ASSERT_EQ(ctc.GetBlockById(150).block, 150);

    ASSERT_EQ(ctc.GetBlockById(2).station_name, "Pioneer");
    ASSERT_EQ(ctc.GetBlockById(22).station_name, "Whited");
    ASSERT_EQ(ctc.GetBlockById(31).station_name, "South bank");
    ASSERT_EQ(ctc.GetBlockById(73).station_name, "Dormont");
}

TEST(CtcBackEndTest, SetStations)
{
    ctc::Ctc              ctc;
    std::filesystem::path base_path = std::filesystem::current_path();
    std::filesystem::path path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line.csv";
    ctc.SetScheduleFilePath(path);
    ctc.SetTrackLayout();

    ASSERT_EQ(ctc.GetNumStation(), 18);
}

