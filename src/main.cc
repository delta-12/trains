#include <thread>

#include <slint.h>
#include "track_model.h"
#include "train_model.h"
#include "train_controller.h"
#include "ctc.h"

#include <iostream>
#include <map>



int main(void)
{
    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    CLOCK.get()->Start();
    std::filesystem::path base_path = std::filesystem::current_path();
    std::filesystem::path path3      = base_path / "tests" / "common" / "test_csv" / "green_line_schedule.csv";
    std::filesystem::path           path      = base_path / "tests" / "common" / "test_csv" / "green_line_path.csv";
    std::filesystem::path           path2     = base_path / "tests" / "common" / "test_csv" / "green_line_track_layout.csv";
    CsvParser                       parser(path);
    CsvParser                       parser2(path2);
    BlockBuilder                    bb(parser.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
    BlockBuilder                    bb2(parser2.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
    

    // // ctc::Ctc              ctc;
    track_model::SoftwareTrackModel track;
    train_model::SoftwareTrainModel TM(CLOCK);
    train_controller::SoftwareTrainController TC(CLOCK);

    std::map<int, std::vector<std::pair<std::shared_ptr<train_model::SoftwareTrainModel>,std::shared_ptr<train_controller::SoftwareTrainController>  >>> trains_map;
    std::shared_ptr<train_model::SoftwareTrainModel> TM_ptr = std::make_shared<train_model::SoftwareTrainModel>(TM);
    std::shared_ptr<train_controller::SoftwareTrainController> TC_ptr = std::make_shared<train_controller::SoftwareTrainController>(TC);



    // ctc.SetScheduleFilePath(path3);
    // ctc.SetTrackLayout();

    //int block_id = 0;
    
    //std::cout << "Enter Block Id: " << block_id;
    //std::cin >> block_id;  // Read the input from the user and store it in 'age'



    //ctc.ManualDispatch(block_id);
    //track.SetTrackLayout(types::TrackId::TRACKID_GREEN, bb.GetBlocks(), bb2.GetBlocks());
    
    // TC_ptr.get()->SetAuthority(10);
    // TC_ptr.get()->SetCommandedSpeed(12);
    

    bool x = true;
    while(x)
    {
    
        //usleep(500000);
        // TC_ptr.get()->Update();
        //std::cout << "\nCurrent Power: " << TC_ptr.get()->GetCommandedPower();

        // TM_ptr.get()->SetCommandedPower(TC_ptr.get()->GetCommandedPower());
        // TM_ptr.get()->Update();
        // TC_ptr.get()->SetCurrentSpeed(TM_ptr.get()->GetActualSpeed());

        std::cout << "\nCurrent Speed: "; //<< TM_ptr.get()->GetActualSpeed();

        // if(TM_ptr.get()->GetActualSpeed() >= 12)
        // {
        //     TC_ptr.get()->SetCommandedSpeed(5);
        // }
    }

    return 0;
}