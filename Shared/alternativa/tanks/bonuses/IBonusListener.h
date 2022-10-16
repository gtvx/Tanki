#ifndef IBONUSLISTENER_H
#define IBONUSLISTENER_H

class IBonus;

class IBonusListener
{
public:
	virtual void onBonusDropped(IBonus*) = 0;
	virtual void onTankCollision(IBonus*) = 0;
};

#endif // IBONUSLISTENER_H
