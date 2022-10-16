#ifndef TANK_CONTROL_H
#define TANK_CONTROL_H

#include <cstdint>

class TankModel;
class Tank;

class TankControl
{
	TankModel *tankModel;
	Tank *tank;
	uint32_t ctrlBits;

	enum : uint8_t
	{
		FORWARD = 1,
		BACK = 2,
		LEFT = 4,
		RIGHT = 8,
		TURRET_LEFT = 16,
		TURRET_RIGHT = 32,
		CENTER_TURRET = 64,
		//REVERSE_TURN_BIT = 128,
	};

public:

	TankControl(TankModel *tankModel, Tank *tank);

	void stop()
	{
		ctrlBits = 0;
	}

	void forward()
	{
		ctrlBits &= ~BACK;
		ctrlBits |= FORWARD;
	}

	void back()
	{
		ctrlBits &= ~FORWARD;
		ctrlBits |= BACK;
	}

	void right()
	{
		ctrlBits &= ~LEFT;
		ctrlBits |= RIGHT;
	}

	void left()
	{
		ctrlBits &= ~RIGHT;
		ctrlBits |= LEFT;
	}

	void stop_turn()
	{
		ctrlBits &= ~(LEFT | RIGHT);
	}

	void stop_motion()
	{
		ctrlBits &= ~(FORWARD | BACK);
	}

	void turret_left()
	{
		ctrlBits &= ~(TURRET_RIGHT | CENTER_TURRET);
		ctrlBits |= TURRET_LEFT;
	}

	void turret_right()
	{
		ctrlBits &= ~(TURRET_LEFT | CENTER_TURRET);
		ctrlBits |= TURRET_RIGHT;
	}

	void turret_center()
	{
		ctrlBits &= ~(TURRET_RIGHT | TURRET_LEFT);
		ctrlBits |= CENTER_TURRET;
	}

	void turret_stop()
	{
		ctrlBits &= ~(TURRET_RIGHT | TURRET_LEFT | CENTER_TURRET);
	}

	bool is_forward()
	{
		return (ctrlBits & FORWARD) != 0;
	}

	bool is_back()
	{
		return (ctrlBits & BACK) != 0;
	}

	bool is_right()
	{
		return (ctrlBits & RIGHT) != 0;
	}

	bool is_left()
	{
		return (ctrlBits & LEFT) != 0;
	}

	bool is_turn()
	{
		return (ctrlBits & (LEFT | RIGHT)) != 0;
	}

	bool is_turret_left_center()
	{
		return (ctrlBits & (TURRET_LEFT | CENTER_TURRET)) != 0;
	}

	bool is_turret_right_center()
	{
		return (ctrlBits & (TURRET_RIGHT | CENTER_TURRET)) != 0;
	}

	bool is_turret_left_right()
	{
		return (ctrlBits & (TURRET_LEFT | TURRET_RIGHT)) != 0;
	}

	bool is_turret_center()
	{
		return (ctrlBits & CENTER_TURRET) != 0;
	}

	bool is_turret()
	{
		return (ctrlBits & (TURRET_LEFT | TURRET_RIGHT | CENTER_TURRET)) != 0;
	}

	void apply();
};

#endif // TANK_CONTROL_H
