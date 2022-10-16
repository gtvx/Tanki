#ifndef SHAFTINDICATOR_H
#define SHAFTINDICATOR_H

class BitmapData;
class ClientObjectWeapon;

class ShaftIndicator
{
public:
	static BitmapData* getIndicator(ClientObjectWeapon *turret);
};

#endif // SHAFTINDICATOR_H
