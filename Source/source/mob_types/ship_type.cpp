/*
 * Copyright (c) Andre 'Espyo' Silva 2013.
 * The following source file belongs to the open-source project Pikifen.
 * Please read the included README and LICENSE files for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Ship type class and ship type-related functions.
 */

#include "ship_type.h"

#include "../functions.h"
#include "../mob_fsms/ship_fsm.h"
#include "../mobs/ship.h"
#include "../utils/string_utils.h"


/* ----------------------------------------------------------------------------
 * Creates a type of ship.
 */
ship_type::ship_type() :
    mob_type(MOB_CATEGORY_SHIPS),
    can_heal(false),
    beam_radius(0.0f) {
    
    target_type = MOB_TARGET_TYPE_NONE;
    
    ship_fsm::create_fsm(this);
}


/* ----------------------------------------------------------------------------
 * Returns the vector of animation conversions.
 */
anim_conversion_vector ship_type::get_anim_conversions() const {
    anim_conversion_vector v;
    v.push_back(std::make_pair(SHIP_ANIM_IDLING, "idling"));
    return v;
}


/* ----------------------------------------------------------------------------
 * Loads properties from a data file.
 * file:
 *   File to read from.
 */
void ship_type::load_properties(data_node* file) {
    reader_setter rs(file);
    
    rs.set("beam_offset_x", beam_offset.x);
    rs.set("beam_offset_y", beam_offset.y);
    rs.set("beam_radius", beam_radius);
    rs.set("can_heal", can_heal);
}
