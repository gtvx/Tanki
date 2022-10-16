#ifndef USERTITLESRENDER_H
#define USERTITLESRENDER_H

class IBattleField;
class TankData;
class Vector3;

class UserTitlesRender
{
public:
	virtual void render() = 0;
	virtual void setBattlefield(IBattleField*) = 0;
	virtual void setLocalData(TankData*) = 0;
	virtual void updateTitle(TankData*, const Vector3*) = 0;
	virtual void configurateTitle(TankData*) = 0;
};

#endif // USERTITLESRENDER_H
