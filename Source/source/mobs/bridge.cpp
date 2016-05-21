/*
 * Copyright (c) Andre 'Espyo' Silva 2013-2016.
 * The following source file belongs to the open-source project
 * Pikmin fangame engine. Please read the included
 * README and LICENSE files for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Bridge class and bridge related functions.
 */

#include "bridge.h"
#include "../functions.h"
#include "../vars.h"

/* ----------------------------------------------------------------------------
 * Creates a bridge mob.
 */
bridge::bridge(
    const float x, const float y, const float angle, const string &vars
) :
    mob(x, y, spec_mob_types["Bridge"], angle, vars) {
    
    //Search neighboring sectors.
    get_neighbor_bridge_sectors(get_sector(x, y, NULL, true));
    team = MOB_TEAM_OBSTACLE;
    
}


/* ----------------------------------------------------------------------------
 * Populates this bridge's list of sectors by checking all
 * neighboring sectors recursively, until it can't find any more
 * sectors of the "bridge" or "bridge rail" type.
 */
void bridge::get_neighbor_bridge_sectors(sector* s_ptr) {

    if(!s_ptr) return;
    
    //If this sector is not a bridge sector, skip it.
    if(
        s_ptr->type != SECTOR_TYPE_BRIDGE &&
        s_ptr->type != SECTOR_TYPE_BRIDGE_RAIL
    ) return;
    
    //If this sector is already on the list, skip.
    for(size_t s = 0; s < secs.size(); ++s) {
        if(secs[s] == s_ptr) return;
    }
    
    secs.push_back(s_ptr);
    
    edge* e_ptr = NULL;
    for(size_t e = 0; e < s_ptr->edges.size(); ++e) {
        e_ptr = s_ptr->edges[e];
        get_neighbor_bridge_sectors(
            e_ptr->sectors[(e_ptr->sectors[0] == s_ptr ? 1 : 0)]
        );
    }
}


/* ----------------------------------------------------------------------------
 * Opens up the bridge. Updates all relevant sectors,
 * does the particle explosion, etc.
 */
void bridge::open(mob* m, void* info1, void* info2) {
    bridge* b_ptr = (bridge*) m;
    b_ptr->set_animation(BRIDGE_ANIM_NOTHING);
    b_ptr->start_dying();
    b_ptr->finish_dying();
    random_particle_explosion(
        PARTICLE_TYPE_BITMAP, bmp_smoke, b_ptr->x, b_ptr->y,
        60, 90, 10, 12, 2.5, 3, 64, 96, al_map_rgb(238, 204, 170)
    );
    
    for(size_t s = 0; s < b_ptr->secs.size(); s++) {
        sector* s_ptr = b_ptr->secs[s];
        
        s_ptr->z = s2f(s_ptr->tag);
        
        sector_correction sc(s_ptr);
        //TODO the file name is so static...
        //plus, the railing should have its own texture.
        sc.new_texture.bitmap = bitmaps.get("Textures/Bridge.png", NULL);
        sc.new_texture.rot = m->angle;
        
        cur_area_data.sector_corrections.push_back(sc);
        cur_area_data.generate_edges_blockmap(s_ptr->edges);
        
    }
}


/* ----------------------------------------------------------------------------
 * Damage the bridge, depending on the Pikmin, hitbox, etc.
 */
void bridge::take_damage(mob* m, void* info1, void* info2) {
    hitbox_touch_info* info = (hitbox_touch_info*) info1;
    float damage = calculate_damage(info->mob2, m, info->hi2, info->hi1);
    m->health -= damage;
}


/* ----------------------------------------------------------------------------
 * Sets the standard "idle" animation.
 */
void bridge::set_anim(mob* m, void* info1, void* info2) {
    m->set_animation(BRIDGE_ANIM_IDLE);
}


/* ----------------------------------------------------------------------------
 * Initializes the bridge mob type.
 */
void init_bridge_mob_type(mob_type* mt) {
    mt->always_active = true;
    mt->radius = 32;
    mt->max_health = 2000;
    mt->casts_shadow = false;
    mt->create_mob = [] (float x, float y, float angle, const string & vars) {
        create_mob(new bridge(x, y, angle, vars));
    };
    mt->load_from_file_func =
        [] (
            data_node * file, const bool load_resources,
            vector<pair<size_t, string> >* anim_conversions
    ) {
        if(load_resources) {
            anim_conversions->push_back(make_pair(0, "idle"));
            anim_conversions->push_back(make_pair(1, "nothing"));
        }
    };
    
    easy_fsm_creator efc;
    efc.new_state("idle", 0); {
        efc.new_event(MOB_EVENT_ON_ENTER); {
            efc.run_function(bridge::set_anim);
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_N_A); {
            efc.run_function(bridge::take_damage);
        }
        efc.new_event(MOB_EVENT_DEATH); {
            efc.run_function(bridge::open);
            efc.change_state("dead");
        }
    }
    efc.new_state("dead", 1); {
    
    }
    
    
    mt->states = efc.finish();
    mt->first_state_nr = fix_states(mt->states, "idle");
}