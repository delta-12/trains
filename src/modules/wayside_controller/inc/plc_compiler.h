/*****************************************************************************
* @file plc_compiler.h
*
* @brief Implements top-level control logic for the PLC program compiler.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_PLC_COMPILER_H
#define TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_PLC_COMPILER_H

#include <filesystem>

namespace plc_compiler
{

bool Compile(std::filesystem::path &file_path);

} // namespace plc_compiler

#endif // TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_PLC_COMPILER_H