#include "battlebonusstate.h"


BattleBonusState::BattleBonusState()
{
	this->pivotZ = 0.;
	this->angleX = 0.;
	this->angleZ = 0.;
}

void BattleBonusState::interpolate(BattleBonusState *s1, BattleBonusState *s2, double t)
{
	this->pivotZ = s1->pivotZ + (t * (s2->pivotZ - s1->pivotZ));
	this->angleX = s1->angleX + (t * (s2->angleX - s1->angleX));
	this->angleZ = s1->angleZ + (t * (s2->angleZ - s1->angleZ));
}

void BattleBonusState::copy(BattleBonusState *src)
{
	this->pivotZ = src->pivotZ;
	this->angleX = src->angleX;
	this->angleZ = src->angleZ;
}
