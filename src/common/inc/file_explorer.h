/*****************************************************************************
* @file file_explorer.h
*
* @brief Implements a file explorer class.
*****************************************************************************/
#ifndef TRAINS_SRC_COMMON_INC_FILE_EXPLORER_H
#define TRAINS_SRC_COMMON_INC_FILE_EXPLORER_H

#ifdef WIN32
#include "windows.h"
#include "commdlg.h"
#undef ERROR
#undef ERROR_INVALID_BLOCK
#endif

#include <filesystem>

#include "types.h"

class FileExplorer
{
    public:
        FileExplorer(void);
        std::filesystem::path GetPath(void) const;
        std::string GetFileName(void) const;

    private:
        void ExtractFileName(const std::string& full_path);
        types::Error OpenFileExplorer(void);
        std::filesystem::path path_;
        std::string file_name_;
};

#endif // TRAINS_SRC_COMMON_INC_FILE_EXPLORER_H