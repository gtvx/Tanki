#ifndef BOT_SHOT_H
#define BOT_SHOT_H

#include <stdint.h>

struct Tank;
struct BattlefieldModel;
struct BattleController;

namespace TANKS
{
	struct StructTank;
}


class BotShot
{
public:
	enum class BodyTurn
	{
		NO,
		LEFT,
		RIGHT,
	};

protected:
	BattleController *battleController;
	BattlefieldModel *battlefield;
	Tank *local_tank;
	Tank *tank_current;
	TANKS::StructTank *struct_tank_current;
	BodyTurn body_turn;
	bool tanks_check;

public:

	virtual ~BotShot() {}
	virtual bool init() = 0;
	virtual void processing(uint32_t time) = 0;
	virtual void deleteTank(Tank*) = 0;
	bool isTankFocus() { return tank_current != nullptr; }
	Tank* getTankFocus() { return tank_current; }
	void clearFocus() { tank_current = nullptr; }
	BodyTurn getBodyTurn() { return body_turn; }
	bool tanksCheck() { return tanks_check; }
	void setTankCurrent(TANKS::StructTank *s_tank);
	void distanceInit();
	TANKS::StructTank* getNearTank(uint32_t distance_min);
};


#endif // BOT_SHOT_H
