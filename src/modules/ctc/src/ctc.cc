/*****************************************************************************
* @file ctc.h
*
* @brief Implements the CTC backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_CTC_INC_CTC_CC
#define TRAINS_SRC_MODULES_CTC_INC_CTC_CC

#include "wayside_controller_gateway.h"
#include "ctc.h"

#include <sstream>
#include "unordered_map"
#include <iostream>


namespace ctc
{

std::vector<std::string> SplitBySemicolon(const std::string& input)
{
    std::vector<std::string> result;
    std::stringstream        ss(input);
    std::string              token;

    // Split by ';' and push each part into the result vector
    while (std::getline(ss, token, ';'))
    {
        // Optionally, trim whitespace around the token
        token.erase(0, token.find_first_not_of(" \t\n\r")); // Trim leading whitespace
        token.erase(token.find_last_not_of(" \t\n\r") + 1); // Trim trailing whitespace

        result.push_back(token);
    }

    return result;
}

} // namespace ctc

#endif // TRAINS_SRC_MODULES_CTC_INC_CTC_CC