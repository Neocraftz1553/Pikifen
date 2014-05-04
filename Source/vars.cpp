#include <limits.h>

#include "vars.h"
#include "const.h"

using namespace std;

ALLEGRO_BITMAP* bmp_blue_onion = NULL;
ALLEGRO_BITMAP* bmp_bubble = NULL;
ALLEGRO_BITMAP* bmp_cloaking_burrow_nit = NULL;
ALLEGRO_BITMAP* bmp_cursor = NULL;
ALLEGRO_BITMAP* bmp_day_bubble = NULL;
ALLEGRO_BITMAP* bmp_enemy_spirit = NULL;
ALLEGRO_BITMAP* bmp_hard_bubble = NULL;
ALLEGRO_BITMAP* bmp_icon = NULL;
ALLEGRO_BITMAP* bmp_idle_glow = NULL;
ALLEGRO_BITMAP* bmp_message_box = NULL;
ALLEGRO_BITMAP* bmp_mouse_cursor = NULL;
ALLEGRO_BITMAP* bmp_mouse_cursor_invalid = NULL;
ALLEGRO_BITMAP* bmp_move_group_arrow = NULL;
ALLEGRO_BITMAP* bmp_nectar = NULL;
ALLEGRO_BITMAP* bmp_number_bubble = NULL;
ALLEGRO_BITMAP* bmp_pikmin_spirit = NULL;
ALLEGRO_BITMAP* bmp_red_onion = NULL;
ALLEGRO_BITMAP* bmp_red_pellet[4] = {NULL, NULL, NULL, NULL};
ALLEGRO_BITMAP* bmp_shadow = NULL;
ALLEGRO_BITMAP* bmp_ship = NULL;
ALLEGRO_BITMAP* bmp_smack = NULL;
ALLEGRO_BITMAP* bmp_smoke = NULL;
ALLEGRO_BITMAP* bmp_sparkle = NULL;
ALLEGRO_BITMAP* bmp_sun = NULL;
ALLEGRO_BITMAP* bmp_sun_bubble = NULL;
ALLEGRO_BITMAP* bmp_tp = NULL;
ALLEGRO_BITMAP* bmp_ub_spray = NULL;
ALLEGRO_BITMAP* bmp_us_spray = NULL;
ALLEGRO_BITMAP* bmp_yellow_onion = NULL;

ALLEGRO_BITMAP* bmp_test = NULL;
sector test_sector;
vector<linedef> test_linedefs;

sample_struct sfx_attack;
sample_struct sfx_camera;
sample_struct sfx_dismiss;
sample_struct sfx_louie_whistle;
sample_struct sfx_louie_name_call;
sample_struct sfx_olimar_whistle;
sample_struct sfx_olimar_name_call;
sample_struct sfx_president_whistle;
sample_struct sfx_president_name_call;
sample_struct sfx_pikmin_attack;
sample_struct sfx_pikmin_called;
sample_struct sfx_pikmin_carrying;
sample_struct sfx_pikmin_carrying_grab;
sample_struct sfx_pikmin_caught;
sample_struct sfx_pikmin_dying;
sample_struct sfx_pikmin_held;
sample_struct sfx_pikmin_idle;
sample_struct sfx_pikmin_pluck;
sample_struct sfx_pikmin_plucked;
sample_struct sfx_pikmin_thrown;
sample_struct sfx_switch_pikmin;
sample_struct sfx_throw;

vector<vector<ALLEGRO_BITMAP*>> area_images;
float                 area_x1 = 0;
float                 area_y1 = 0;
float                 auto_pluck_input_time = 0;
vector<unsigned int>  berries;
bmp_manager           bitmaps;
ALLEGRO_BITMAP*       bmp_error;
float                 cam_trans_pan_final_x = 0;
float                 cam_trans_pan_final_y = 0;
float                 cam_trans_pan_initial_x = 0;
float                 cam_trans_pan_initial_y = 0;
float                 cam_trans_pan_time_left = 0;
float                 cam_trans_zoom_final_level = 1;
float                 cam_trans_zoom_initial_level = 1;
float                 cam_trans_zoom_time_left = 0;
float                 cam_x = 0;
float                 cam_y = 0;
float                 cam_zoom = 1;
mob*                  closest_party_member = NULL;
vector<control_info>  controls;
area_map              cur_area_map;
size_t                cur_leader_nr = 0;
string                cur_message;
size_t                cur_message_char = 0;
float                 cur_message_char_time = 0;
size_t                cur_message_section = 0;
ALLEGRO_BITMAP*       cur_message_speaker = NULL;
vector<size_t>        cur_message_stopping_chars;
unsigned char         cur_screen = SCREEN_GAME;
weather               cur_weather;
float                 cursor_angle = 0;
float                 cursor_invalid_effect = 0;
float                 cursor_save_time = CURSOR_SAVE_INTERVAL;
float                 cursor_spin_angle = 0;
vector<point>         cursor_spots;
float                 cursor_x = 0;
float                 cursor_y = 0;
unsigned int          day = 1;
float                 day_minutes = 60 * 12; //ToDo set to the start of the day.
float                 day_minutes_end = 60 * 19;
float                 day_minutes_per_irl_sec = 2;
float                 day_minutes_start = 60 * 7;
bool                  daylight_effect = true;
double                delta_t = 0;
ALLEGRO_DISPLAY*      display = NULL;
bool                  draw_cursor_trail = true;
double                prev_frame_time = 0;
animation_set         ed_anims;
animation*            ed_cur_anim = NULL;
bool                  ed_anim_playing = false;
frame*                ed_cur_frame = NULL;
size_t                ed_cur_frame_instance_nr = string::npos;
float                 ed_cur_frame_time = 0;
hitbox*               ed_cur_hitbox = NULL;
float                 ed_cur_hitbox_alpha = 0;
size_t                ed_cur_hitbox_instance_nr = string::npos;
string                ed_filename;
size_t                ed_grabbing_hitbox = string::npos;
bool                  ed_grabbing_hitbox_edge = false;
float                 ed_grabbing_hitbox_x;
float                 ed_grabbing_hitbox_y;
lafi_gui*             ed_gui = NULL;
bool                  ed_hitboxes_visible = true;
bool                  ed_holding_m2 = false;
unsigned char         ed_maturity = 0;
unsigned char         ed_mob_type_list = 0;
unsigned char         ed_mode = EDITOR_MODE_MAIN;
float                 ed_new_hitbox_corner_x = FLT_MAX;
float                 ed_new_hitbox_corner_y = FLT_MAX;
string                ed_object_name;
ALLEGRO_BITMAP*       ed_top_bmp[3] = { NULL, NULL, NULL };
map<string, enemy_type*>
enemy_types;
vector<enemy*>        enemies;
ALLEGRO_FONT*         font = NULL;
ALLEGRO_FONT*         font_area_name = NULL;
ALLEGRO_FONT*         font_counter = NULL;
unsigned int          font_counter_h = 0;
unsigned int          font_h = 0;
ALLEGRO_FONT*         font_value = NULL;
unsigned char         game_fps = DEF_FPS;
vector<vector<float> >
group_spots_x;
vector<vector<float> >
group_spots_y;
float                 idle_glow_angle = 0;
mob_type*             info_spot_mob_type = NULL;
vector<info_spot*>    info_spots;
map<ALLEGRO_JOYSTICK*, int>
joystick_numbers;
vector<leader*>       leaders;
float                 leader_move_x = 0;
float                 leader_move_y = 0;
map<string, leader_type*>
leader_types;
unsigned              max_pikmin_in_field = 100;
ALLEGRO_MIXER*        mixer = NULL;
vector<mob*>          mobs;
float                 mouse_cursor_x = scr_w / 2 + CURSOR_MAX_DIST;
float                 mouse_cursor_y = scr_h / 2;
float                 mouse_cursor_speed_x = 0;
float                 mouse_cursor_speed_y = 0;
bool                  mouse_cursor_valid = true;
bool                  mouse_moves_cursor[4] = {true, false, false, false};
vector<float>         move_group_arrows;
float                 move_group_next_arrow_time = 0;
float                 moving_group_angle = 0;
float                 moving_group_intensity = 0;
float                 moving_group_pos_x = 0;
float                 moving_group_pos_y = 0;
bool                  moving_group_to_cursor = false;
mob_type*             nectar_mob_type = NULL;
vector<nectar*>       nectars;
bool                  no_error_logs_today = true;
map<string, onion_type*>
onion_types;
vector<onion*>        onions;
unsigned char         particle_quality = 2;
vector<particle>      particles;
bool                  paused = false;
map<string, pellet_type*>
pellet_types;
vector<pellet*>       pellets;
vector<point>         percipitation;
float                 percipitation_time_left = 0;
unsigned char         pikmin_ai_portion = 0; //On this frame, handle the nth portion of the Pikmin's AI. There are N_PIKMIN_AI_PORTIONS;
map<pikmin_type*, unsigned long>
pikmin_in_onions;
vector<pikmin*>       pikmin_list;
map<string, pikmin_type*>
pikmin_types;
bool                  pretty_whistle = true;
float                 prev_moving_group_intensity = 0;
bool                  running = true;
unsigned short        scr_h = DEF_SCR_H;
unsigned short        scr_w = DEF_SCR_W;
vector<sector>        sectors;
unsigned int          selected_spray = 0;
unsigned char         ship_beam_ring_color[3] = {0, 0, 0};
bool                  ship_beam_ring_color_up[3] = {true, true, true};
mob_type*             ship_mob_type = NULL;
vector<ship*>         ships;
bool                  smooth_scaling = true;
vector<unsigned long> spray_amounts;
vector<spray_type>    spray_types;
vector<status>        statuses;
float                 sun_meter_sun_angle = 0;
float                 throw_particle_timer = 0;
map<string, treasure_type*>
treasure_types;
vector<treasure*>     treasures;
ALLEGRO_VOICE*        voice = NULL;
map<string, weather>  weather_conditions;
float                 whistle_dot_offset = 0;
float                 whistle_dot_radius[6] = { -1, -1, -1, -1, -1, -1};
float                 whistle_fade_radius = 0;
float                 whistle_fade_time = 0;
float                 whistle_max_hold = 0;
float                 whistle_next_dot_time = 0;
float                 whistle_next_ring_time = 0;
float                 whistle_radius = 0;
vector<unsigned char> whistle_ring_colors;
unsigned char         whistle_ring_prev_color = 0;
vector<float>         whistle_rings;
bool                  whistling = false;
int                   window_x = INT_MAX;
int                   window_y = INT_MAX;
