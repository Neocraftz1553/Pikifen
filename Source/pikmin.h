#ifndef PIKMIN_INCLUDED
#define PIKMIN_INCLUDED

class leader;

#include "enemy.h"
#include "leader.h"
#include "party_follower.h"
#include "pikmin_type.h"

class pikmin : public mob{
public:
	pikmin(pikmin_type* type, float x, float y, sector* sec);
	~pikmin();

	pikmin_type* type;
	float hazard_time_left;     //Time it has left until it drowns/chokes/etc.
	enemy* enemy_attacking;     //Enemy it's attacking.

	mob* carrying_mob; //Mob it's carrying.
	size_t carrying_spot;

	unsigned char maturity;     //0: leaf. 1: bud. 2: flower.
	bool burrowed;
};

#endif //ifndef PIKMIN_INCLUDED
