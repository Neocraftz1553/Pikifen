#include "vars.h"
#include "const.h"

using namespace std;

ALLEGRO_BITMAP* bmp_background = NULL;
ALLEGRO_BITMAP* bmp_blue[3] = {NULL, NULL, NULL};
ALLEGRO_BITMAP* bmp_blue_burrowed[3] = {NULL, NULL, NULL};
ALLEGRO_BITMAP* bmp_blue_idle[3] = {NULL, NULL, NULL};
ALLEGRO_BITMAP* bmp_blue_onion = NULL;
ALLEGRO_BITMAP* bmp_bubble = NULL;
ALLEGRO_BITMAP* bmp_cursor = NULL;
ALLEGRO_BITMAP* bmp_day_bubble = NULL;
ALLEGRO_BITMAP* bmp_health_bubble = NULL;
ALLEGRO_BITMAP* bmp_icon = NULL;
ALLEGRO_BITMAP* bmp_idle_glow = NULL;
ALLEGRO_BITMAP* bmp_louie = NULL;
ALLEGRO_BITMAP* bmp_louie_lying = NULL;
ALLEGRO_BITMAP* bmp_mouse_cursor = NULL;
ALLEGRO_BITMAP* bmp_move_group_arrow = NULL;
ALLEGRO_BITMAP* bmp_nectar = NULL;
ALLEGRO_BITMAP* bmp_olimar = NULL;
ALLEGRO_BITMAP* bmp_olimar_lying = NULL;
ALLEGRO_BITMAP* bmp_president = NULL;
ALLEGRO_BITMAP* bmp_president_lying = NULL;
ALLEGRO_BITMAP* bmp_red[3] = {NULL, NULL, NULL};
ALLEGRO_BITMAP* bmp_red_burrowed[3] = {NULL, NULL, NULL};
ALLEGRO_BITMAP* bmp_red_idle[3] = {NULL, NULL, NULL};
ALLEGRO_BITMAP* bmp_red_onion = NULL;
ALLEGRO_BITMAP* bmp_red_pellet[4] = {NULL, NULL, NULL, NULL};
ALLEGRO_BITMAP* bmp_shadow = NULL;
ALLEGRO_BITMAP* bmp_ship = NULL;
ALLEGRO_BITMAP* bmp_sun = NULL;
ALLEGRO_BITMAP* bmp_ub_spray = NULL;
ALLEGRO_BITMAP* bmp_us_spray = NULL;
ALLEGRO_BITMAP* bmp_yellow[3] = {NULL, NULL, NULL};
ALLEGRO_BITMAP* bmp_yellow_burrowed[3] = {NULL, NULL, NULL};
ALLEGRO_BITMAP* bmp_yellow_idle[3] = {NULL, NULL, NULL};
ALLEGRO_BITMAP* bmp_yellow_onion = NULL;

ALLEGRO_BITMAP* bmp_test = NULL;
sector test_sector;
vector<linedef> test_linedefs;

sample_struct sfx_camera;
sample_struct sfx_dismiss;
sample_struct sfx_louie_whistle;
sample_struct sfx_louie_name_call;
sample_struct sfx_olimar_whistle;
sample_struct sfx_olimar_name_call;
sample_struct sfx_president_whistle;
sample_struct sfx_president_name_call;
sample_struct sfx_pikmin_called;
sample_struct sfx_pikmin_held;
sample_struct sfx_pikmin_plucked;
sample_struct sfx_pikmin_thrown;
sample_struct sfx_switch_pikmin;
sample_struct sfx_throw;

vector<vector<ALLEGRO_BITMAP*>>
                             area_images;
float                 area_x1 = 0;
float                 area_y1 = 0;
float                 auto_pluck_input_time = 0;
vector<unsigned int>  berries;
ALLEGRO_BITMAP*       bmp_error;
float                 cam_trans_pan_final_x = 0;
float                 cam_trans_pan_final_y = 0;
float                 cam_trans_pan_initi_x = 0;
float                 cam_trans_pan_initi_y = 0;
float                 cam_trans_pan_time_left = 0;
float                 cam_trans_zoom_final_level = 1;
float                 cam_trans_zoom_initi_level = 1;
float                 cam_trans_zoom_time_left = 0;
float                 cam_x = 0;
float                 cam_y = 0;
float                 cam_zoom = 1;
mob*                  closest_party_member = NULL;
vector<control_info>  controls;
size_t                cur_leader_nr = 0;
unsigned char         cur_screen = SCREEN_GAME;
weather               cur_weather;
float                 cursor_angle = 0;
float                 cursor_x = 0;
float                 cursor_y = 0;
unsigned int          day = 0;
float                 day_minutes = 60 * 12; //ToDo set to the start of the day.
float                 day_minutes_end = 60 * 19;
float                 day_minutes_per_irl_sec = 2;
float                 day_minutes_start = 60 * 7;
bool                  daylight_effect = true;
bool                  editor_holding_m2 = false;
ALLEGRO_FONT*         font = NULL;
ALLEGRO_FONT*         font_area_name = NULL;
unsigned short        font_h = 0;
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
vector<mob_type*>     mob_types;
vector<mob*>          mobs;
float                 mouse_cursor_x = scr_w / 2 + CURSOR_MAX_DIST;
float                 mouse_cursor_y = scr_h / 2;
float                 mouse_cursor_speed_x = 0;
float                 mouse_cursor_speed_y = 0;
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
map<pikmin_type*, unsigned long>
pikmin_in_onions;
vector<pikmin*>       pikmin_list;
map<string, pikmin_type*>
pikmin_types;
bool                  pretty_whistle = false;
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
string                total_error_log;
map<string, treasure_type*>
treasure_types;
vector<treasure*>     treasures;
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
