#ifndef BATTLEBONUSSTATE_H
#define BATTLEBONUSSTATE_H


class BattleBonusState
{
public:

	double pivotZ;
	double angleX;
	double angleZ;

	BattleBonusState();

	void interpolate(BattleBonusState *s1, BattleBonusState *s2, double t);
	void copy(BattleBonusState *src);
};

#endif // BATTLEBONUSSTATE_H
