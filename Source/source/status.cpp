/*
 * Copyright (c) Andre 'Espyo' Silva 2013-2016.
 * The following source file belongs to the open-source project
 * Pikmin fangame engine. Please read the included
 * README and LICENSE files for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Status effect classes and status effect-related functions.
 */

#include <algorithm>

#include "status.h"

using namespace std;

/* ----------------------------------------------------------------------------
 * Creates a status effect type.
 */
status_type::status_type() :
    color(al_map_rgba(0, 0, 0, 0)),
    tint(al_map_rgb(255, 255, 255)),
    removable_with_whistle(false),
    auto_remove_time(0.0f),
    health_change_ratio(1.0f),
    maturity_change_amount(0),
    causes_panic(false),
    causes_flailing(false),
    speed_multiplier(1.0f),
    attack_multiplier(1.0f),
    defense_multiplier(1.0f),
    anim_speed_multiplier(1.0f) {
    
}



/* ----------------------------------------------------------------------------
 * Creates a status effect instance.
 */
status::status(status_type* type) :
    type(type),
    to_delete(false) {
    
    time_left = type->auto_remove_time;
}


/* ----------------------------------------------------------------------------
 * Ticks a status effect instance's logic, but not its effects.
 */
void status::tick(const float delta_t) {
    if(type->auto_remove_time > 0.0f) {
        time_left -= delta_t;
        if(time_left <= 0.0f) {
            to_delete = true;
        }
    }
}