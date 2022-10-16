#ifndef LEVELUPEFFECT_H
#define LEVELUPEFFECT_H

class Tank;
class Object3D;
class BattlefieldModel;

class LevelUpEffect
{
public:
	static void createEffect(BattlefieldModel *battlefield, Tank *tank, int rang);
};

#endif // LEVELUPEFFECT_H
