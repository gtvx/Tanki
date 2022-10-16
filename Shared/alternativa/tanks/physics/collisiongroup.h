#ifndef COLLISIONGROUP_H
#define COLLISIONGROUP_H

class CollisionGroup
{
public:
	enum : int
	{
		TANK = 1,
		ACTIVE_TRACK = 2,
		INACTIVE_TRACK = 4,
		WEAPON = 16,
		CAMERA = 32,
		STATIC = 64,
		BONUS_WITH_STATIC = 128,
		BONUS_WITH_TANK = 0x0100,
	};
};

#endif // COLLISIONGROUP_H
