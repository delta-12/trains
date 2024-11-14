#include"track_model.h"
#include "train_model.h"
#include "train_controller.h"
#include "wayside_controller.h"

#include <gtest/gtest.h>
#include <memory>
#include <map>
#include <vector>
#include <utility>

#include "csv_parser.h"
#include "block_builder.h"
#include "train_model.h"
#include "types.h"
#include "ctc.h"





TEST(IntegrationTests, GreenLine)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    // ctc::Ctc              ctc;
    // std::filesystem::path base_path = std::filesystem::current_path();
    // std::filesystem::path path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_schedule.csv";
    // ctc.SetScheduleFilePath(path);
    // ctc.SetTrackLayout();
    std::filesystem::path           base_path = std::filesystem::current_path();
    std::filesystem::path           path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_path.csv";
    std::filesystem::path           path2     = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_track_layout.csv";
    CsvParser                       parser(path);
    CsvParser                       parser2(path2);
    BlockBuilder                    bb(parser.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
    BlockBuilder                    bb2(parser2.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
    
    
    // ctc.ManualDispatch(70);

    // auto x = ctc.GetSuggestedSpeedsAndAuthorities();
    // x[0].authority;
    // x[0].speed;
    // std::vector<types::BlockState> block_states;
    // types::BlockState              block_state_1(63, true, false);
    // block_states.push_back(block_state_1);
    // ctc.SetBlockStates(types::TrackId::TRACKID_GREEN, block_states);
    // auto x = ctc.GetSuggestedSpeedsAndAuthorities();

    track_model::SoftwareTrackModel track;
    track.SetTrackLayout(types::TrackId::TRACKID_GREEN, bb.GetBlocks(), bb2.GetBlocks());
    train_model::SoftwareTrainModel TM(CLOCK);
    train_controller::SoftwareTrainController TC(CLOCK);
    
    

    std::map<int, std::vector<std::pair<std::shared_ptr<train_model::SoftwareTrainModel>,std::shared_ptr<train_controller::SoftwareTrainController>  >>> trains_map;

    std::shared_ptr<train_model::SoftwareTrainModel> TM_ptr = std::make_shared<train_model::SoftwareTrainModel>(TM);
    std::shared_ptr<train_controller::SoftwareTrainController> TC_ptr = std::make_shared<train_controller::SoftwareTrainController>(TC);
    trains_map[1].push_back({TM_ptr, TC_ptr});

    TM_ptr.get()->SetAuthority(2);

    track.AddTrainModel(trains_map[1][0].first);

    ASSERT_EQ(track.GetTrainModel(1).get()->GetAuthority(),2);

}