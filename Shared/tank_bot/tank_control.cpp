#include "tank_control.h"
#include "alternativa/tanks/models/tank/tankmodel.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "_global.h"


TankControl::TankControl(TankModel *tankModel, Tank *tank)
{
	this->tankModel = tankModel;
	this->tank = tank;
	this->ctrlBits = 0;
}


void TankControl::apply()
{
	_function_name("TankControl::apply");

	uint32_t value = ctrlBits | tankModel->controlBits;

	uint32_t ctrlBits_current = tank->tankData->ctrlBits;

	if (ctrlBits_current == value)
		return;

	if ((value & TURRET_LEFT) != 0) {
		if (tank->turretTurnSpeed < 0.0)
			value &= ~(TURRET_LEFT);
	}

	if ((value & TURRET_RIGHT) != 0) {
		if (tank->turretTurnSpeed > 0.0)
			value &= ~(TURRET_RIGHT);
	}

	if (ctrlBits_current == value)
		return;

#if 0
	QString text;

	if (tank_control_is_forward())
		text += "forward  ";

	if (tank_control_is_back())
		text += "back  ";

	if (tank_control_is_right())
		text += "right  ";

	if (tank_control_is_left())
		text += "left  ";

	if (tank_control_is_turn())
		text += "turn  ";

	if (tank_control_is_turret_left_center())
		text += "left_center  ";

	if (tank_control_is_turret_right_center())
		text += "right_center  ";

	if (tank_control_is_turret_left_right())
		text += "left_right  ";

	if (tank_control_is_turret_center())
		text += "turret_center  ";

	if (tank_control_is_turret())
		text += "turret  ";

	qDebug() << text;
#endif

	tankModel->applyControlState(tank->tankData, value, true);
}
