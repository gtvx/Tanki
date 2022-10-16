#include "itempropertyfromstring.h"
#include <QString>


ItemProperty ItemPropertyFromString(const QString &src)
{
	if (src == "damage") {
		return ItemProperty::DAMAGE;
	} else if (src == "damage_per_second") {
		return ItemProperty::DAMAGE_PER_SECOND;
	} else if (src == "critical_chance") {
		return ItemProperty::CRITICAL_CHANCE;
	} else if (src == "heating_time") {
		return ItemProperty::HEATING_TIME;
	} else if (src == "aiming_error") {
		return ItemProperty::AIMING_ERROR;
	} else if (src == "cone_angle") {
		return ItemProperty::CONE_ANGLE;
	} else if (src == "shot_area") {
		return ItemProperty::SHOT_AREA;
	} else if (src == "shot_frequency") {
		return ItemProperty::SHOT_FREQUENCY;
	} else if (src == "shot_range") {
		return ItemProperty::SHOT_RANGE;
	} else if (src == "turn_speed") {
		return ItemProperty::TURN_SPEED;
	} else if (src == "mech_resistance") {
		return ItemProperty::MECH_RESISTANCE;
	} else if (src == "plasma_resistance") {
		return ItemProperty::PLASMA_RESISTANCE;
	} else if (src == "rail_resistance") {
		return ItemProperty::RAIL_RESISTANCE;
	} else if (src == "terminator_resistance") {
		return ItemProperty::TERMINATOR_RESISTANCE;
	} else if (src == "mine_resistance") {
		return ItemProperty::MINE_RESISTANCE;
	} else if (src == "vampire_resistance") {
		return ItemProperty::VAMPIRE_RESISTANCE;
	} else if (src == "armor") {
		return ItemProperty::ARMOR;
	} else if (src == "turret_turn_speed") {
		return ItemProperty::TURRET_TURN_SPEED;
	} else if (src == "fire_resistance") {
		return ItemProperty::FIRE_RESISTANCE;
	} else if (src == "thunder_resistance") {
		return ItemProperty::THUNDER_RESISTANCE;
	} else if (src == "freeze_resistance") {
		return ItemProperty::FREEZE_RESISTANCE;
	} else if (src == "ricochet_resistance") {
		return ItemProperty::RICOCHET_RESISTANCE;
	} else if (src == "healing_radius") {
		return ItemProperty::HEALING_RADUIS;
	} else if (src == "heal_rate") {
		return ItemProperty::HEAL_RATE;
	} else if (src == "vampire_rate") {
		return ItemProperty::VAMPIRE_RATE;
	} else if (src == "speed") {
		return ItemProperty::SPEED;
	} else if (src == "shaft_damage") {
		return ItemProperty::SHAFT_DAMAGE;
	} else if (src == "shaft_shot_frequency") {
		return ItemProperty::SHAFT_FIRE_RATE;
	} else if (src == "shaft_resistance") {
		return ItemProperty::SHAFT_RESISTANCE;
	} else {
		return ItemProperty::SHAFT_RESISTANCE;
	}
}
