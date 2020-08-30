/*
 * Copyright (c) Andre 'Espyo' Silva 2013.
 * The following source file belongs to the open-source project Pikifen.
 * Please read the included README and LICENSE files for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Header for the control-related classes and functions.
 * This refers to both hardware input receiving
 * and the corresponding in-game actions.
 */

#ifndef CONTROLS_INCLUDED
#define CONTROLS_INCLUDED

#include <functional>
#include <string>
#include <vector>

#include <allegro5/allegro.h>


using std::size_t;
using std::string;
using std::vector;


/* ----------------------------------------------------------------------------
 * This holds information over a user-specified
 * control. It has info over what hardware input
 * is required for this in-game control,
 * and what action it triggers.
 */
struct control_info {
    //Action number. Use BUTTON_*.
    unsigned char action;
    //Type of control (hardware). Use CONTROL_TYPE_*.
    unsigned char type;
    //Device number. i.e. the gamepad number.
    int device_nr;
    //Button, whether the gamepad digital button, or the keyboard key.
    int button;
    //Stick on the gamepad.
    int stick;
    //Axis of the stick.
    int axis;
    
    control_info(unsigned char action, const string &s);
    string stringify() const;
};


struct action_from_event {
    size_t button;
    float pos;
    size_t player;
    action_from_event(
        const size_t button, const float pos, const size_t player
    ) :
        button(button),
        pos(pos),
        player(player) { }
};


vector<action_from_event> get_actions_from_event(const ALLEGRO_EVENT &ev);


enum BUTTONS {
    BUTTON_NONE,
    BUTTON_THROW,
    BUTTON_WHISTLE,
    BUTTON_RIGHT,
    BUTTON_UP,
    BUTTON_LEFT,
    BUTTON_DOWN,
    BUTTON_CURSOR_RIGHT,
    BUTTON_CURSOR_UP,
    BUTTON_CURSOR_LEFT,
    BUTTON_CURSOR_DOWN,
    BUTTON_GROUP_RIGHT,
    BUTTON_GROUP_UP,
    BUTTON_GROUP_LEFT,
    BUTTON_GROUP_DOWN,
    BUTTON_GROUP_CURSOR,
    BUTTON_NEXT_LEADER,
    BUTTON_PREV_LEADER,
    BUTTON_DISMISS,
    BUTTON_USE_SPRAY_1,
    BUTTON_USE_SPRAY_2,
    BUTTON_USE_SPRAY,
    BUTTON_NEXT_SPRAY,
    BUTTON_PREV_SPRAY,
    BUTTON_CHANGE_ZOOM,
    BUTTON_ZOOM_IN,
    BUTTON_ZOOM_OUT,
    BUTTON_NEXT_TYPE,
    BUTTON_PREV_TYPE,
    BUTTON_NEXT_MATURITY,
    BUTTON_PREV_MATURITY,
    BUTTON_LIE_DOWN,
    BUTTON_PAUSE,
    BUTTON_MENU_RIGHT,
    BUTTON_MENU_UP,
    BUTTON_MENU_LEFT,
    BUTTON_MENU_DOWN,
    BUTTON_MENU_OK,
    BUTTON_MENU_BACK,
    
    N_BUTTONS,
};


enum CONTROL_TYPES {
    CONTROL_TYPE_NONE,
    CONTROL_TYPE_KEYBOARD_KEY,
    CONTROL_TYPE_MOUSE_BUTTON,
    CONTROL_TYPE_MOUSE_WHEEL_UP,
    CONTROL_TYPE_MOUSE_WHEEL_DOWN,
    CONTROL_TYPE_MOUSE_WHEEL_LEFT,
    CONTROL_TYPE_MOUSE_WHEEL_RIGHT,
    CONTROL_TYPE_JOYSTICK_BUTTON,
    CONTROL_TYPE_JOYSTICK_AXIS_POS,
    CONTROL_TYPE_JOYSTICK_AXIS_NEG,
};


#endif //ifndef CONTROLS_INCLUDED
