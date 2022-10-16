#ifndef IFLAMETHROWER_H
#define IFLAMETHROWER_H

class FlamethrowerData;
class ClientObjectWeapon;

class IFlamethrower
{
public:
	virtual FlamethrowerData* getFlameData(ClientObjectWeapon*) = 0;
};

#endif // IFLAMETHROWER_H
