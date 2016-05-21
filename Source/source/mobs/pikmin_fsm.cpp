/*
 * Copyright (c) Andre 'Espyo' Silva 2013-2016.
 * The following source file belongs to the open-source project
 * Pikmin fangame engine. Please read the included
 * README and LICENSE files for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Pikmin finite state machine logic.
 */

#include "../functions.h"
#include "../hazard.h"
#include "mob_fsm.h"
#include "pikmin.h"
#include "pikmin_fsm.h"
#include "../vars.h"

/* ----------------------------------------------------------------------------
 * Creates the finite state machine for the Pikmin's logic.
 */
void pikmin_fsm::create_fsm(mob_type* typ) {
    easy_fsm_creator efc;
    efc.new_state("buried", PIKMIN_STATE_BURIED); {
        efc.new_event(MOB_EVENT_ON_ENTER); {
            efc.run_function(pikmin_fsm::become_buried);
        }
        efc.new_event(MOB_EVENT_PLUCKED); {
            efc.run_function(pikmin_fsm::begin_pluck);
            efc.change_state("plucked");
        }
        efc.new_event(MOB_EVENT_LANDED); {
            efc.run_function(pikmin_fsm::stand_still);
        }
    }
    
    efc.new_state("plucked", PIKMIN_STATE_PLUCKING); {
        efc.new_event(MOB_EVENT_ANIMATION_END); {
            efc.run_function(pikmin_fsm::end_pluck);
            efc.change_state("in_group_chasing");
        }
    }
    
    efc.new_state("in_group_chasing", PIKMIN_STATE_IN_GROUP_CHASING); {
        efc.new_event(MOB_EVENT_ON_ENTER); {
            efc.run_function(pikmin_fsm::chase_leader);
        }
        efc.new_event(MOB_EVENT_GRABBED_BY_FRIEND); {
            efc.run_function(pikmin_fsm::be_grabbed_by_friend);
            efc.change_state("grabbed_by_leader");
        }
        efc.new_event(MOB_EVENT_REACHED_DESTINATION); {
            efc.change_state("in_group_stopped");
        }
        efc.new_event(MOB_EVENT_GROUP_MOVE_STARTED); {
            efc.change_state("group_move_chasing");
        }
        efc.new_event(MOB_EVENT_DISMISSED); {
            efc.run_function(pikmin_fsm::be_dismissed);
            efc.change_state("going_to_dismiss_spot");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_N_A); {
            efc.run_function(pikmin_fsm::get_knocked_down);
            efc.change_state("knocked_back");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_EAT); {
            efc.run_function(pikmin_fsm::be_grabbed_by_enemy);
            efc.change_state("grabbed_by_enemy");
        }
        efc.new_event(MOB_EVENT_TOUCHED_HAZARD); {
            efc.run_function(pikmin_fsm::touched_hazard);
        }
        efc.new_event(MOB_EVENT_TOUCHED_SPRAY); {
            efc.run_function(pikmin_fsm::touched_spray);
        }
        efc.new_event(MOB_EVENT_BOTTOMLESS_PIT); {
            efc.run_function(pikmin_fsm::fall_down_pit);
        }
    }
    
    efc.new_state("in_group_stopped", PIKMIN_STATE_IN_GROUP_STOPPED); {
        efc.new_event(MOB_EVENT_ON_ENTER); {
            efc.run_function(pikmin_fsm::stop_in_group);
        }
        efc.new_event(MOB_EVENT_ON_TICK); {
            efc.run_function(pikmin_fsm::face_leader);
        }
        efc.new_event(MOB_EVENT_GRABBED_BY_FRIEND); {
            efc.run_function(pikmin_fsm::be_grabbed_by_friend);
            efc.change_state("grabbed_by_leader");
        }
        efc.new_event(MOB_EVENT_SPOT_IS_FAR); {
            efc.change_state("in_group_chasing");
        }
        efc.new_event(MOB_EVENT_GROUP_MOVE_STARTED); {
            efc.change_state("group_move_chasing");
        }
        efc.new_event(MOB_EVENT_DISMISSED); {
            efc.run_function(pikmin_fsm::be_dismissed);
            efc.change_state("going_to_dismiss_spot");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_N_A); {
            efc.run_function(pikmin_fsm::get_knocked_down);
            efc.change_state("knocked_back");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_EAT); {
            efc.run_function(pikmin_fsm::be_grabbed_by_enemy);
            efc.change_state("grabbed_by_enemy");
        }
        efc.new_event(MOB_EVENT_TOUCHED_HAZARD); {
            efc.run_function(pikmin_fsm::touched_hazard);
        }
        efc.new_event(MOB_EVENT_TOUCHED_SPRAY); {
            efc.run_function(pikmin_fsm::touched_spray);
        }
        efc.new_event(MOB_EVENT_BOTTOMLESS_PIT); {
            efc.run_function(pikmin_fsm::fall_down_pit);
        }
    }
    
    efc.new_state("group_move_chasing", PIKMIN_STATE_GROUP_MOVE_CHASING); {
        efc.new_event(MOB_EVENT_ON_ENTER); {
            efc.run_function(pikmin_fsm::chase_leader);
        }
        efc.new_event(MOB_EVENT_ON_TICK); {
            efc.run_function(pikmin_fsm::chase_leader);
        }
        efc.new_event(MOB_EVENT_REACHED_DESTINATION); {
            efc.change_state("group_move_stopped");
        }
        efc.new_event(MOB_EVENT_GROUP_MOVE_ENDED); {
            efc.change_state("in_group_chasing");
        }
        efc.new_event(MOB_EVENT_DISMISSED); {
            efc.run_function(pikmin_fsm::be_dismissed);
            efc.change_state("going_to_dismiss_spot");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_N_A); {
            efc.run_function(pikmin_fsm::get_knocked_down);
            efc.change_state("knocked_back");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_EAT); {
            efc.run_function(pikmin_fsm::be_grabbed_by_enemy);
            efc.change_state("grabbed_by_enemy");
        }
        efc.new_event(MOB_EVENT_NEAR_OPPONENT); {
            efc.run_function(pikmin_fsm::go_to_opponent);
            efc.change_state("going_to_opponent");
        }
        efc.new_event(MOB_EVENT_NEAR_CARRIABLE_OBJECT); {
            efc.run_function(pikmin_fsm::go_to_carriable_object);
            efc.change_state("going_to_carriable_object");
        }
        efc.new_event(MOB_EVENT_TOUCHED_HAZARD); {
            efc.run_function(pikmin_fsm::touched_hazard);
        }
        efc.new_event(MOB_EVENT_TOUCHED_SPRAY); {
            efc.run_function(pikmin_fsm::touched_spray);
        }
        efc.new_event(MOB_EVENT_BOTTOMLESS_PIT); {
            efc.run_function(pikmin_fsm::fall_down_pit);
        }
    }
    
    efc.new_state("group_move_stopped", PIKMIN_STATE_GROUP_MOVE_STOPPED); {
        efc.new_event(MOB_EVENT_ON_ENTER); {
            efc.run_function(pikmin_fsm::stop_in_group);
        }
        efc.new_event(MOB_EVENT_ON_TICK); {
            efc.run_function(pikmin_fsm::face_leader);
        }
        efc.new_event(MOB_EVENT_REACHED_DESTINATION); {
            efc.change_state("group_move_stopped");
        }
        efc.new_event(MOB_EVENT_SPOT_IS_FAR); {
            efc.change_state("group_move_chasing");
        }
        efc.new_event(MOB_EVENT_GROUP_MOVE_ENDED); {
            efc.change_state("in_group_chasing");
        }
        efc.new_event(MOB_EVENT_DISMISSED); {
            efc.run_function(pikmin_fsm::be_dismissed);
            efc.change_state("going_to_dismiss_spot");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_N_A); {
            efc.run_function(pikmin_fsm::get_knocked_down);
            efc.change_state("knocked_back");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_EAT); {
            efc.run_function(pikmin_fsm::be_grabbed_by_enemy);
            efc.change_state("grabbed_by_enemy");
        }
        efc.new_event(MOB_EVENT_NEAR_OPPONENT); {
            efc.run_function(pikmin_fsm::go_to_opponent);
            efc.change_state("going_to_opponent");
        }
        efc.new_event(MOB_EVENT_NEAR_CARRIABLE_OBJECT); {
            efc.run_function(pikmin_fsm::go_to_carriable_object);
            efc.change_state("going_to_carriable_object");
        }
        efc.new_event(MOB_EVENT_TOUCHED_HAZARD); {
            efc.run_function(pikmin_fsm::touched_hazard);
        }
        efc.new_event(MOB_EVENT_TOUCHED_SPRAY); {
            efc.run_function(pikmin_fsm::touched_spray);
        }
        efc.new_event(MOB_EVENT_BOTTOMLESS_PIT); {
            efc.run_function(pikmin_fsm::fall_down_pit);
        }
    }
    
    efc.new_state("grabbed_by_leader", PIKMIN_STATE_GRABBED_BY_LEADER); {
        efc.new_event(MOB_EVENT_ON_LEAVE); {
            efc.run_function(pikmin_fsm::be_released);
        }
        efc.new_event(MOB_EVENT_THROWN); {
            efc.run_function(pikmin_fsm::be_thrown);
            efc.change_state("thrown");
        }
        efc.new_event(MOB_EVENT_RELEASED); {
            efc.change_state("in_group_chasing");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_N_A); {
            efc.run_function(pikmin_fsm::get_knocked_down);
            efc.change_state("knocked_back");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_EAT); {
            efc.run_function(pikmin_fsm::be_grabbed_by_enemy);
            efc.change_state("grabbed_by_enemy");
        }
        efc.new_event(MOB_EVENT_TOUCHED_HAZARD); {
            efc.run_function(pikmin_fsm::touched_hazard);
        }
        efc.new_event(MOB_EVENT_TOUCHED_SPRAY); {
            efc.run_function(pikmin_fsm::touched_spray);
        }
        efc.new_event(MOB_EVENT_BOTTOMLESS_PIT); {
            efc.run_function(pikmin_fsm::fall_down_pit);
        }
    }
    
    efc.new_state("thrown", PIKMIN_STATE_THROWN); {
        efc.new_event(MOB_EVENT_LANDED); {
            efc.run_function(pikmin_fsm::land);
            efc.change_state("idle");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_A_N); {
            efc.run_function(pikmin_fsm::land_on_mob);
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_EAT); {
            efc.run_function(pikmin_fsm::be_grabbed_by_enemy);
            efc.change_state("grabbed_by_enemy");
        }
        efc.new_event(MOB_EVENT_TOUCHED_HAZARD); {
            efc.run_function(pikmin_fsm::touched_hazard);
        }
        efc.new_event(MOB_EVENT_TOUCHED_SPRAY); {
            efc.run_function(pikmin_fsm::touched_spray);
        }
        efc.new_event(MOB_EVENT_BOTTOMLESS_PIT); {
            efc.run_function(pikmin_fsm::fall_down_pit);
        }
    }
    
    efc.new_state(
        "going_to_dismiss_spot", PIKMIN_STATE_GOING_TO_DISMISS_SPOT
    ); {
        efc.new_event(MOB_EVENT_WHISTLED); {
            efc.run_function(pikmin_fsm::called);
            efc.change_state("in_group_chasing");
        }
        efc.new_event(MOB_EVENT_REACHED_DESTINATION); {
            efc.run_function(pikmin_fsm::reach_dismiss_spot);
            efc.change_state("idle");
        }
        efc.new_event(MOB_EVENT_NEAR_OPPONENT); {
            efc.run_function(pikmin_fsm::go_to_opponent);
            efc.change_state("going_to_opponent");
        }
        efc.new_event(MOB_EVENT_NEAR_CARRIABLE_OBJECT); {
            efc.run_function(pikmin_fsm::go_to_carriable_object);
            efc.change_state("going_to_carriable_object");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_N_A); {
            efc.run_function(pikmin_fsm::get_knocked_down);
            efc.change_state("knocked_back");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_EAT); {
            efc.run_function(pikmin_fsm::be_grabbed_by_enemy);
            efc.change_state("grabbed_by_enemy");
        }
        efc.new_event(MOB_EVENT_TOUCHED_HAZARD); {
            efc.run_function(pikmin_fsm::touched_hazard);
        }
        efc.new_event(MOB_EVENT_TOUCHED_SPRAY); {
            efc.run_function(pikmin_fsm::touched_spray);
        }
        efc.new_event(MOB_EVENT_BOTTOMLESS_PIT); {
            efc.run_function(pikmin_fsm::fall_down_pit);
        }
    }
    
    efc.new_state("idle", PIKMIN_STATE_IDLE); {
        efc.new_event(MOB_EVENT_ON_ENTER); {
            efc.run_function(pikmin_fsm::become_idle);
        }
        efc.new_event(MOB_EVENT_ON_LEAVE); {
            efc.run_function(pikmin_fsm::stop_being_idle);
        }
        efc.new_event(MOB_EVENT_NEAR_OPPONENT); {
            efc.run_function(pikmin_fsm::go_to_opponent);
            efc.change_state("going_to_opponent");
        }
        efc.new_event(MOB_EVENT_NEAR_CARRIABLE_OBJECT); {
            efc.run_function(pikmin_fsm::go_to_carriable_object);
            efc.change_state("going_to_carriable_object");
        }
        efc.new_event(MOB_EVENT_WHISTLED); {
            efc.run_function(pikmin_fsm::called);
            efc.change_state("in_group_chasing");
        }
        efc.new_event(MOB_EVENT_TOUCHED_LEADER); {
            efc.run_function(pikmin_fsm::called);
            efc.change_state("in_group_chasing");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_N_A); {
            efc.run_function(pikmin_fsm::get_knocked_down);
            efc.change_state("knocked_back");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_EAT); {
            efc.run_function(pikmin_fsm::be_grabbed_by_enemy);
            efc.change_state("grabbed_by_enemy");
        }
        efc.new_event(MOB_EVENT_TOUCHED_HAZARD); {
            efc.run_function(pikmin_fsm::touched_hazard);
        }
        efc.new_event(MOB_EVENT_TOUCHED_SPRAY); {
            efc.run_function(pikmin_fsm::touched_spray);
        }
        efc.new_event(MOB_EVENT_BOTTOMLESS_PIT); {
            efc.run_function(pikmin_fsm::fall_down_pit);
        }
    }
    
    efc.new_state("going_to_opponent", PIKMIN_STATE_GOING_TO_OPPONENT); {
        efc.new_event(MOB_EVENT_REACHED_DESTINATION); {
            efc.change_state("attacking_grounded");
        }
        efc.new_event(MOB_EVENT_WHISTLED); {
            efc.run_function(pikmin_fsm::called);
            efc.change_state("in_group_chasing");
        }
        efc.new_event(MOB_EVENT_LOST_FOCUSED_MOB); {
            efc.change_state("idle");
        }
        efc.new_event(MOB_EVENT_FOCUSED_MOB_DIED); {
            efc.change_state("idle");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_N_A); {
            efc.run_function(pikmin_fsm::get_knocked_down);
            efc.change_state("knocked_back");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_EAT); {
            efc.run_function(pikmin_fsm::be_grabbed_by_enemy);
            efc.change_state("grabbed_by_enemy");
        }
        efc.new_event(MOB_EVENT_TOUCHED_HAZARD); {
            efc.run_function(pikmin_fsm::touched_hazard);
        }
        efc.new_event(MOB_EVENT_TOUCHED_SPRAY); {
            efc.run_function(pikmin_fsm::touched_spray);
        }
        efc.new_event(MOB_EVENT_BOTTOMLESS_PIT); {
            efc.run_function(pikmin_fsm::fall_down_pit);
        }
    }
    
    efc.new_state(
        "going_to_carriable_object", PIKMIN_STATE_GOING_TO_CARRIABLE_OBJECT
    ); {
        efc.new_event(MOB_EVENT_REACHED_DESTINATION); {
            efc.run_function(pikmin_fsm::reach_carriable_object);
            efc.change_state("carrying");
        }
        efc.new_event(MOB_EVENT_FOCUSED_MOB_UNCARRIABLE); {
            efc.run_function(pikmin_fsm::forget_carriable_object);
            efc.change_state("idle");
        }
        efc.new_event(MOB_EVENT_TIMER); {
            efc.run_function(pikmin_fsm::forget_carriable_object);
            efc.change_state("sighing");
        }
        efc.new_event(MOB_EVENT_WHISTLED); {
            efc.run_function(pikmin_fsm::forget_carriable_object);
            efc.run_function(pikmin_fsm::called);
            efc.change_state("in_group_chasing");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_N_A); {
            efc.run_function(pikmin_fsm::forget_carriable_object);
            efc.run_function(pikmin_fsm::get_knocked_down);
            efc.change_state("knocked_back");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_EAT); {
            efc.run_function(pikmin_fsm::forget_carriable_object);
            efc.run_function(pikmin_fsm::be_grabbed_by_enemy);
            efc.change_state("grabbed_by_enemy");
        }
        efc.new_event(MOB_EVENT_TOUCHED_HAZARD); {
            efc.run_function(pikmin_fsm::touched_hazard);
        }
        efc.new_event(MOB_EVENT_TOUCHED_SPRAY); {
            efc.run_function(pikmin_fsm::touched_spray);
        }
        efc.new_event(MOB_EVENT_BOTTOMLESS_PIT); {
            efc.run_function(pikmin_fsm::forget_carriable_object);
            efc.run_function(pikmin_fsm::fall_down_pit);
        }
    }
    
    efc.new_state("sighing", PIKMIN_STATE_SIGHING); {
        efc.new_event(MOB_EVENT_ON_ENTER); {
            efc.run_function(pikmin_fsm::stand_still);
            efc.run_function(pikmin_fsm::sigh);
        }
        efc.new_event(MOB_EVENT_ANIMATION_END); {
            efc.change_state("idle");
        }
        efc.new_event(MOB_EVENT_WHISTLED); {
            efc.run_function(pikmin_fsm::called);
            efc.change_state("in_group_chasing");
        }
        efc.new_event(MOB_EVENT_TOUCHED_LEADER); {
            efc.run_function(pikmin_fsm::called);
            efc.change_state("in_group_chasing");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_N_A); {
            efc.run_function(pikmin_fsm::get_knocked_down);
            efc.change_state("knocked_back");
        }
        efc.new_event(MOB_EVENT_TOUCHED_HAZARD); {
            efc.run_function(pikmin_fsm::touched_hazard);
        }
        efc.new_event(MOB_EVENT_TOUCHED_SPRAY); {
            efc.run_function(pikmin_fsm::touched_spray);
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_EAT); {
            efc.run_function(pikmin_fsm::be_grabbed_by_enemy);
            efc.change_state("grabbed_by_enemy");
        }
    }
    
    efc.new_state("carrying", PIKMIN_STATE_CARRYING); {
        efc.new_event(MOB_EVENT_ON_LEAVE); {
            efc.run_function(pikmin_fsm::stop_carrying);
            efc.run_function(pikmin_fsm::stand_still);
        }
        efc.new_event(MOB_EVENT_WHISTLED); {
            efc.run_function(pikmin_fsm::called);
            efc.change_state("in_group_chasing");
        }
        efc.new_event(MOB_EVENT_FINISHED_CARRYING); {
            efc.change_state("idle");
        }
        efc.new_event(MOB_EVENT_LOST_FOCUSED_MOB); {
            efc.change_state("idle");
        }
        efc.new_event(MOB_EVENT_FOCUSED_MOB_UNCARRIABLE); {
            efc.change_state("idle");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_N_A); {
            efc.run_function(pikmin_fsm::get_knocked_down);
            efc.change_state("knocked_back");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_EAT); {
            efc.run_function(pikmin_fsm::be_grabbed_by_enemy);
            efc.change_state("grabbed_by_enemy");
        }
        efc.new_event(MOB_EVENT_TOUCHED_HAZARD); {
            efc.run_function(pikmin_fsm::touched_hazard);
        }
        efc.new_event(MOB_EVENT_TOUCHED_SPRAY); {
            efc.run_function(pikmin_fsm::touched_spray);
        }
        efc.new_event(MOB_EVENT_BOTTOMLESS_PIT); {
            efc.run_function(pikmin_fsm::fall_down_pit);
        }
    }
    
    efc.new_state("attacking_grounded", PIKMIN_STATE_ATTACKING_GROUNDED); {
        efc.new_event(MOB_EVENT_ON_ENTER); {
            efc.run_function(pikmin_fsm::prepare_to_attack);
        }
        efc.new_event(MOB_EVENT_ON_TICK); {
            efc.run_function(pikmin_fsm::tick_attacking_grounded);
        }
        efc.new_event(MOB_EVENT_WHISTLED); {
            efc.run_function(pikmin_fsm::called);
            efc.change_state("in_group_chasing");
        }
        efc.new_event(MOB_EVENT_ANIMATION_END); {
            efc.run_function(pikmin_fsm::rechase_opponent);
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_N_A); {
            efc.run_function(pikmin_fsm::get_knocked_down);
            efc.change_state("knocked_back");
        }
        efc.new_event(MOB_EVENT_TOUCHED_HAZARD); {
            efc.run_function(pikmin_fsm::touched_hazard);
        }
        efc.new_event(MOB_EVENT_TOUCHED_SPRAY); {
            efc.run_function(pikmin_fsm::touched_spray);
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_EAT); {
            efc.run_function(pikmin_fsm::be_grabbed_by_enemy);
            efc.change_state("grabbed_by_enemy");
        }
    }
    
    efc.new_state("attacking_latched", PIKMIN_STATE_ATTACKING_LATCHED); {
        efc.new_event(MOB_EVENT_ON_ENTER); {
            efc.run_function(pikmin_fsm::prepare_to_attack);
        }
        efc.new_event(MOB_EVENT_ON_TICK); {
            efc.run_function(pikmin_fsm::tick_latched);
        }
        efc.new_event(MOB_EVENT_WHISTLED); {
            efc.run_function(pikmin_fsm::called);
            efc.change_state("in_group_chasing");
        }
        efc.new_event(MOB_EVENT_FOCUSED_MOB_DIED); {
            efc.run_function(pikmin_fsm::lose_latched_mob);
            efc.change_state("idle");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_N_A); {
            efc.run_function(pikmin_fsm::get_knocked_down);
            efc.change_state("knocked_back");
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_EAT); {
            efc.run_function(pikmin_fsm::be_grabbed_by_enemy);
            efc.change_state("grabbed_by_enemy");
        }
        efc.new_event(MOB_EVENT_TOUCHED_HAZARD); {
            efc.run_function(pikmin_fsm::touched_hazard);
        }
        efc.new_event(MOB_EVENT_TOUCHED_SPRAY); {
            efc.run_function(pikmin_fsm::touched_spray);
        }
        efc.new_event(MOB_EVENT_BOTTOMLESS_PIT); {
            efc.run_function(pikmin_fsm::fall_down_pit);
        }
    }
    
    efc.new_state("grabbed_by_enemy", PIKMIN_STATE_GRABBED_BY_ENEMY); {
        efc.new_event(MOB_EVENT_RELEASED); {
            efc.change_state("idle");
        }
        efc.new_event(MOB_EVENT_ON_TICK); {
            efc.run_function(pikmin_fsm::tick_grabbed_by_enemy);
        }
    }
    
    efc.new_state("knocked_back", PIKMIN_STATE_KNOCKED_BACK); {
        efc.new_event(MOB_EVENT_ANIMATION_END); {
            efc.run_function(pikmin_fsm::stand_still);
            efc.change_state("idle");
        }
        efc.new_event(MOB_EVENT_LANDED); {
            efc.run_function(pikmin_fsm::stand_still);
        }
        efc.new_event(MOB_EVENT_HITBOX_TOUCH_EAT); {
            efc.run_function(pikmin_fsm::be_grabbed_by_enemy);
            efc.change_state("grabbed_by_enemy");
        }
        efc.new_event(MOB_EVENT_TOUCHED_HAZARD); {
            efc.run_function(pikmin_fsm::touched_hazard);
        }
        efc.new_event(MOB_EVENT_TOUCHED_SPRAY); {
            efc.run_function(pikmin_fsm::touched_spray);
        }
        efc.new_event(MOB_EVENT_BOTTOMLESS_PIT); {
            efc.run_function(pikmin_fsm::fall_down_pit);
        }
    }
    
    efc.new_state("flailing", PIKMIN_STATE_FLAILING); {
        efc.new_event(MOB_EVENT_ON_ENTER); {
            efc.run_function(pikmin_fsm::start_flailing);
        }
        efc.new_event(MOB_EVENT_TIMER); {
            efc.run_function(pikmin_fsm::stand_still);
        }
        efc.new_event(MOB_EVENT_LEFT_HAZARD); {
            efc.run_function(pikmin_fsm::check_remove_flailing);
        }
        efc.new_event(MOB_EVENT_WHISTLED); {
            efc.run_function(pikmin_fsm::flail_to_whistle);
        }
        efc.new_event(MOB_EVENT_BOTTOMLESS_PIT); {
            efc.run_function(pikmin_fsm::fall_down_pit);
        }
    }
    
    efc.new_state("panic", PIKMIN_STATE_PANIC); {
        efc.new_event(MOB_EVENT_ON_ENTER); {
            efc.run_function(pikmin_fsm::start_panicking);
        }
        efc.new_event(MOB_EVENT_TIMER); {
            efc.run_function(pikmin_fsm::panic_new_chase);
        }
        efc.new_event(MOB_EVENT_WHISTLED); {
            efc.run_function(pikmin_fsm::called);
            efc.change_state("in_group_chasing");
        }
        efc.new_event(MOB_EVENT_BOTTOMLESS_PIT); {
            efc.run_function(pikmin_fsm::fall_down_pit);
        }
    }
    
    efc.new_state("celebrating", PIKMIN_STATE_CELEBRATING); {
        //TODO
    }
    
    typ->states = efc.finish();
    typ->first_state_nr = fix_states(typ->states, "idle");
    
    if(typ->states.size() != N_PIKMIN_STATES) {
        log_error(
            "ENGINE WARNING: Number of Pikmin states on the FSM (" +
            i2s(typ->states.size()) +
            ") and the enum (" + i2s(N_PIKMIN_STATES) + ") do not match."
        );
    }
}


/* ----------------------------------------------------------------------------
 * When a Pikmin becomes buried.
 */
void pikmin_fsm::become_buried(mob* m, void* info1, void* info2) {
    m->set_animation(PIKMIN_ANIM_BURROWED);
}


/* ----------------------------------------------------------------------------
 * Makes a Pikmin begin its plucking process.
 * info1: Pointer to the leader that plucked.
 */
void pikmin_fsm::begin_pluck(mob* m, void* info1, void* info2) {
    pikmin* pik = (pikmin*) m;
    mob* lea = (mob*) info1;
    
    if(lea->following_group) {
        if(typeid(*lea->following_group) == typeid(leader)) {
            //If this leader is following another one,
            //then the new Pikmin should be in the group of that top leader.
            lea = lea->following_group;
        }
    }
    
    pik->set_animation(PIKMIN_ANIM_PLUCKING);
    add_to_group(lea, pik);
}


/* ----------------------------------------------------------------------------
 * Makes a Pikmin finish its plucking process.
 */
void pikmin_fsm::end_pluck(mob* m, void* info1, void* info2) {
    pikmin* pik = (pikmin*) m;
    pik->set_animation(PIKMIN_ANIM_IDLE);
    sfx_pikmin_plucked.play(0, false);
    sfx_pikmin_pluck.play(0, false);
}


/* ----------------------------------------------------------------------------
 * When a Pikmin is grabbed by a leader.
 */
void pikmin_fsm::be_grabbed_by_friend(mob* m, void* info1, void* info2) {
    sfx_pikmin_held.play(0, false);
    m->set_animation(PIKMIN_ANIM_IDLE);
}


/* ----------------------------------------------------------------------------
 * When a Pikmin is grabbed by an enemy.
 * info1: Pointer to the mob.
 * info2: Pointer to the hitbox instance that grabbed.
 */
void pikmin_fsm::be_grabbed_by_enemy(mob* m, void* info1, void* info2) {
    pikmin* pik_ptr = (pikmin*) m;
    mob* mob_ptr = (mob*) info1;
    hitbox_instance* hi_ptr = (hitbox_instance*) info2;
    
    pik_ptr->set_connected_hitbox_info(hi_ptr, mob_ptr);
    
    pik_ptr->focused_mob = mob_ptr;
    
    sfx_pikmin_caught.play(0.2, 0);
    pik_ptr->set_animation(PIKMIN_ANIM_IDLE);
    remove_from_group(pik_ptr);
}


/* ----------------------------------------------------------------------------
 * When a Pikmin is dismissed by its leader.
 * info1: Pointer to the world X coordinate to go to.
 * info2: Pointer to the world Y coordinate to go to.
 */
void pikmin_fsm::be_dismissed(mob* m, void* info1, void* info2) {
    m->chase(
        *(float*) info1,
        *(float*) info2,
        NULL,
        NULL,
        false
    );
    sfx_pikmin_idle.play(0, false);
    
    m->set_animation(PIKMIN_ANIM_IDLE);
}


/* ----------------------------------------------------------------------------
 * When a Pikmin reaches its dismissal spot.
 */
void pikmin_fsm::reach_dismiss_spot(mob* m, void* info1, void* info2) {
    m->stop_chasing();
    m->set_animation(PIKMIN_ANIM_IDLE);
}


/* ----------------------------------------------------------------------------
 * When a Pikmin becomes idle.
 */
void pikmin_fsm::become_idle(mob* m, void* info1, void* info2) {
    m->set_animation(PIKMIN_ANIM_IDLE);
    unfocus_mob(m);
}


/* ----------------------------------------------------------------------------
 * When a Pikmin is thrown by a leader.
 */
void pikmin_fsm::be_thrown(mob* m, void* info1, void* info2) {
    m->stop_chasing();
    sfx_pikmin_held.stop();
    sfx_pikmin_thrown.stop();
    sfx_pikmin_thrown.play(0, false);
    m->set_animation(PIKMIN_ANIM_THROWN);
}


/* ----------------------------------------------------------------------------
 * When a Pikmin is gently released by a leader.
 */
void pikmin_fsm::be_released(mob* m, void* info1, void* info2) {

}


/* ----------------------------------------------------------------------------
 * When a thrown Pikmin lands.
 */
void pikmin_fsm::land(mob* m, void* info1, void* info2) {
    m->set_animation(PIKMIN_ANIM_IDLE);
    pikmin_fsm::stand_still(m, NULL, NULL);
}


/* ----------------------------------------------------------------------------
 * When a Pikmin is meant to sigh.
 */
void pikmin_fsm::sigh(mob* m, void* info1, void* info2) {
    m->set_animation(PIKMIN_ANIM_SIGH);
}


/* ----------------------------------------------------------------------------
 * When a Pikmin is meant to stand still in place.
 */
void pikmin_fsm::stand_still(mob* m, void* info1, void* info2) {
    m->stop_chasing();
    m->speed_x = m->speed_y = 0;
}


/* ----------------------------------------------------------------------------
 * When a Pikmin is called over by a leader, either by being whistled,
 * or touched when idle.
 */
void pikmin_fsm::called(mob* m, void* info1, void* info2) {
    pikmin* pik = (pikmin*) m;
    
    for(size_t s = 0; s < m->statuses.size(); ++s) {
        if(m->statuses[s].type->removable_with_whistle) {
            m->statuses[s].to_delete = true;
        }
    }
    m->delete_old_status_effects();
    
    pik->attack_time = 0;
    add_to_group(cur_leader_ptr, pik);
    sfx_pikmin_called.play(0.03, false);
}


/* ----------------------------------------------------------------------------
 * When a Pikmin is hit by an attack and gets knocked back.
 * info1: Pointer to the hitbox touch information structure.
 */
void pikmin_fsm::get_knocked_down(mob* m, void* info1, void* info2) {
    hitbox_touch_info* info = (hitbox_touch_info*) info1;
    float knockback = 0;
    float knockback_angle = 0;
    
    calculate_knockback(
        info->mob2, m, info->hi2, info->hi1, &knockback, &knockback_angle
    );
    apply_knockback(m, knockback, knockback_angle);
    
    m->set_animation(PIKMIN_ANIM_LYING);
    
    remove_from_group(m);
}


/* ----------------------------------------------------------------------------
 * When a Pikmin needs to walk towards an opponent.
 * info1: Pointer to the opponent.
 */
void pikmin_fsm::go_to_opponent(mob* m, void* info1, void* info2) {
    focus_mob(m, (mob*) info1);
    m->stop_chasing();
    m->chase(
        0, 0,
        &m->focused_mob->x, &m->focused_mob->y,
        false, nullptr, false,
        m->focused_mob->type->radius + m->type->radius + PIKMIN_ATTACK_RANGE
    );
    m->set_animation(PIKMIN_ANIM_WALK);
    remove_from_group(m);
}


/* ----------------------------------------------------------------------------
 * When a Pikmin that just attacked an opponent needs to walk
 * towards it again.
 */
void pikmin_fsm::rechase_opponent(mob* m, void* info1, void* info2) {
    if(
        m->focused_mob &&
        m->focused_mob->health > 0 &&
        dist(m->x, m->y, m->focused_mob->x, m->focused_mob->y) <=
        (m->type->radius + m->focused_mob->type->radius + PIKMIN_ATTACK_RANGE)
    ) {
        return;
    }
    
    m->fsm.set_state(PIKMIN_STATE_IDLE);
}


/* ----------------------------------------------------------------------------
 * When a Pikmin needs to go towards its spot on a carriable object.
 * info1: Pointer to the mob to carry.
 */
void pikmin_fsm::go_to_carriable_object(mob* m, void* info1, void* info2) {
    mob* carriable_mob = (mob*) info1;
    pikmin* pik_ptr = (pikmin*) m;
    
    pik_ptr->carrying_mob = carriable_mob;
    pik_ptr->stop_chasing();
    
    size_t closest_spot = INVALID;
    dist closest_spot_dist;
    carrier_spot_struct* closest_spot_ptr;
    
    for(size_t s = 0; s < carriable_mob->type->max_carriers; ++s) {
        carrier_spot_struct* s_ptr = &carriable_mob->carry_info->spot_info[s];
        if(s_ptr->state != CARRY_SPOT_FREE) continue;
        
        dist d(
            pik_ptr->x,
            pik_ptr->y,
            carriable_mob->x + s_ptr->x,
            carriable_mob->y + s_ptr->y
        );
        if(closest_spot == INVALID || d < closest_spot_dist) {
            closest_spot = s;
            closest_spot_dist = d;
            closest_spot_ptr = s_ptr;
        }
    }
    
    pik_ptr->carrying_spot = closest_spot;
    closest_spot_ptr->state = CARRY_SPOT_RESERVED;
    closest_spot_ptr->pik_ptr = pik_ptr;
    
    pik_ptr->carrying_mob->fsm.run_event(MOB_EVENT_CARRY_WAIT_UP);
    
    pik_ptr->chase(
        closest_spot_ptr->x,
        closest_spot_ptr->y,
        &carriable_mob->x,
        &carriable_mob->y,
        false, nullptr, false,
        pik_ptr->type->radius * 1.2
    );
    pik_ptr->set_animation(PIKMIN_ANIM_WALK);
    remove_from_group(pik_ptr);
    
    pik_ptr->set_timer(PIKMIN_GOTO_TIMEOUT);
}


/* ----------------------------------------------------------------------------
 * When a Pikmin reaches its spot on a carriable object.
 */
void pikmin_fsm::reach_carriable_object(mob* m, void* info1, void* info2) {
    pikmin* pik_ptr = (pikmin*) m;
    mob* carriable_mob = pik_ptr->carrying_mob;
    
    pik_ptr->set_animation(PIKMIN_ANIM_GRAB, true);
    
    float final_x =
        carriable_mob->x +
        carriable_mob->carry_info->spot_info[pik_ptr->carrying_spot].x;
    float final_y =
        carriable_mob->y +
        carriable_mob->carry_info->spot_info[pik_ptr->carrying_spot].y;
        
    pik_ptr->chase(
        carriable_mob->carry_info->spot_info[pik_ptr->carrying_spot].x,
        carriable_mob->carry_info->spot_info[pik_ptr->carrying_spot].y,
        &carriable_mob->x, &carriable_mob->y,
        true, &carriable_mob->z
    );
    
    pik_ptr->face(
        atan2(carriable_mob->y - final_y, carriable_mob->x - final_x)
    );
    
    pik_ptr->set_animation(PIKMIN_ANIM_CARRY);
    
    //Let the carriable mob know that a new Pikmin has grabbed on.
    pik_ptr->carrying_mob->fsm.run_event(MOB_EVENT_CARRY_KEEP_GOING);
    pik_ptr->carrying_mob->fsm.run_event(
        MOB_EVENT_CARRIER_ADDED, (void*) pik_ptr
    );
    
}


/* ----------------------------------------------------------------------------
 * When a Pikmin is meant to drop the object it's carrying, or
 * stop chasing the object if it's not carrying it yet, but wants to.
 */
void pikmin_fsm::forget_carriable_object(mob* m, void* info1, void* info2) {
    pikmin* p = (pikmin*) m;
    
    p->carrying_mob->carry_info->spot_info[p->carrying_spot].state =
        CARRY_SPOT_FREE;
    p->carrying_mob->carry_info->spot_info[p->carrying_spot].pik_ptr =
        NULL;
        
    p->carrying_mob->fsm.run_event(MOB_EVENT_CARRY_KEEP_GOING);
    
    p->carrying_mob = NULL;
    p->set_timer(0);
}


/* ----------------------------------------------------------------------------
 * When a Pikmin is meant to release an object it is carrying.
 */
void pikmin_fsm::stop_carrying(mob* m, void* info1, void* info2) {
    pikmin* p = (pikmin*) m;
    if(!p->carrying_mob) return;
    
    p->carrying_mob->fsm.run_event(MOB_EVENT_CARRIER_REMOVED, (void*) p);
    
    p->carrying_mob = NULL;
    p->set_timer(0);
}


/* ----------------------------------------------------------------------------
 * When a Pikmin needs to decide a new spot to run off to whilst
 * in panic.
 */
void pikmin_fsm::panic_new_chase(mob* m, void* info1, void* info2) {
    m->chase(
        m->x + randomf(-1000, 1000), m->y + randomf(-1000, 1000),
        NULL, NULL, false
    );
    m->set_timer(PIKMIN_PANIC_CHASE_INTERVAL);
}


/* ----------------------------------------------------------------------------
 * When a Pikmin is meant to reel back to unleash an attack.
 */
void pikmin_fsm::prepare_to_attack(mob* m, void* info1, void* info2) {
    pikmin* p = (pikmin*) m;
    p->set_animation(PIKMIN_ANIM_ATTACK);
    ((pikmin*) p)->attack_time = p->pik_type->attack_interval;
}


/* ----------------------------------------------------------------------------
 * When a thrown Pikmin lands on a mob, to latch on to it.
 * info1: Pointer to the hitbox touch information structure.
 */
void pikmin_fsm::land_on_mob(mob* m, void* info1, void* info2) {
    pikmin* pik_ptr = (pikmin*) m;
    hitbox_touch_info* info = (hitbox_touch_info*) info1;
    
    mob* mob_ptr = info->mob2;
    hitbox_instance* hi_ptr = info->hi2;
    
    if(!hi_ptr || !hi_ptr->can_pikmin_latch) {
        //No good for latching on. Make it act like it landed on the ground.
        m->fsm.run_event(MOB_EVENT_LANDED);
        return;
    }
    
    pik_ptr->connected_hitbox_nr = hi_ptr->hitbox_nr;
    pik_ptr->speed_x = pik_ptr->speed_y = pik_ptr->speed_z = 0;
    
    pik_ptr->focused_mob = mob_ptr;
    pik_ptr->set_connected_hitbox_info(hi_ptr, mob_ptr);
    
    pik_ptr->was_thrown = false;
    
    pik_ptr->fsm.set_state(PIKMIN_STATE_ATTACKING_LATCHED);
    
}


/* ----------------------------------------------------------------------------
 * When the mob the Pikmin is latched on to disappears.
 */
void pikmin_fsm::lose_latched_mob(mob* m, void* info1, void* info2) {
    m->stop_chasing();
}


/* ----------------------------------------------------------------------------
 * When a frame has passed while the Pikmin is being grabbed by an enemy.
 */
void pikmin_fsm::tick_grabbed_by_enemy(mob* m, void* info1, void* info2) {
    pikmin* pik_ptr = (pikmin*) m;
    if(!pik_ptr->focused_mob) return;
    
    pik_ptr->teleport_to_connected_hitbox();
}


/* ----------------------------------------------------------------------------
 * When a frame has passed while the Pikmin is latched on to an enemy.
 */
void pikmin_fsm::tick_latched(mob* m, void* info1, void* info2) {
    pikmin* pik_ptr = (pikmin*) m;
    if(!pik_ptr->focused_mob) return;
    
    pik_ptr->teleport_to_connected_hitbox();
    
    pik_ptr->attack_time -= delta_t;
    
    if(pik_ptr->attack_time <= 0) {
        pik_ptr->do_attack(
            pik_ptr->focused_mob,
            get_hitbox_instance(
                pik_ptr->focused_mob, pik_ptr->connected_hitbox_nr
            )
        );
    }
}


/* ----------------------------------------------------------------------------
 * When a frame has passed while a Pikmin is attacking on the ground.
 */
void pikmin_fsm::tick_attacking_grounded(mob* m, void* info1, void* info2) {
    pikmin* pik_ptr = (pikmin*) m;
    pik_ptr->attack_time -= delta_t;
    
    if(!pik_ptr->focused_mob || pik_ptr->focused_mob->dead) {
        return;
    }
    if(pik_ptr->attack_time <= 0) {
        if(
            !(
                (
                    pik_ptr->focused_mob->z >
                    pik_ptr->z + pik_ptr->type->height
                ) ||
                (
                    pik_ptr->focused_mob->z +
                    pik_ptr->focused_mob->type->height <
                    pik_ptr->z
                )
            )
        ) {
            pik_ptr->do_attack(
                pik_ptr->focused_mob,
                get_hitbox_instance(
                    pik_ptr->focused_mob, pik_ptr->connected_hitbox_nr
                )
            );
        }
        pik_ptr->attack_time = pik_ptr->pik_type->attack_interval;
    }
    
    pik_ptr->face(
        atan2(
            pik_ptr->focused_mob->y - pik_ptr->y,
            pik_ptr->focused_mob->x - pik_ptr->x
        )
    );
}


/* ----------------------------------------------------------------------------
 * When a Pikmin needs to turn towards its leader.
 */
void pikmin_fsm::face_leader(mob* m, void* info1, void* info2) {
    m->face(
        atan2(
            m->following_group->y - m->y,
            m->following_group->x - m->x
        )
    );
}


/* ----------------------------------------------------------------------------
 * When a Pikmin falls down a bottomless pit.
 */
void pikmin_fsm::fall_down_pit(mob* m, void* info1, void* info2) {
    m->health = 0;
}


/* ----------------------------------------------------------------------------
 * When a Pikmin needs to chase after its leader (or the group spot
 * belonging to the leader).
 */
void pikmin_fsm::chase_leader(mob* m, void* info1, void* info2) {
    m->chase(
        m->group_spot_x, m->group_spot_y,
        &m->following_group->group->group_center_x,
        &m->following_group->group->group_center_y,
        false
    );
    m->set_animation(PIKMIN_ANIM_WALK);
    focus_mob(m, m->following_group);
}


/* ----------------------------------------------------------------------------
 * When a Pikmin starts flailing.
 */
void pikmin_fsm::start_flailing(mob* m, void* info1, void* info2) {
    //If the Pikmin is following a moveable point, let's change it to
    //a static point. This will make the Pikmin continue to move
    //forward into the water in a straight line.
    float final_x, final_y;
    m->get_chase_target(&final_x, &final_y);
    m->chase(
        final_x, final_y,
        NULL, NULL,
        false
    );
    
    remove_from_group(m);
    
    //Let the Pikmin continue to swim into the water for a bit
    //before coming to a stop. Otherwise the Pikmin would stop nearly
    //on the edge of the water, and that just looks bad.
    m->set_timer(1.0f);
}


/* ----------------------------------------------------------------------------
 * When a Pikmin checks if it's no longer meant to be flailing.
 * info1: Points to the hazard that the Pikmin left.
 */
void pikmin_fsm::check_remove_flailing(mob* m, void* info1, void* info2) {
    hazard* h_ptr = (hazard*) info1;
    
    for(size_t s = 0; s < m->statuses.size(); ++s) {
        for(size_t e = 0; e < h_ptr->effects.size(); ++e) {
            if(
                m->statuses[s].type == h_ptr->effects[e] &&
                h_ptr->effects[e]->causes_flailing
            ) {
            
                m->statuses[s].to_delete = true;
                m->fsm.set_state(PIKMIN_STATE_IDLE);
                pikmin_fsm::stand_still(m, NULL, NULL);
                
            }
        }
    }
    
}


/* ----------------------------------------------------------------------------
 * When the Pikmin must move towards the whistle.
 */
void pikmin_fsm::flail_to_whistle(mob* m, void* info1, void* info2) {
    m->chase(
        cur_leader_ptr->x,
        cur_leader_ptr->y,
        NULL, NULL,
        false,
        NULL, true
    );
}


/* ----------------------------------------------------------------------------
 * When a Pikmin starts panicking.
 */
void pikmin_fsm::start_panicking(mob* m, void* info1, void* info2) {
    remove_from_group(m);
    pikmin_fsm::panic_new_chase(m, info1, info2);
}


/* ----------------------------------------------------------------------------
 * When a Pikmin must no longer be idle.
 */
void pikmin_fsm::stop_being_idle(mob* m, void* info1, void* info2) {

}


/* ----------------------------------------------------------------------------
 * When a Pikmin stands still while in a leader's group.
 */
void pikmin_fsm::stop_in_group(mob* m, void* info1, void* info2) {
    m->stop_chasing();
    m->set_animation(PIKMIN_ANIM_IDLE);
}


/* ----------------------------------------------------------------------------
 * When a Pikmin touches a hazard.
 * info1: Pointer to the hazard type.
 */
void pikmin_fsm::touched_hazard(mob* m, void* info1, void* info2) {
    pikmin* p = (pikmin*) m;
    hazard* h = (hazard*) info1;
    for(size_t r = 0; r < p->pik_type->resistances.size(); ++r) {
        if(p->pik_type->resistances[r] == h) return; //Immune!
    }
    
    for(size_t e = 0; e < h->effects.size(); ++e) {
        p->apply_status_effect(h->effects[e], false);
    }
}


/* ----------------------------------------------------------------------------
 * When a Pikmin is sprayed.
 * info1: Pointer to the spray type.
 */
void pikmin_fsm::touched_spray(mob* m, void* info1, void* info2) {
    spray_type* s = (spray_type*) info1;
    
    for(size_t e = 0; e < s->effects.size(); ++e) {
        m->apply_status_effect(s->effects[e], false);
    }
}