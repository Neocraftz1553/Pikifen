/*
 * Copyright (c) Andre 'Espyo' Silva 2013.
 * The following source file belongs to the open-source project Pikifen.
 * Please read the included README and LICENSE files for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Global game-related functions.
 */

#include <algorithm>

#include <allegro5/allegro_native_dialog.h>

#include "game.h"

#include "drawing.h"
#include "functions.h"
#include "imgui/imgui_impl_allegro5.h"
#include "init.h"
#include "load.h"


/* ----------------------------------------------------------------------------
 * Constructor for the game class.
 */
game_class::game_class() :
    bitmaps(""),
    bmp_error(nullptr),
    delta_t(0.0),
    display(nullptr),
    errors_reported_so_far(0),
    framerate_last_avg_point(0),
    is_game_running(true),
    loading_subtext_bmp(nullptr),
    loading_text_bmp(nullptr),
    mixer(nullptr),
    perf_mon(nullptr),
    show_system_info(false),
    textures(TEXTURES_FOLDER_NAME),
    win_fullscreen(options_struct::DEF_WIN_FULLSCREEN),
    win_h(options_struct::DEF_WIN_H),
    win_w(options_struct::DEF_WIN_W),
    voice(nullptr),
    cur_state(nullptr),
    logic_queue(nullptr),
    logic_timer(nullptr),
    reset_delta_t(true) {
    
    team_internal_names[MOB_TEAM_NONE] = "none";
    team_internal_names[MOB_TEAM_PLAYER_1] = "player_1";
    team_internal_names[MOB_TEAM_PLAYER_2] = "player_2";
    team_internal_names[MOB_TEAM_PLAYER_3] = "player_3";
    team_internal_names[MOB_TEAM_PLAYER_4] = "player_4";
    team_internal_names[MOB_TEAM_ENEMY_1] = "enemy_1";
    team_internal_names[MOB_TEAM_ENEMY_2] = "enemy_2";
    team_internal_names[MOB_TEAM_ENEMY_3] = "enemy_3";
    team_internal_names[MOB_TEAM_ENEMY_4] = "enemy_4";
    team_internal_names[MOB_TEAM_OBSTACLE] = "obstacle";
    team_internal_names[MOB_TEAM_OTHER] = "other";
    
    team_names[MOB_TEAM_NONE] = "None";
    team_names[MOB_TEAM_PLAYER_1] = "Player 1";
    team_names[MOB_TEAM_PLAYER_2] = "Player 2";
    team_names[MOB_TEAM_PLAYER_3] = "Player 3";
    team_names[MOB_TEAM_PLAYER_4] = "Player 4";
    team_names[MOB_TEAM_ENEMY_1] = "Enemy 1";
    team_names[MOB_TEAM_ENEMY_2] = "Enemy 2";
    team_names[MOB_TEAM_ENEMY_3] = "Enemy 3";
    team_names[MOB_TEAM_ENEMY_4] = "Enemy 4";
    team_names[MOB_TEAM_OBSTACLE] = "Obstacle";
    team_names[MOB_TEAM_OTHER] = "Other";
}


/* ----------------------------------------------------------------------------
 * Changes to a different game state.
 * new_state:
 *   State to change to.
 */
void game_class::change_state(game_state* new_state) {

    if(cur_state) {
        cur_state->unload();
    }
    cur_state = new_state;
    cur_state->load();
    
    //Because during the loading screens, there is no activity, on the
    //next frame, the game will assume the time between that and the last
    //non-loading frame is normal. This could be something like 2 seconds.
    //Let's reset the delta_t, then.
    reset_delta_t = true;
}


/* ----------------------------------------------------------------------------
 * Returns the name of the current state.
 */
string game_class::get_cur_state_name() const {
    if(cur_state) {
        return cur_state->get_name();
    }
    return "none";
}


/* ----------------------------------------------------------------------------
 * The main loop of the program. Processes events, ticks frames of gameplay,
 * etc.
 */
void game_class::main_loop() {
    //Used to calculate the time difference between the current and last frames.
    double prev_frame_time = 0.0;
    ALLEGRO_EVENT ev;
    
    //Main loop.
    al_start_timer(logic_timer);
    while(is_game_running) {
    
        /*  ************************************************
          *** | _ |                                  | _ | ***
        *****  \_/           EVENT HANDLING           \_/  *****
          *** +---+                                  +---+ ***
            ************************************************/
        
        al_wait_for_event(logic_queue, &ev);
        
        cur_state->handle_allegro_event(ev);
        
        switch(ev.type) {
        case ALLEGRO_EVENT_TIMER: {
            if(al_is_event_queue_empty(logic_queue)) {
                double cur_time = al_get_time();
                if(reset_delta_t) {
                    //Failsafe.
                    prev_frame_time = cur_time - 1.0f / options.target_fps;
                    reset_delta_t = false;
                }
                
                //Anti speed-burst cap.
                delta_t = std::min(cur_time - prev_frame_time, 0.2);
                
                game_state* prev_state = cur_state;
                
                cur_state->do_logic();
                if(cur_state == prev_state) {
                    //Only draw if we didn't change states in the meantime.
                    cur_state->do_drawing();
                }
                
                prev_frame_time = cur_time;
            }
            break;
            
        } case ALLEGRO_EVENT_DISPLAY_CLOSE: {
            is_game_running = false;
            break;
            
        } case ALLEGRO_EVENT_KEY_DOWN: {
            if(ev.keyboard.keycode == ALLEGRO_KEY_F12) {
                save_screenshot();
            }
            break;
            
        }
        }
    }
}


/* ----------------------------------------------------------------------------
 * Shuts down the program, cleanly freeing everything.
 */
void game_class::shutdown() {
    if(perf_mon) {
        perf_mon->save_log();
    }
    
    if(cur_state) {
        cur_state->unload();
    }
    unload_misc_resources();
    destroy_mob_categories();
    states.destroy();
    destroy_misc();
    destroy_event_things(logic_timer, logic_queue);
    destroy_allegro();
}


/* ----------------------------------------------------------------------------
 * Starts up the program, setting up everything that's necessary.
 * Returns 0 if everything is okay, otherwise a return code to quit the
 * program with.
 */
int game_class::start() {
    //Allegro initializations.
    init_allegro();
    
    //Panic check: is there a Game_data folder?
    if(folder_to_vector(GAME_DATA_FOLDER_PATH, true).empty()) {
        show_message_box(
            NULL, "Game_data folder not found!",
            "Game_data folder not found!",
            "Could not find the \"Game_data\" folder! "
            "If you are running the engine from a zip file, "
            "you have to unpack it first.",
            NULL,
            ALLEGRO_MESSAGEBOX_ERROR
        );
        return -1;
    }
    
    //Essentials.
    init_essentials();
    states.init();
    
    //Controls and options.
    init_controls();
    load_options();
    save_options();
    
    //Event stuff.
    init_event_things(logic_timer, logic_queue);
    
    //Other fundamental initializations and loadings.
    init_misc();
    init_error_bitmap();
    load_asset_file_names();
    load_fonts();
    load_misc_graphics();
    load_system_animations();
    load_misc_sounds();
    
    //Draw the basic loading screen.
    draw_loading_screen("", "", 1.0);
    al_flip_display();
    
    //Init Dear ImGui.
    init_dear_imgui();
    
    //Init and load some engine things.
    init_mob_actions();
    init_mob_categories();
    init_sector_types();
    init_hud_items();
    load_game_config();
    load_maker_tools();
    save_maker_tools();
    
    if(game.maker_tools.use_perf_mon) {
        game.perf_mon = new performance_monitor_struct();
    }
    
    if(
        game.maker_tools.enabled &&
        game.maker_tools.auto_start_mode == "play" &&
        !game.maker_tools.auto_start_option.empty()
    ) {
        game.states.gameplay_st->area_to_load =
            game.maker_tools.auto_start_option;
        game.change_state(game.states.gameplay_st);
    } else if(
        game.maker_tools.enabled &&
        game.maker_tools.auto_start_mode == "animation_editor"
    ) {
        game.states.animation_editor_st->auto_load_anim =
            game.maker_tools.auto_start_option;
        game.change_state(game.states.animation_editor_st);
    } else if(
        game.maker_tools.enabled &&
        game.maker_tools.auto_start_mode == "area_editor"
    ) {
        game.states.area_editor_st->auto_load_area =
            game.maker_tools.auto_start_option;
        game.change_state(game.states.area_editor_st);
    } else {
        game.change_state(game.states.main_menu_st);
    }
    
    return 0;
}


/* ----------------------------------------------------------------------------
 * Creates a game state list struct.
 */
game_state_list::game_state_list() :
    animation_editor_st(nullptr),
    area_editor_st(nullptr),
    area_menu_st(nullptr),
    controls_menu_st(nullptr),
    gameplay_st(nullptr),
    main_menu_st(nullptr),
    options_menu_st(nullptr) {
    
}


/* ----------------------------------------------------------------------------
 * Destroys the states in the list.
 */
void game_state_list::destroy() {
    delete animation_editor_st;
    delete area_menu_st;
    delete controls_menu_st;
    delete gameplay_st;
    delete main_menu_st;
    delete options_menu_st;
    
    animation_editor_st = NULL;
    area_menu_st = NULL;
    controls_menu_st = NULL;
    gameplay_st = NULL;
    main_menu_st = NULL;
    options_menu_st = NULL;
}


/* ----------------------------------------------------------------------------
 * Initializes the states in the list.
 */
void game_state_list::init() {
    animation_editor_st = new animation_editor();
    area_editor_st = new area_editor();
    area_menu_st = new area_menu();
    controls_menu_st = new controls_menu();
    gameplay_st = new gameplay();
    main_menu_st = new main_menu();
    options_menu_st = new options_menu();
}


game_class game;
