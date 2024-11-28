/*****************************************************************************
* @file plc_signals.h
*
* @brief Stores the lexemes that represent the PLC input and output signals.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_PLC_SIGNALS_H
#define TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_PLC_SIGNALS_H

#include <array>

#include "wayside_controller.h"

namespace plc_compiler
{

const std::array<const char *const, wayside_controller::kTotalInputs> kInputSignals = {
    "IN_0",
    "IN_1",
    "IN_2",
    "IN_3",
    "IN_4",
    "IN_5",
    "IN_6",
    "IN_7",
    "IN_8",
    "IN_9",
    "IN_10",
    "IN_11",
    "IN_12",
    "IN_13",
    "IN_14",
    "IN_15",
    "IN_16",
    "IN_17",
    "IN_18",
    "IN_19",
    "IN_20",
    "IN_21",
    "IN_22",
    "IN_23",
    "IN_24",
    "IN_25",
    "IN_26",
    "IN_27",
    "IN_28",
    "IN_29",
    "IN_30",
    "IN_31",
    "IN_32",
    "IN_33",
    "IN_34",
    "IN_35",
    "IN_36",
    "IN_37",
    "IN_38",
    "IN_39",
    "IN_40",
    "IN_41",
    "IN_42",
    "IN_43",
    "IN_44",
    "IN_45",
    "IN_46",
    "IN_47",
    "IN_48",
    "IN_49",
    "IN_50",
    "IN_51",
    "IN_52",
    "IN_53",
    "IN_54",
    "IN_55",
    "IN_56",
    "IN_57",
    "IN_58",
    "IN_59",
    "IN_60",
    "IN_61",
    "IN_62",
    "IN_63",
    "IN_64",
    "IN_65",
    "IN_66",
    "IN_67",
    "IN_68",
    "IN_69",
    "IN_70",
    "IN_71",
    "IN_72",
    "IN_73",
    "IN_74",
    "IN_75",
    "IN_76",
    "IN_77",
    "IN_78",
    "IN_79",
    "IN_80",
    "IN_81",
    "IN_82",
    "IN_83",
    "IN_84"
};

const std::array<const char *const, wayside_controller::kTotalOutputs> kOutputSignals = {
    "OUT_0",
    "OUT_1",
    "OUT_2",
    "OUT_3",
    "OUT_4",
    "OUT_5",
    "OUT_6",
    "OUT_7",
    "OUT_8",
    "OUT_9",
    "OUT_10",
    "OUT_11",
    "OUT_12",
    "OUT_13",
    "OUT_14",
    "OUT_15",
    "OUT_16",
    "OUT_17",
    "OUT_18",
    "OUT_19",
    "OUT_20",
    "OUT_21",
    "OUT_22"
};

inline bool GetInput(const std::string &lexeme, wayside_controller::InputId &input)
{
    bool valid = true;

    for (size_t i = 0; i < kInputSignals.size(); i++)
    {
        if (lexeme == kInputSignals[i])
        {
            input = static_cast<wayside_controller::InputId>(i);
            break;
        }
    }

    return valid;
}

inline bool GetOutput(const std::string &lexeme, wayside_controller::OutputId &output)
{
    bool valid = true;

    for (size_t i = 0; i < kOutputSignals.size(); i++)
    {
        if (lexeme == kOutputSignals[i])
        {
            output = static_cast<wayside_controller::OutputId>(i);
            break;
        }
    }

    return valid;
}

} // namespace plc_compiler

#endif // TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_PLC_SIGNALS_H