/*
 * Copyright (c) Andr´ 'Espyo' Silva 2014.
 * The following source file belongs to the open-source project
 * Pikmin fangame engine. Please read the included README file
 * for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Main game loop logic.
 */

#include <algorithm>

#include "const.h"
#include "drawing.h"
#include "functions.h"
#include "logic.h"
#include "pikmin.h"
#include "vars.h"

//Because we get events so many times per frame, it's faster
//to access them directly than to call a function.
#define q_get_event(m_ptr, ev_type) ((m_ptr)->fsm.cur_state ? (m_ptr)->fsm.cur_state->events[(ev_type)] : nullptr)

void do_logic() {

    /*  ********************************************
      ***  .-.                                .-.  ***
    ***** ( L )          MAIN LOGIC          ( L ) *****
      ***  `-´                                `-´  ***
        ********************************************/
    
    leader* cur_leader_ptr = leaders[cur_leader_nr];
    
    
    /*************************************
    *                               .-.  *
    *   Timer things - aesthetic   ( L ) *
    *                               `-´  *
    **************************************/
    
    //Rotation angle for the glow atop idle Pikmin.
    idle_glow_angle += IDLE_GLOW_SPIN_SPEED * delta_t;
    
    //Camera transitions.
    if(cam_trans_pan_time_left > 0) {
        cam_trans_pan_time_left -= delta_t;
        if(cam_trans_pan_time_left < 0) cam_trans_pan_time_left = 0;
        
        float percentage_left = cam_trans_pan_time_left / CAM_TRANSITION_DURATION;
        percentage_left = ease(EASE_IN, percentage_left);
        
        cam_x = cam_trans_pan_initial_x + (cam_trans_pan_final_x - cam_trans_pan_initial_x) * (1 - percentage_left);
        cam_y = cam_trans_pan_initial_y + (cam_trans_pan_final_y - cam_trans_pan_initial_y) * (1 - percentage_left);
    }
    
    if(cam_trans_zoom_time_left > 0) {
        cam_trans_zoom_time_left -= delta_t;
        if(cam_trans_zoom_time_left < 0) cam_trans_zoom_time_left = 0;
        
        float percentage_left = cam_trans_zoom_time_left / CAM_TRANSITION_DURATION;
        percentage_left = ease(EASE_IN, percentage_left);
        
        cam_zoom = cam_trans_zoom_initial_level + (cam_trans_zoom_final_level - cam_trans_zoom_initial_level) * (1 - percentage_left);
    }
    
    //"Move group" arrows.
    if(group_move_intensity) {
        group_move_next_arrow_time -= delta_t;
        if(group_move_next_arrow_time <= 0) {
            group_move_next_arrow_time = GROUP_MOVE_ARROWS_INTERVAL;
            group_move_arrows.push_back(0);
        }
    }
    
    dist leader_to_cursor_dis(cur_leader_ptr->x, cur_leader_ptr->y, cursor_x, cursor_y);
    for(size_t a = 0; a < group_move_arrows.size(); ) {
        group_move_arrows[a] += GROUP_MOVE_ARROW_SPEED * delta_t;
        
        dist max_dist =
            ((group_move_intensity > 0) ? CURSOR_MAX_DIST* group_move_intensity : leader_to_cursor_dis);
            
        if(max_dist < group_move_arrows[a]) {
            group_move_arrows.erase(group_move_arrows.begin() + a);
        } else {
            a++;
        }
    }
    
    //Whistle animations.
    whistle_dot_offset -= WHISTLE_DOT_SPIN_SPEED * delta_t;
    
    if(whistle_fade_time > 0) {
        whistle_fade_time -= delta_t;
        if(whistle_fade_time < 0) whistle_fade_time = 0;
    }
    
    if(whistling) {
        //Create rings.
        whistle_next_ring_time -= delta_t;
        if(whistle_next_ring_time <= 0) {
            whistle_next_ring_time = WHISTLE_RINGS_INTERVAL;
            whistle_rings.push_back(0);
            whistle_ring_colors.push_back(whistle_ring_prev_color);
            whistle_ring_prev_color = (whistle_ring_prev_color + 1) % N_WHISTLE_RING_COLORS;
        }
        
        if(pretty_whistle) {
            whistle_next_dot_time -= delta_t;
            if(whistle_next_dot_time <= 0) {
                whistle_next_dot_time = WHISTLE_DOT_INTERVAL;
                unsigned char dot = 255;
                for(unsigned char d = 0; d < 6; d++) { //Find WHAT dot to add.
                    if(whistle_dot_radius[d] == -1) { dot = d; break;}
                }
                
                if(dot != 255) whistle_dot_radius[dot] = 0;
            }
        }
        
        for(unsigned char d = 0; d < 6; d++) {
            if(whistle_dot_radius[d] == -1) continue;
            
            whistle_dot_radius[d] += WHISTLE_RADIUS_GROWTH_SPEED * delta_t;
            if(whistle_radius > 0 && whistle_dot_radius[d] > cur_leader_ptr->lea_type->whistle_range) {
                whistle_dot_radius[d] = cur_leader_ptr->lea_type->whistle_range;
            } else if(whistle_fade_radius > 0 && whistle_dot_radius[d] > whistle_fade_radius) {
                whistle_dot_radius[d] = whistle_fade_radius;
            }
        }
    }
    
    for(size_t r = 0; r < whistle_rings.size(); ) {
        //Erase rings that go beyond the cursor.
        whistle_rings[r] += WHISTLE_RING_SPEED * delta_t;
        if(leader_to_cursor_dis < whistle_rings[r]) {
            whistle_rings.erase(whistle_rings.begin() + r);
            whistle_ring_colors.erase(whistle_ring_colors.begin() + r);
        } else {
            r++;
        }
    }
    
    //Ship beam ring.
    //The way this works is that the three color components are saved.
    //Each frame, we increase them or decrease them (if it reaches 255, set it to decrease, if 0, set it to increase).
    //Each index increases/decreases at a different speed, with red being the slowest and blue the fastest.
    for(unsigned char i = 0; i < 3; i++) {
        float dir_mult = (ship_beam_ring_color_up[i]) ? 1.0 : -1.0;
        signed char addition = dir_mult * SHIP_BEAM_RING_COLOR_SPEED * (i + 1) * delta_t;
        if(ship_beam_ring_color[i] + addition >= 255) {
            ship_beam_ring_color[i] = 255;
            ship_beam_ring_color_up[i] = false;
        } else if(ship_beam_ring_color[i] + addition <= 0) {
            ship_beam_ring_color[i] = 0;
            ship_beam_ring_color_up[i] = true;
        } else {
            ship_beam_ring_color[i] += addition;
        }
    }
    
    //Sun meter.
    sun_meter_sun_angle += SUN_METER_SUN_SPIN_SPEED * delta_t;
    
    //Cursor spin angle and invalidness effect.
    cursor_spin_angle -= CURSOR_SPIN_SPEED * delta_t;
    cursor_invalid_effect += CURSOR_INVALID_EFFECT_SPEED * delta_t;
    
    //Cursor trail.
    if(draw_cursor_trail) {
        if(cursor_save_time > 0) {
            cursor_save_time -= delta_t;
            if(cursor_save_time <= 0) {
                cursor_save_time = CURSOR_SAVE_INTERVAL;
                cursor_spots.push_back(point(mouse_cursor_x, mouse_cursor_y));
                if(cursor_spots.size() > CURSOR_SAVE_N_SPOTS) {
                    cursor_spots.erase(cursor_spots.begin());
                }
            }
        }
    }
    
    //Tree shadow swaying.
    tree_shadow_sway += TREE_SHADOW_SWAY_SPEED * delta_t;
    
    
    if(cur_message.empty()) {
    
        /************************************
        *                              .-.  *
        *   Timer things - gameplay   ( L ) *
        *                              `-´  *
        *************************************/
        
        day_minutes += (day_minutes_per_irl_sec * delta_t);
        if(day_minutes > 60 * 24) day_minutes -= 60 * 24;
        
        if(auto_pluck_input_time > 0) {
            auto_pluck_input_time -= delta_t;
            if(auto_pluck_input_time < 0) auto_pluck_input_time = 0;
        }
        
        //Tick all particles.
        size_t n_particles = particles.size();
        for(size_t p = 0; p < n_particles; ) {
            if(!particles[p].tick()) {
                particles.erase(particles.begin() + p);
                n_particles--;
            } else {
                p++;
            }
        }
        
        /********************
        *              ***  *
        *   Whistle   * O * *
        *              ***  *
        ********************/
        
        if(whistling && whistle_radius < cur_leader_ptr->lea_type->whistle_range) {
            whistle_radius += WHISTLE_RADIUS_GROWTH_SPEED * delta_t;
            if(whistle_radius > cur_leader_ptr->lea_type->whistle_range) {
                whistle_radius = cur_leader_ptr->lea_type->whistle_range;
                whistle_max_hold = WHISTLE_MAX_HOLD_TIME;
            }
        }
        
        if(whistle_max_hold > 0) {
            whistle_max_hold -= delta_t;
            if(whistle_max_hold <= 0) {
                stop_whistling();
            }
        }
        
        
        /*****************
        *                *
        *   Mobs   ()--> *
        *                *
        ******************/
        
        size_t n_mobs = mobs.size();
        for(size_t m = 0; m < n_mobs;) {
        
            //Tick the mob.
            mob* m_ptr = mobs[m];
            m_ptr->tick();
            
            if(m_ptr->carrier_info) {
                if(m_ptr->state == MOB_STATE_BEING_CARRIED && m_ptr->reached_destination && m_ptr->carrier_info->decided_type) {
                    m_ptr->set_state(MOB_STATE_BEING_DELIVERED);
                    sfx_pikmin_carrying.stop();
                }
            }
            
            if(m_ptr->carrier_info && m_ptr->state == MOB_STATE_BEING_DELIVERED && m_ptr->time_in_state >= DELIVERY_SUCK_TIME) {
                if(m_ptr->carrier_info->carry_to_ship) {
                    //Find ship.
                    //TODO.
                    
                } else {
                    //TODO make the pellet, enemy, etc. class react to this via script (i.e. was_delivered event).
                    //Find Onion.
                    size_t n_onions = onions.size();
                    size_t o = 0;
                    for(; o < n_onions; o++) {
                        if(onions[o]->oni_type->pik_type == m_ptr->carrier_info->decided_type) break;
                    }
                    
                    if(typeid(*m_ptr) == typeid(pellet)) {
                        pellet* p_ptr = (pellet*) m_ptr;
                        if(p_ptr->pel_type->pik_type == p_ptr->carrier_info->decided_type) {
                            give_pikmin_to_onion(onions[o], p_ptr->pel_type->match_seeds);
                        } else {
                            give_pikmin_to_onion(onions[o], p_ptr->pel_type->non_match_seeds);
                        }
                        
                    } else if(typeid(*m_ptr) == typeid(enemy)) {
                        enemy* e_ptr = (enemy*) m_ptr;
                        give_pikmin_to_onion(onions[o], e_ptr->ene_type->pikmin_seeds);
                        
                    }
                }
                
                random_particle_explosion(
                    PARTICLE_TYPE_BITMAP, bmp_smoke,
                    m_ptr->x, m_ptr->y,
                    60, 80, 10, 20,
                    1, 2, 24, 24, al_map_rgb(255, 255, 255)
                );
                
                make_uncarriable(m_ptr);
                if(typeid(*m_ptr) != typeid(leader)) m_ptr->to_delete = true;
            }
            
            
            
            /********************************
             *                              *
             *   Mob interactions   () - () *
             *                              *
             ********************************/
            //Interactions between this mob and the others.
            
            for(size_t m2 = 0; m2 < n_mobs; m2++) {
                if(m == m2) continue;
                mob* m2_ptr = mobs[m2];
                
                dist d(m_ptr->x, m_ptr->y, m2_ptr->x, m2_ptr->y);
                
                if(!m2_ptr->dead) {
                    //Check "see"s.
                    mob_event* see_op_ev = q_get_event(m_ptr, MOB_EVENT_SEEN_OPPONENT);
                    mob_event* see_ob_ev = q_get_event(m_ptr, MOB_EVENT_SEEN_OBJECT);
                    if(see_op_ev || see_ob_ev) {
                    
                        if(d <= (m_ptr->type->radius + m2_ptr->type->radius + m_ptr->type->sight_radius)) {
                            if(see_ob_ev) see_ob_ev->run(m_ptr, (void*) m2_ptr);
                            if(see_op_ev && should_attack(m_ptr, m2_ptr)) {
                                see_op_ev->run(m_ptr, (void*) m2_ptr);
                            }
                        }
                        
                    }
                    
                    //Check "near"s.
                    mob_event* near_op_ev = q_get_event(m_ptr, MOB_EVENT_NEAR_OPPONENT);
                    mob_event* near_ob_ev = q_get_event(m_ptr, MOB_EVENT_NEAR_OBJECT);
                    if(near_op_ev || near_ob_ev) {
                    
                        if(d <= (m_ptr->type->radius + m2_ptr->type->radius + m_ptr->type->near_radius)) {
                            if(near_ob_ev) near_ob_ev->run(m_ptr, (void*) m2_ptr);
                            if(near_op_ev && should_attack(m_ptr, m2_ptr)) {
                                near_op_ev->run(m_ptr, (void*) m2_ptr);
                            }
                        }
                        
                    }
                    
                    //Check if it's facing.
                    mob_event* facing_op_ev = q_get_event(m_ptr, MOB_EVENT_FACING_OPPONENT);
                    mob_event* facing_ob_ev = q_get_event(m_ptr, MOB_EVENT_FACING_OBJECT);
                    if(facing_op_ev || facing_ob_ev) {
                    
                        float angle_dif = get_angle_smallest_dif(m_ptr->angle, atan2(m2_ptr->y - m_ptr->y, m2_ptr->x - m_ptr->x));
                        if(
                            d <= (m_ptr->type->radius + m2_ptr->type->radius + m_ptr->type->near_radius) &&
                            angle_dif <= (m_ptr->type->near_angle / 2.0)
                        ) {
                        
                            if(facing_ob_ev) facing_ob_ev->run(m_ptr, (void*) m2_ptr);
                            if(facing_op_ev && should_attack(m_ptr, m2_ptr)) {
                                facing_op_ev->run(m_ptr, (void*) m2_ptr);
                            }
                        }
                        
                    }
                }
                
                //Check touches. This does not use hitboxes.
                mob_event* touch_op_ev = q_get_event(m_ptr, MOB_EVENT_TOUCHED_OPPONENT);
                mob_event* touch_le_ev = q_get_event(m_ptr, MOB_EVENT_TOUCHED_LEADER);
                mob_event* touch_ob_ev = q_get_event(m_ptr, MOB_EVENT_TOUCHED_OBJECT);
                if(touch_op_ev || touch_le_ev || touch_ob_ev) {
                    
                    bool z_touch;
                    if(m_ptr->type->height == 0 || m2_ptr->type->height == 0){
                        z_touch = true;
                    }else{
                        z_touch = !((m2_ptr->z > m_ptr->z + m_ptr->type->height) || (m2_ptr->z + m2_ptr->type->height < m2_ptr->z));
                    }
                
                    if(z_touch && d <= (m_ptr->type->radius + m2_ptr->type->radius)) {
                        if(touch_ob_ev) touch_ob_ev->run(m_ptr, (void*) m2_ptr);
                        if(touch_op_ev && should_attack(m_ptr, m2_ptr)) {
                            touch_op_ev->run(m_ptr, (void*) m2_ptr);
                        }
                        if(touch_le_ev && m2_ptr == cur_leader_ptr) {
                            touch_le_ev->run(m_ptr, (void*) m2_ptr);
                        }
                    }
                    
                }
                
                //Check hitbox touches.
                mob_event* hitbox_touch_an_ev =  q_get_event(m_ptr, MOB_EVENT_HITBOX_TOUCH_A_N);
                mob_event* hitbox_touch_na_ev =  q_get_event(m_ptr, MOB_EVENT_HITBOX_TOUCH_N_A);
                mob_event* hitbox_touch_eat_ev = q_get_event(m_ptr, MOB_EVENT_HITBOX_TOUCH_EAT);
                if(hitbox_touch_an_ev || hitbox_touch_na_ev || hitbox_touch_eat_ev) {
                
                    frame* f1_ptr = m_ptr->anim.get_frame();
                    frame* f2_ptr = m2_ptr->anim.get_frame();
                    
                    //If neither of the mobs have hitboxes up, never mind.
                    bool m1_is_pikmin = typeid(*m_ptr) == typeid(pikmin);
                    bool m1_has_hitboxes = f1_ptr && (!f1_ptr->hitbox_instances.empty() || m1_is_pikmin);
                    bool m2_has_hitboxes = f2_ptr && !f2_ptr->hitbox_instances.empty();
                    
                    if(m1_has_hitboxes && m2_has_hitboxes) {
                    
                        //If they're so far away the hitboxes can't touch, just skip the check.
                        if(d < f1_ptr->hitbox_span + f2_ptr->hitbox_span) {
                        
                            bool reported_an_ev = false;
                            bool reported_na_ev = false;
                            bool reported_eat_ev = false;
                            
                            float m1_angle_sin = 0;
                            float m1_angle_cos = 0;
                            if(!m1_is_pikmin) {
                                m1_angle_sin = sin(m_ptr->angle);
                                m1_angle_cos = cos(m_ptr->angle);
                            }
                            float m2_angle_sin = sin(m2_ptr->angle);
                            float m2_angle_cos = cos(m2_ptr->angle);
                            
                            //For all of mob 2's hitboxes, check for collisions.
                            for(size_t h2 = 0; h2 < f2_ptr->hitbox_instances.size(); h2++) {
                                hitbox_instance* h2_ptr = &f2_ptr->hitbox_instances[h2];
                                
                                //Get mob 2's real hitbox location.
                                float m2_h_x = m2_ptr->x + (h2_ptr->x * m2_angle_cos - h2_ptr->y * m2_angle_sin);
                                float m2_h_y = m2_ptr->y + (h2_ptr->x * m2_angle_sin + h2_ptr->y * m2_angle_cos);
                                float m2_h_z = m2_ptr->z + h2_ptr->z;
                                
                                if(m1_is_pikmin) {
                                    //Just check if the entire Pikmin touched mob 2's hitbox.
                                    
                                    bool z_collision;
                                    if(h2_ptr->height == 0){
                                        //Always hits vertically. Imagine the hitbox is infinitely high.
                                        z_collision = true;
                                    }else{
                                        z_collision = !((m2_h_z > m_ptr->z) || (m2_h_z + h2_ptr->height < m_ptr->z));
                                    }
                                    
                                    if(
                                        z_collision &&
                                        dist(m_ptr->x, m_ptr->y, m2_h_x, m2_h_y) <
                                        (m_ptr->type->radius + h2_ptr->radius)
                                    ) {
                                        //Collision!
                                        if(
                                            hitbox_touch_eat_ev && !reported_eat_ev &&
                                            h2_ptr->type != HITBOX_TYPE_DISABLED &&
                                            m2_ptr->chomping_pikmin.size() < m2_ptr->chomp_max &&
                                            find(m2_ptr->chomp_hitboxes.begin(), m2_ptr->chomp_hitboxes.end(), h2_ptr->hitbox_nr) !=
                                            m2_ptr->chomp_hitboxes.end()
                                        ) {
                                            hitbox_touch_eat_ev->run(m_ptr, (void*) m2_ptr, (void*) h2_ptr);
                                            m2_ptr->chomping_pikmin.push_back(m_ptr);
                                            reported_eat_ev = true;
                                        }
                                        
                                        if(
                                            h2_ptr->type == HITBOX_TYPE_ATTACK &&
                                            hitbox_touch_na_ev && !reported_na_ev &&
                                            !reported_eat_ev //This way, Pikmin aren't knocked back AND eaten.
                                        ) {
                                            hitbox_touch_na_ev->run(m_ptr, (void*) m2_ptr, (void*) h2_ptr);
                                            reported_na_ev = true;
                                            
                                        } else if(
                                            h2_ptr->type == HITBOX_TYPE_NORMAL &&
                                            hitbox_touch_an_ev && !reported_an_ev
                                        ) {
                                            hitbox_touch_an_ev->run(m_ptr, (void*) m2_ptr, (void*) h2_ptr);
                                            reported_an_ev = true;
                                        }
                                    }
                                    
                                } else {
                                    //Check if any hitbox touched mob 2's hitbox.
                                    
                                    for(size_t h1 = 0; h1 < f1_ptr->hitbox_instances.size(); h1++) {
                                    
                                        hitbox_instance* h1_ptr = &f1_ptr->hitbox_instances[h1];
                                        if(h1_ptr->type == HITBOX_TYPE_DISABLED) continue;
                                        
                                        //Get mob 1's real hitbox location.
                                        float m1_h_x = m_ptr->x + (h1_ptr->x * m1_angle_cos - h1_ptr->y * m1_angle_sin);
                                        float m1_h_y = m_ptr->y + (h1_ptr->x * m1_angle_sin + h1_ptr->y * m1_angle_cos);
                                        float m1_h_z = m_ptr->z + h1_ptr->z;
                                        
                                        bool z_collision;
                                        if(h1_ptr->height == 0 || h2_ptr->height == 0){
                                            z_collision = true;
                                        } else {
                                            z_collision = !((m2_h_z > m1_h_z + h1_ptr->height) || (m2_h_z + h2_ptr->height < m1_h_z));
                                        }
                                        
                                        if(
                                            z_collision &&
                                            dist(m1_h_x, m1_h_y, m2_h_x, m2_h_y) <
                                            (h1_ptr->radius + h2_ptr->radius)
                                        ) {
                                            //Collision!
                                            if(
                                                hitbox_touch_eat_ev && !reported_eat_ev &&
                                                h1_ptr->type == HITBOX_TYPE_NORMAL &&
                                                h2_ptr->type != HITBOX_TYPE_DISABLED &&
                                                m2_ptr->chomping_pikmin.size() < m2_ptr->chomp_max &&
                                                find(m2_ptr->chomp_hitboxes.begin(), m2_ptr->chomp_hitboxes.end(), h2_ptr->hitbox_nr) !=
                                                m2_ptr->chomp_hitboxes.end()
                                            ) {
                                                hitbox_touch_eat_ev->run(m_ptr, (void*) m2_ptr, (void*) h2_ptr);
                                                m2_ptr->chomping_pikmin.push_back(m_ptr);
                                                reported_eat_ev = true;
                                            }
                                            
                                            if(
                                                h1_ptr->type == HITBOX_TYPE_NORMAL &&
                                                h2_ptr->type == HITBOX_TYPE_ATTACK &&
                                                hitbox_touch_na_ev && !reported_na_ev
                                            ) {
                                                hitbox_touch_na_ev->run(m_ptr, (void*) m2_ptr, (void*) h2_ptr);
                                                reported_na_ev = true;
                                                
                                            } else if(
                                                h1_ptr->type == HITBOX_TYPE_ATTACK &&
                                                h2_ptr->type == HITBOX_TYPE_NORMAL &&
                                                hitbox_touch_an_ev && !reported_an_ev
                                            ) {
                                                hitbox_touch_an_ev->run(m_ptr, (void*) m2_ptr, (void*) h2_ptr);
                                                reported_an_ev = true;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                
                //Find a carriable mob to grab.
                mob_event* near_carriable_object_ev = q_get_event(m_ptr, MOB_EVENT_NEAR_CARRIABLE_OBJECT);
                if(near_carriable_object_ev) {
                
                    if(
                        m2_ptr->carrier_info &&
                        m2_ptr->carrier_info->current_n_carriers != m2_ptr->carrier_info->max_carriers &&
                        d <= m_ptr->type->radius + m2_ptr->type->radius + PIKMIN_MIN_TASK_RANGE
                    ) {
                    
                        near_carriable_object_ev->run(m_ptr, (void*) m2_ptr);
                        
                    }
                    
                }
            }
            
            //Check if it got whistled.
            mob_event* whistled_ev = q_get_event(m_ptr, MOB_EVENT_WHISTLED);
            if(whistling && whistled_ev) {
                if(dist(m_ptr->x, m_ptr->y, cursor_x, cursor_y) <= whistle_radius) {
                    whistled_ev->run(m_ptr);
                }
            }
            
            //Following a leader.
            if(m_ptr->following_party) {
                mob_event* leader_near_ev = q_get_event(m_ptr, MOB_EVENT_LEADER_IS_NEAR);
                mob_event* leader_far_ev =  q_get_event(m_ptr, MOB_EVENT_LEADER_IS_FAR);
                
                if(leader_near_ev || leader_far_ev) {
                    dist d(m_ptr->x, m_ptr->y, m_ptr->following_party->x, m_ptr->following_party->y);
                    if(leader_far_ev && d >= 50) {
                        leader_far_ev->run(m_ptr);
                    } else if(leader_near_ev && d < 50) {
                        leader_near_ev->run(m_ptr);
                    }
                }
            }
            
            //Focused on a mob.
            if(m_ptr->focused_mob) {
            
                dist d(m_ptr->x, m_ptr->y, m_ptr->focused_mob->x, m_ptr->focused_mob->y);
                if(m_ptr->focused_mob->dead) {
                    m_ptr->fsm.run_event(MOB_EVENT_FOCUSED_MOB_DIED);
                }
                
                if(m_ptr->focused_mob) {
                    if(d > (m_ptr->type->sight_radius * 1.1f)) {
                        m_ptr->fsm.run_event(MOB_EVENT_LOST_FOCUSED_MOB);
                    }
                }
                
                if(m_ptr->focused_mob) {
                    if(!m_ptr->focused_mob->carrier_info) {
                        m_ptr->fsm.run_event(MOB_EVENT_FOCUSED_MOB_UNCARRIABLE);
                    }
                }
                
            }
            
            //Far away from home.
            mob_event* far_from_home_ev = q_get_event(m_ptr, MOB_EVENT_FAR_FROM_HOME);
            if(far_from_home_ev) {
                dist d(m_ptr->x, m_ptr->y, m_ptr->home_x, m_ptr->home_y);
                if(d >= m_ptr->type->territory_radius) {
                    far_from_home_ev->run(m_ptr);
                }
            }
            
            //Check its mouth.
            if(m_ptr->chomping_pikmin.empty()) {
                m_ptr->fsm.run_event(MOB_EVENT_MOUTH_EMPTY);
            } else {
                m_ptr->fsm.run_event(MOB_EVENT_MOUTH_OCCUPIED);
            }
            
            //Big damage.
            mob_event* big_damage_ev = q_get_event(m_ptr, MOB_EVENT_BIG_DAMAGE);
            if(big_damage_ev && m_ptr->big_damage_ev_queued) {
                big_damage_ev->run(m_ptr);
                m_ptr->big_damage_ev_queued = false;
            }
            
            //Tick.
            m_ptr->fsm.run_event(MOB_EVENT_ON_TICK);
            
            //Mob deletion.
            if(m_ptr->to_delete) {
                delete_mob(m_ptr);
                n_mobs--;
                continue;
            }
            m++;
            
            
        }
        
        
        
        /******************
        *             /\  *
        *   Pikmin   (@:) *
        *             \/  *
        ******************/
        
        for(size_t p = 0; p < pikmin_list.size(); p++) {
            pikmin* pik_ptr = pikmin_list[p];
            
            //Is it dead?
            //TODO move to the script?
            if(pik_ptr->dead) {
            
                pik_ptr->to_delete = true;
                particles.push_back(
                    particle(
                        PARTICLE_TYPE_PIKMIN_SPIRIT, bmp_pikmin_spirit, pik_ptr->x, pik_ptr->y,
                        0, -50, 0.5, 0, 2, pik_ptr->pik_type->size, pik_ptr->pik_type->main_color
                    )
                );
                sfx_pikmin_dying.play(0.03, false);
                continue;
                
            }
        }
        
        
        /********************
        *              .-.  *
        *   Leaders   (*:O) *
        *              `-´  *
        ********************/
        
        if(cur_leader_ptr->holding_pikmin) {
            cur_leader_ptr->holding_pikmin->x = cur_leader_ptr->x + cos(cur_leader_ptr->angle + M_PI) * cur_leader_ptr->type->radius;
            cur_leader_ptr->holding_pikmin->y = cur_leader_ptr->y + sin(cur_leader_ptr->angle + M_PI) * cur_leader_ptr->type->radius;
            cur_leader_ptr->holding_pikmin->z = cur_leader_ptr->z;
            cur_leader_ptr->holding_pikmin->angle = cur_leader_ptr->angle;
        }
        
        //Current leader movement.
        if(!cur_leader_ptr->auto_pluck_mode && !cur_leader_ptr->auto_pluck_pikmin && !cur_leader_ptr->carrier_info) {
            float leader_move_x = leader_movement.get_x();
            float leader_move_y = leader_movement.get_y();
            float leader_move_intensity = leader_movement.get_intensity();
            if(leader_move_intensity < 0.75) leader_move_intensity = 0;
            if(leader_move_intensity > 1) leader_move_intensity = 1;
            if(leader_move_intensity == 0 && cur_leader_ptr->go_to_target)
                cur_leader_ptr->remove_target(true);
            else
                cur_leader_ptr->set_target(
                    cur_leader_ptr->x + leader_move_x * cur_leader_ptr->type->move_speed,
                    cur_leader_ptr->y + leader_move_y * cur_leader_ptr->type->move_speed,
                    NULL, NULL, false, NULL, true);
        }
        
        size_t n_leaders = leaders.size();
        for(size_t l = 0; l < n_leaders; l++) {
            leader* l_ptr = leaders[l];
            if(whistling) {
                if(l != cur_leader_nr) {
                    if(dist(l_ptr->x, l_ptr->y, cursor_x, cursor_y) <= whistle_radius) {
                    
                        stop_auto_pluck(l_ptr);
                        
                        if(!l_ptr->following_party && !l_ptr->was_thrown) {
                            //Leader got whistled.
                            add_to_party(cur_leader_ptr, l_ptr);
                            
                            size_t n_party_members = l_ptr->party->members.size();
                            for(size_t m = 0; m < n_party_members; m++) {
                                mob* member = l_ptr->party->members[0];
                                remove_from_party(member);
                                add_to_party(cur_leader_ptr, member);
                            }
                        }
                    }
                }
            }
            
            if(l_ptr->following_party && !l_ptr->auto_pluck_mode) {
                l_ptr->set_target(
                    0,
                    0,
                    &l_ptr->following_party->party->party_center_x,
                    &l_ptr->following_party->party->party_center_y,
                    false);
                    
            } else {
            
                if(l_ptr->auto_pluck_pikmin && l_ptr->reached_destination) {
                
                    if(l_ptr->pluck_time == -1) {
                        l_ptr->pluck_time = l_ptr->lea_type->pluck_delay;
                        l_ptr->anim.change(LEADER_ANIM_PLUCK, true, false, false);
                    }
                    
                    if(l_ptr->pluck_time > 0) {
                        l_ptr->pluck_time -= delta_t;
                        
                    } else {
                    
                        l_ptr->auto_pluck_pikmin->fsm.run_event(MOB_EVENT_PLUCKED, (void*) l_ptr, (void*) l_ptr->auto_pluck_pikmin);
                        l_ptr->auto_pluck_pikmin = NULL;
                    }
                }
                
                if(l_ptr->auto_pluck_mode) {
                    if(!l_ptr->auto_pluck_pikmin) {
                        float d;
                        pikmin* new_pikmin = get_closest_buried_pikmin(l_ptr->x, l_ptr->y, &d, true);
                        
                        if(new_pikmin && d <= AUTO_PLUCK_MAX_RADIUS) {
                            go_pluck(l_ptr, new_pikmin);
                        } else { //No more buried Pikmin, or none nearby. Give up.
                            stop_auto_pluck(l_ptr);
                        }
                    }
                }
            }
            
            if(!l_ptr->carrier_info && !whistling) {
                if(l_ptr->speed_x != 0 || l_ptr->speed_y != 0) {
                    l_ptr->anim.change(LEADER_ANIM_WALK, true, true, true);
                } else {
                    l_ptr->anim.change(LEADER_ANIM_IDLE, true, true, true);
                }
            }
            
            
        }
        
        if(cam_trans_pan_time_left > 0) {
            cam_trans_pan_final_x = cur_leader_ptr->x;
            cam_trans_pan_final_y = cur_leader_ptr->y;
        } else {
            cam_x = cur_leader_ptr->x;
            cam_y = cur_leader_ptr->y;
        }
        
        
        /***********************************
        *                             ***  *
        *   Current leader's group   ****O *
        *                             ***  *
        ************************************/
        
        dist closest_distance = 0;
        size_t n_members = cur_leader_ptr->party->members.size();
        closest_party_member = cur_leader_ptr->holding_pikmin;
        
        if(n_members > 0 && !closest_party_member) {
        
            for(size_t m = 0; m < n_members; m++) {
                dist d(cur_leader_ptr->x, cur_leader_ptr->y, cur_leader_ptr->party->members[m]->x, cur_leader_ptr->party->members[m]->y);
                if(m == 0 || d < closest_distance) {
                    closest_distance = d;
                    closest_party_member = cur_leader_ptr->party->members[m];
                }
            }
            
            if(closest_distance > MIN_GRAB_RANGE) {
                closest_party_member = NULL;
            }
        }
        
        float group_move_x = group_movement.get_x();
        float group_move_y = group_movement.get_y();
        
        if(group_move_go_to_cursor) {
            group_move_angle = cursor_angle;
            group_move_intensity = leader_to_cursor_dis.to_float() / CURSOR_MAX_DIST;
        } else if(group_move_x != 0 || group_move_y != 0) {
            coordinates_to_angle(
                group_move_x, group_move_y,
                &group_move_angle, &group_move_intensity
            );
            if(group_move_intensity > 1) group_move_intensity = 1;
        } else {
            group_move_intensity = 0;
        }
        
        if(group_move_intensity) {
            cur_leader_ptr->party->party_center_x = cur_leader_ptr->x + cos(group_move_angle) * group_move_intensity * CURSOR_MAX_DIST;
            cur_leader_ptr->party->party_center_y = cur_leader_ptr->y + sin(group_move_angle) * group_move_intensity * CURSOR_MAX_DIST;
        } else if(prev_group_move_intensity != 0) {
            float d = get_leader_to_group_center_dist(cur_leader_ptr);
            cur_leader_ptr->party->party_center_x = cur_leader_ptr->x + cos(group_move_angle) * d;
            cur_leader_ptr->party->party_center_y = cur_leader_ptr->y + sin(group_move_angle) * d;
        }
        prev_group_move_intensity = group_move_intensity;
        
        
        /********************
        *             .-.   *
        *   Cursor   ( = )> *
        *             `-´   *
        ********************/
        
        float mouse_cursor_speed_x = delta_t* MOUSE_CURSOR_MOVE_SPEED * cursor_movement.get_x();
        float mouse_cursor_speed_y = delta_t* MOUSE_CURSOR_MOVE_SPEED * cursor_movement.get_y();
        
        mouse_cursor_x += mouse_cursor_speed_x;
        mouse_cursor_y += mouse_cursor_speed_y;
        
        float mcx = mouse_cursor_x, mcy = mouse_cursor_y;
        ALLEGRO_TRANSFORM world_to_screen_transform = get_world_to_screen_transform();
        ALLEGRO_TRANSFORM screen_to_world_transform = world_to_screen_transform;
        al_invert_transform(&screen_to_world_transform);
        al_transform_coordinates(&screen_to_world_transform, &mcx, &mcy);
        cursor_x = mcx;
        cursor_y = mcy;
        
        if(!cur_leader_ptr->auto_pluck_mode && cur_leader_ptr->pluck_time == -1 && !cur_leader_ptr->carrier_info) {
            cursor_angle = atan2(cursor_y - cur_leader_ptr->y, cursor_x - cur_leader_ptr->x);
            cur_leader_ptr->face(cursor_angle);
        }
        
        leader_to_cursor_dis = dist(cur_leader_ptr->x, cur_leader_ptr->y, cursor_x, cursor_y);
        if(leader_to_cursor_dis > CURSOR_MAX_DIST) {
            //TODO with an analog stick, if the cursor is being moved, it's considered off-limit a lot more than it should.
            //Cursor goes beyond the range limit.
            cursor_x = cur_leader_ptr->x + (cos(cursor_angle) * CURSOR_MAX_DIST);
            cursor_y = cur_leader_ptr->y + (sin(cursor_angle) * CURSOR_MAX_DIST);
            
            if(mouse_cursor_speed_x != 0 || mouse_cursor_speed_y != 0) {
                //If we're speeding the mouse cursor (via analog stick), don't let it go beyond the edges.
                mouse_cursor_x = cursor_x;
                mouse_cursor_y = cursor_y;
                al_transform_coordinates(&world_to_screen_transform, &mouse_cursor_x, &mouse_cursor_y);
            }
        }
        
        
        /**************************
        *                    /  / *
        *   Percipitation     / / *
        *                   /  /  *
        **************************/
        
        if(cur_area_map.weather_condition.percipitation_type != PERCIPITATION_TYPE_NONE) {
            percipitation_time_left -= delta_t;
            if(percipitation_time_left <= 0) {
                percipitation_time_left = cur_area_map.weather_condition.percipitation_frequency.get_random_number();
                percipitation.push_back(point(0, 0));
            }
            
            for(size_t p = 0; p < percipitation.size();) {
                percipitation[p].y += cur_area_map.weather_condition.percipitation_speed.get_random_number() * delta_t;
                if(percipitation[p].y > scr_h) {
                    percipitation.erase(percipitation.begin() + p);
                } else {
                    p++;
                }
            }
        }
        
        
        /**********************
        *                 *   *
        *   Particles   *   * *
        *                ***  *
        **********************/
        
        throw_particle_timer -= delta_t;
        if(throw_particle_timer <= 0) {
            throw_particle_timer = THROW_PARTICLE_INTERVAL;
            
            for(size_t l = 0; l < n_leaders; l++) {
                if(leaders[l]->was_thrown)
                    particles.push_back(
                        particle(
                            PARTICLE_TYPE_CIRCLE, NULL, leaders[l]->x, leaders[l]->y, 0, 0, 0, 0, 0.6, leaders[l]->type->radius, change_alpha(leaders[l]->type->main_color, 128)
                        )
                    );
            }
            
            for(size_t p = 0; p < pikmin_list.size(); p++) {
                if(pikmin_list[p]->was_thrown)
                    particles.push_back(
                        particle(
                            PARTICLE_TYPE_CIRCLE, NULL, pikmin_list[p]->x, pikmin_list[p]->y, 0, 0, 0, 0, 0.6, pikmin_list[p]->type->radius, change_alpha(pikmin_list[p]->type->main_color, 128)
                        )
                    );
            }
        }
        
    } else { //Displaying a message.
    
        if(cur_message_char < cur_message_stopping_chars[cur_message_section + 1]) {
            cur_message_char_time -= delta_t;
            if(cur_message_char_time <= 0) {
                cur_message_char_time = MESSAGE_CHAR_INTERVAL;
                cur_message_char++;
            }
        }
        
    }
}
