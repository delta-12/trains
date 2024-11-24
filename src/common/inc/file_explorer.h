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

FileExplorer::FileExplorer(void)
{
    OpenFileExplorer();
}

types::Error FileExplorer::OpenFileExplorer(void)
{
    types::Error error = types::Error::ERROR_NONE;
#ifdef WIN32
    OPENFILENAME ofn;                 // Common dialog box structure
    char         file_name[MAX_PATH]; // Buffer for the file name
    ZeroMemory(&ofn, sizeof(ofn));    // Clear the structure
    ofn.lStructSize     = sizeof(ofn);
    ofn.hwndOwner       = nullptr; // If using in a GUI, set to the parent window handle
    ofn.lpstrFile       = file_name;
    ofn.lpstrFile[0]    = '\0'; // Initialize buffer to empty string
    ofn.nMaxFile        = sizeof(file_name);
    ofn.lpstrFilter     = "CSV Files\0*.CSV\0";
    ofn.nFilterIndex    = 1; // Default to the first filter (Excel)
    ofn.lpstrFileTitle  = nullptr;
    ofn.nMaxFileTitle   = 0;
    ofn.lpstrInitialDir = nullptr; // Default directory
    ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Open the file dialog
    if (GetOpenFileName(&ofn))
    {
        path_ = ofn.lpstrFile;
        ExtractFileName(ofn.lpstrFile);
    }
    else
    {
        error = types::Error::ERROR_INVALID_FORMAT;
    }
#else
    std::cout << "Operation not supported" << std::endl;
    error = types::Error::ERROR_INVALID_FORMAT;
#endif
    return error;
}

void FileExplorer::ExtractFileName(const std::string& full_path)
{
    // Find the last occurrence of backslash
    size_t pos = full_path.find_last_of("\\/");
    if (pos != std::string::npos)
    {
        file_name_ = full_path.substr(pos + 1); // Return everything after the last backslash
    }
    else
    {
        file_name_ = full_path; // If no backslash is found, return the full string (it may already be a file name)
    }
}

std::filesystem::path FileExplorer::GetPath(void) const
{
    return path_;
}

std::string FileExplorer::GetFileName(void) const
{
    return file_name_;
}

#endif // TRAINS_SRC_COMMON_INC_FILE_EXPLORER_H