/*
 * Copyright (c) Andre 'Espyo' Silva 2013.
 * The following source file belongs to the open-source project Pikifen.
 * Please read the included README and LICENSE files for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Header for the decoration type class and decoration type-related functions.
 */

#ifndef DECORATION_TYPE_INCLUDED
#define DECORATION_TYPE_INCLUDED

#include <allegro5/allegro.h>

#include "../utils/data_file.h"
#include "mob_type.h"


enum DECORATION_ANIMATIONS {
    DECORATION_ANIM_IDLING,
    DECORATION_ANIM_BUMPED,
};


enum DECORATION_STATES {
    DECORATION_STATE_IDLING,
    DECORATION_STATE_BUMPED,
    
    N_DECORATION_STATES,
};


/* ----------------------------------------------------------------------------
 * A type of decoration.
 */
class decoration_type : public mob_type {
public:
    ALLEGRO_COLOR tint_random_maximum;
    float scale_random_variation;
    float rotation_random_variation;
    bool random_animation_delay;
    
    decoration_type();
    void load_properties(data_node* file);
    anim_conversion_vector get_anim_conversions() const;
};


#endif //ifndef DECORATION_TYPE_INCLUDED
