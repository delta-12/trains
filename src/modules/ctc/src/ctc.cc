/*****************************************************************************
* @file ctc.h
*
* @brief Implements the CTC backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_CTC_INC_CTC_CC
#define TRAINS_SRC_MODULES_CTC_INC_CTC_CC

#include "wayside_controller_gateway.h"
#include "ctc.h"
#include "windows.h"
#include "commdlg.h"

#include <sstream>
#include "unordered_map"
#include <iostream>


namespace ctc
{
static std::string ExtractFileName(const std::string& full_path);

/*------------------------------ Graph Builder Implementation ------------------------------*/
GraphBuilder::GraphBuilder(std::vector<types::Block> &blocks) {
    block_list_ = blocks;  
}

void GraphBuilder::BuildGreenLine(void) {
    // A and B
    graph_.AddEdge(6, 5, 100);
    graph_.AddEdge(5, 4, 100);
    graph_.AddEdge(4, 3, 100);
    graph_.AddEdge(3, 2, 100);
    graph_.AddEdge(2, 1, 100);
    graph_.AddEdge(1, 13, 150);
    graph_.AddEdge(7, 6, 100);      // C to B connection
    
    // C Bidirectional
    graph_.AddEdge(7, 8, 100);
    graph_.AddEdge(8, 9, 100);
    graph_.AddEdge(9, 10, 100);
    graph_.AddEdge(10, 11, 100);
    graph_.AddEdge(11, 12, 100);
    graph_.AddEdge(12, 13, 150);    // C to D 
    graph_.AddEdge(13, 12, 150);    // D to C connection
    
    graph_.AddEdge(12, 11, 100);
    graph_.AddEdge(11, 10, 100);
    graph_.AddEdge(10, 9, 100);
    graph_.AddEdge(9, 8, 100);
    graph_.AddEdge(8, 7, 100);
    
    // D Bidirectional
    graph_.AddEdge(13, 14, 150);
    graph_.AddEdge(14, 15, 150);
    graph_.AddEdge(15, 16, 150);
    graph_.AddEdge(16, 15, 150);
    graph_.AddEdge(15, 14, 150);
    graph_.AddEdge(14, 13, 150);
    graph_.AddEdge(16, 17, 150);    // D to E
    graph_.AddEdge(17, 16, 150);    // E to D

    // E Bidirectional
    graph_.AddEdge(17, 18, 150);
    graph_.AddEdge(18, 19, 150);
    graph_.AddEdge(19, 20, 150);
    graph_.AddEdge(20, 19, 150);
    graph_.AddEdge(19, 18, 150);
    graph_.AddEdge(18, 17, 150);
    graph_.AddEdge(20, 21, 300);    // E to F
    graph_.AddEdge(21, 20, 300);    // F to E

    // F Bidirectional
    graph_.AddEdge(21, 22, 300);
    graph_.AddEdge(22, 23, 300);
    graph_.AddEdge(23, 24, 300);
    graph_.AddEdge(24, 25, 200);
    graph_.AddEdge(25, 26, 100);
    graph_.AddEdge(26, 27, 50);
    graph_.AddEdge(27, 28, 50);

    graph_.AddEdge(28, 27, 50);
    graph_.AddEdge(27, 26, 50);
    graph_.AddEdge(26, 25, 100);
    graph_.AddEdge(25, 24, 200);
    graph_.AddEdge(24, 23, 300);
    graph_.AddEdge(23, 22, 300);
    graph_.AddEdge(22, 21, 300);

    graph_.AddEdge(28, 29, 50);     // F to G


    // G Unidirectional
    graph_.AddEdge(29, 30, 50);
    graph_.AddEdge(30, 31, 50);
    graph_.AddEdge(31, 32, 50);

    graph_.AddEdge(32, 33, 50);     // G to H

    // H Unidirectional
    graph_.AddEdge(33, 34, 50);
    graph_.AddEdge(34, 35, 50);
    graph_.AddEdge(35, 36, 50);     // H to I

    // I Unidirectional
    graph_.AddEdge(36, 37, 50);
    graph_.AddEdge(37, 38, 50);
    graph_.AddEdge(38, 39, 50);  
    graph_.AddEdge(39, 40, 50);
    graph_.AddEdge(40, 41, 50);
    graph_.AddEdge(41, 42, 50); 
    graph_.AddEdge(42, 43, 50);
    graph_.AddEdge(43, 44, 50);
    graph_.AddEdge(44, 45, 50);  
    graph_.AddEdge(45, 46, 50); 
    graph_.AddEdge(46, 47, 50);
    graph_.AddEdge(47, 48, 50);
    graph_.AddEdge(48, 49, 50); 
    graph_.AddEdge(49, 50, 50);  
    graph_.AddEdge(50, 51, 50); 
    graph_.AddEdge(51, 52, 50);
    graph_.AddEdge(52, 53, 50);
    graph_.AddEdge(53, 54, 50); 
    graph_.AddEdge(54, 55, 50); 
    graph_.AddEdge(55, 56, 50); 
    graph_.AddEdge(56, 57, 50); 

    graph_.AddEdge(57, 58, 50);     // I to J


    // J Unidirectional
    graph_.AddEdge(58, 0, 500);     // to Yard
    graph_.AddEdge(58, 59, 50);
    graph_.AddEdge(59, 60, 50);
    graph_.AddEdge(60, 61, 50);
    graph_.AddEdge(61, 62, 50);

    graph_.AddEdge(62, 63, 100);    // J to K
    graph_.AddEdge(0, 63, 50);      // Yard to K

    // K Unidirectional
    graph_.AddEdge(63, 64, 100);
    graph_.AddEdge(64, 65, 200);
    graph_.AddEdge(65, 66, 200);
    graph_.AddEdge(66, 67, 100);
    graph_.AddEdge(67, 68, 100);
    graph_.AddEdge(68, 69, 100);    // K to L

    // L Unidirectional
    graph_.AddEdge(69, 70, 100);
    graph_.AddEdge(70, 71, 100);
    graph_.AddEdge(71, 72, 100);
    graph_.AddEdge(72, 73, 100);
    graph_.AddEdge(73, 74, 100);    // L to M

    // L Unidirectional
    graph_.AddEdge(74, 75, 100);
    graph_.AddEdge(75, 76, 100);
    graph_.AddEdge(76, 77, 300);    // M to N

    // N Bidirectional
    graph_.AddEdge(77, 78, 300);
    graph_.AddEdge(78, 79, 300);
    graph_.AddEdge(79, 80, 300);  
    graph_.AddEdge(70, 81, 300); 
    graph_.AddEdge(81, 82, 300);
    graph_.AddEdge(82, 83, 300);
    graph_.AddEdge(83, 84, 300);  
    graph_.AddEdge(84, 85, 300);  

    graph_.AddEdge(85, 84, 300);
    graph_.AddEdge(84, 83, 300);
    graph_.AddEdge(83, 82, 300);  
    graph_.AddEdge(82, 81, 300); 
    graph_.AddEdge(81, 80, 300);
    graph_.AddEdge(80, 79, 300);
    graph_.AddEdge(79, 78, 300);  
    graph_.AddEdge(78, 77, 300);  

    graph_.AddEdge(77, 101, 35);   // N to R
    graph_.AddEdge(85, 86, 100);    // N to O

    // O Bidirectional 
    graph_.AddEdge(86, 87, 86.6);
    graph_.AddEdge(87, 88, 100);
    graph_.AddEdge(88, 87, 100);
    graph_.AddEdge(87, 86, 86.6);

    graph_.AddEdge(86, 85, 100);    // O to N
    graph_.AddEdge(88, 89, 75);     // O to P

    // P Unidirectional
    graph_.AddEdge(89, 90, 75);
    graph_.AddEdge(90, 91, 75);
    graph_.AddEdge(91, 92, 75);
    graph_.AddEdge(92, 93, 75);
    graph_.AddEdge(93, 94, 75);
    graph_.AddEdge(94, 95, 75);
    graph_.AddEdge(95, 96, 75);
    graph_.AddEdge(96, 97, 75);

    graph_.AddEdge(97, 98, 75); // P to Q

    // Q Unidirectional
    graph_.AddEdge(98, 99, 75);
    graph_.AddEdge(99, 100, 75);

    graph_.AddEdge(100, 85, 35);

    // R Unidirectional
    graph_.AddEdge(101, 102, 100); // R to S

    // S Unidirectional
    graph_.AddEdge(102, 103, 100);
    graph_.AddEdge(103, 104, 80);
    graph_.AddEdge(104, 105, 100); // S to T

    // T Unidirectional
    graph_.AddEdge(105, 106, 100);
    graph_.AddEdge(106, 107, 90);
    graph_.AddEdge(107, 108, 100);
    graph_.AddEdge(108, 109, 100);

    graph_.AddEdge(109, 110, 100);  // T to U

    // U Unidirectional 
    graph_.AddEdge(110, 111, 100);
    graph_.AddEdge(111, 112, 100);
    graph_.AddEdge(112, 113, 100);
    graph_.AddEdge(113, 114, 162);
    graph_.AddEdge(114, 115, 100);
    graph_.AddEdge(115, 116, 100);

    graph_.AddEdge(116, 117, 50);  // U to V

    // V Unidirectional 
    graph_.AddEdge(117, 118, 50);
    graph_.AddEdge(118, 119, 40);
    graph_.AddEdge(119, 120, 50);
    graph_.AddEdge(120, 121, 502);

    graph_.AddEdge(121, 122, 50);   // V to W

    // W Unidirectional
    graph_.AddEdge(122, 123, 50);
    graph_.AddEdge(123, 124, 50);
    graph_.AddEdge(124, 125, 50);
    graph_.AddEdge(125, 126, 50);
    graph_.AddEdge(126, 127, 50);
    graph_.AddEdge(127, 128, 50);
    graph_.AddEdge(128, 129, 50);
    graph_.AddEdge(129, 130, 50);
    graph_.AddEdge(130, 131, 50);
    graph_.AddEdge(131, 132, 50);
    graph_.AddEdge(132, 133, 50);
    graph_.AddEdge(133, 134, 50);
    graph_.AddEdge(134, 135, 50);
    graph_.AddEdge(135, 136, 50);
    graph_.AddEdge(136, 137, 50);
    graph_.AddEdge(137, 138, 50);
    graph_.AddEdge(138, 139, 50);
    graph_.AddEdge(139, 140, 50);
    graph_.AddEdge(140, 141, 50);
    graph_.AddEdge(141, 142, 50);
    graph_.AddEdge(142, 143, 50);
    graph_.AddEdge(143, 144, 50);   // W to X

    // X Unidirectional
    graph_.AddEdge(144, 145, 50);
    graph_.AddEdge(145, 146, 50);
    graph_.AddEdge(146, 147, 50);   // X to Y

    // Y Unidirectional
    graph_.AddEdge(147, 148, 184);
    graph_.AddEdge(148, 149, 40);
    graph_.AddEdge(149, 150, 35); 
    graph_.AddEdge(150, 28, 50);
}

std::vector<types::Block> GraphBuilder::BuildSection(types::SectionId section_id) {
    std::vector<types::Block> section;
    for (types::Block &block : block_list_) {
        if (block.section == section_id) {
            section.push_back(block);
        }
    }
    return section;
}

std::size_t GraphBuilder::GetBlockList(void) const {
    return block_list_.size();
}

void GraphBuilder::AddSection(std::vector<types::Block> section, types::BlockDirection direction) {
    for (size_t i = 0; i < section.size(); ++i) {
        graph_.AddEdge(section[i].block, section[i+1].block, section[i+1].length);
    }
    if (direction == types::BLOCKDIRECTION_BIDIRECTIONAL) {
        for (size_t i = section.size() - 1; i > 0; --i) {
            graph_.AddEdge(section[i].block, section[i-1].block, section[i-1].length);
        }
    }
}

Graph<types::BlockId, types::Meters> GraphBuilder::GetGraph(void) {
    return graph_;
}

/*------------------------------ Ctc Implementation ------------------------------*/
void Ctc::SetTrackLayout(void) {
    CsvParser parser(schedule_file_path_);
    BlockBuilder bb(parser.GetRecords(), Module::MODULE_TRACK_MODEL);
    std::vector<types::Block> blocks = bb.GetBlocks();
    SetBlocks(blocks);
    SetGraphLayout(blocks_);
}

void Ctc::SetGraphLayout(std::vector<types::Block> &blocks) {
    GraphBuilder gb(blocks);
    gb.BuildGreenLine();
    graph_ = gb.GetGraph();
}

void Ctc::SetGraphManually(Graph<types::BlockId, types::Meters> graph) {
    graph_ = graph;
}

std::vector<types::BlockId> Ctc::FindRoute(const types::BlockId start, const types::BlockId end) {
    return graph_.Dijkstra(start, end);
}

void Ctc::AssignAuthority(const std::vector<types::BlockId> &route, types::TrainId train_id) {
    std::queue<types::BlockId> authority(std::deque<types::BlockId>(route.begin(), route.end()));

    for (ctc::Train &train : train_schedules_) {
        if (train.train_id == train_id) {
            train.authority = authority;
        }
    }
}


/* Method: LoadSchedule()
* 1. Create CsvParser Object CsvParser(const std::filesystem::path &file_path) to get list of records
* 2. Create BlockBuilder Object to parse record information into blocks and populate ctc.blocks_ and ctc.train_schedules_
* 3. Upon receiving train schedule, tell train_model and train_controller to create according objects
* 4. Calculate authority and suggested speed for each train in train_schedules_
* 5. Calculate departure time for each train in ctc.train_schedule_
* 6. Set up timer for each train; when timer is up and match with depature time, get top block in authority queue and set suggested speed to block's speed limit
*/
void Ctc::LoadSchedule(void) {

}

types::Error Ctc::OpenFileExplorer(std::string &file) {
    types::Error error;
    OPENFILENAME ofn;       // Common dialog box structure
    char file_name[MAX_PATH]; // Buffer for the file name
    ZeroMemory(&ofn, sizeof(ofn)); // Clear the structure
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr; // If using in a GUI, set to the parent window handle
    ofn.lpstrFile = file_name;
    ofn.lpstrFile[0] = '\0';  // Initialize buffer to empty string
    ofn.nMaxFile = sizeof(file_name);
    ofn.lpstrFilter = "CSV Files\0*.CSV\0";
    ofn.nFilterIndex = 1; // Default to the first filter (Excel)
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr; // Default directory
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Open the file dialog
    if (GetOpenFileName(&ofn)) {
        SetScheduleFilePath(ofn.lpstrFile);
        file = ExtractFileName(ofn.lpstrFile);
        std::cout << "File Name: " << file << std::endl;
        error = types::Error::ERROR_NONE;
    } else {
        error = types::Error::ERROR_INVALID_FORMAT;
    }
    return error;
}

void Ctc::ManualDispatch(types::BlockId destination) {
    ctc::Train train;
    std::cout << "Train is created! Train ID: " << train.train_id << std::endl; 
    AddTrainToTrainSchedule(train);
    std::vector<types::BlockId> route = FindRoute(0, destination);
    AssignAuthority(route, train.train_id);
}

static std::string ExtractFileName(const std::string& full_path) {
    // Find the last occurrence of backslash
    size_t pos = full_path.find_last_of("\\/");
    if (pos != std::string::npos) {
        return full_path.substr(pos + 1); // Return everything after the last backslash
    } else {
        return full_path; // If no backslash is found, return the full string (it may already be a file name)
    }
}


/*------------------------------ Getters ------------------------------*/
std::filesystem::path Ctc::GetCsvPath(void) const {
    return schedule_file_path_;
}

void Ctc::SetScheduleFilePath(std::filesystem::path path) {
    schedule_file_path_ = path;
}

ctc::Train Ctc::GetTrainById(const types::TrainId train_id) const {
    ctc::Train result;
    for (ctc::Train train : train_schedules_) {
        if (train.train_id == train_id) {
            result = train;
        }
    }
    return result;
}

std::size_t Ctc::GetBlockSize(void) const {
    return blocks_.size();
}

std::vector<types::Block> Ctc::GetBlocks(void) const {
    return blocks_;
}

std::size_t Ctc::GetTrainAuthority(types::TrainId train_id) const {
    ctc::Train train = GetTrainById(train_id);
    return train.authority.size();
}

/*------------------------------ Setters ------------------------------*/
void Ctc::SetBlocks(std::vector<types::Block> &blocks) {
    types::Block yard;
    yard.block = 0;
    blocks_.push_back(yard);
    for ( types::Block &block : blocks) {
        blocks_.push_back(block);
    }
}

void Ctc::AddTrainToTrainSchedule(ctc::Train train) {
    train_schedules_.push_back(train);
}

} // namespace ctc

#endif // TRAINS_SRC_MODULES_CTC_INC_CTC_CC