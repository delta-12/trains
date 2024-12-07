/*****************************************************************************
* @file plc_compiler.h
*
* @brief Implements top-level control logic for the PLC program compiler.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_PLC_COMPILER_H
#define TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_PLC_COMPILER_H

#include <filesystem>
#include <ostream>

#include "types.h"

namespace plc_compiler
{

types::Error Compile(const std::filesystem::path &file_path, std::ostream &stream);

} // namespace plc_compiler

#endif // TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_PLC_COMPILER_H