// #include "wayside_controller_callback_handler.h"

// #include <string>

// #include "channel.h"
// #include "launcher.h"


// namespace wayside_controller{

    
// // ------ Channels ------//

//     //block table information channels
//     Channel<std::vector<types::BlockState>> block_data_channel;

//     //authority
//     Channel<std::string> authority_channel;



// // ------ Helper Functions ------//


// // ------ Register Callbacks  ------//
// static void register_callbacks(slint::ComponentHandle<ui::WaysideControllerUi> &wayside_controller_ui);


// // ------ Callbacks and Handler Functions ------//

// //callbacks
// static inline void load_block_data_callback(slint::ComponentHandle<ui::WaysideControllerUi> &wayside_controller_ui);

// static inline void disp_sugg_auth_callback(slint::ComponentHandle<ui::WaysideControllerUi> &wayside_controller_ui);



// //backend handlers
// static void load_block_data_handler(wayside_controller::WaysideController &wc, slint::ComponentHandle<ui::WaysideControllerUi> &wayside_controller_ui);

// static void disp_sugg_auth_handler(wayside_controller::WaysideController &wc, slint::ComponentHandle<ui::WaysideControllerUi> &wayside_controller_ui);



// //setup ui function
// void setup_ui(slint::ComponentHandle<ui::WaysideControllerUi> &wayside_controller_ui){


//     //1 register callbacks
//     register_callbacks(wayside_controller_ui);


//     auto block_data = std::make_shared<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>();
//     wayside_controller_ui->set_block_data(block_data);





//     }


// //backend handler
// void backend_handler(wayside_controller::WaysideController &wc, slint::ComponentHandle<ui::WaysideControllerUi> &wayside_controller_ui){

//     load_block_data_handler(wc, wayside_controller_ui);

//     disp_sugg_auth_handler(wc, wayside_controller_ui);

// }


// static void register_callbacks(slint::ComponentHandle<ui::WaysideControllerUi> &wayside_controller_ui){


//     //repeat for each
//     wayside_controller_ui->on_load_block_data([&wayside_controller_ui]{

//         load_block_data_callback(wayside_controller_ui);

//     });

//     wayside_controller_ui->on_disp_sugg_auth([&wayside_controller_ui]{

//         disp_sugg_auth_callback(wayside_controller_ui);

//     });
// }


//     // ------ Populate Chart ------//

//     //callbakcs run on front end, handlers run on back end

//     static inline void load_block_data_callback(){

//         block_data_channel.Send(std::string(wayside_controller_ui->())); //placeholder

//     }


//     static inline void load_block_data_handler(wayside_controller::WaysideController &wc, slint::ComponentHandle<ui::WaysideControllerUi> &wayside_controller_ui){
        
        
    
//         //get block information
//         std::unordered_map<types::BlockId, WaysideBlock> block_information = wc.GetBlockConfiguration();
//         // vector to store WaysideBlock values
//         std::vector<WaysideBlock> wc_blocks;

//         for (const auto& pair : block_information) {
//             wc_blocks.push_back(pair.second);
//         } 

//        if(block_data_channel.DataAvailable()){

//             // 1 - configure + check for errors
//             wayside_controller::Error  error    = wc.Configure(wc_blocks);


//             /* //get block information
//             std::unordered_map<types::BlockId, WaysideBlock> block_information = wc.GetBlockConfiguration();
//             // vector to store WaysideBlock values
//             std::vector<WaysideBlock> wc_blocks;

//             for (const auto& pair : block_information) {
//                 wc_blocks.push_back(pair.second);
//             } 

//             */



//        }

//     }


//     static inline void disp_sugg_auth_callback(slint::ComponentHandle<ui::WaysideControllerUi> &wayside_controller_ui){

//         authority_channel.Send(std::string(wayside_controller_ui->get_sugg_auth()));

//     }

//     static void disp_sugg_auth_handler(wayside_controller::WaysideController &wc, slint::ComponentHandle<ui::WaysideControllerUi> &wayside_controller_ui){
        
//         if(authority_channel.DataAvailable()){

//             std::string authority;

//             //capture auth data from backend

//             authority = std::to_string(wc.GetCommandedSpeedAndAuthority());


//             //update ui
//             slint::ComponentWeakHandle<ui::WaysideControllerUi> weak_ui_handle(wayside_controller_ui);
//             slint::invoke_from_event_loop([weak_ui_handle, authority] () {

//                 if (auto ui = weak_ui_handle.lock()){
                    
//                     if(ui.has_value()){

//                         ////push entrey here into table component / line component
//                     }
//                 }

//             )};



//         }
//     }
