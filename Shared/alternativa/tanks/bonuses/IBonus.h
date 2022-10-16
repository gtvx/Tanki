#ifndef IBONUS_H
#define IBONUS_H

class QString;
class Vector3;
class PhysicsScene;
class Scene3DContainer;
class IBonusListener;

class IBonus
{
public:
	virtual QString getBonusId() = 0;
	virtual void attach(const Vector3*, PhysicsScene*, Scene3DContainer*, IBonusListener*) = 0;
	virtual bool update(int, int, double) = 0;
	virtual bool isFalling() = 0;
	//virtual void setRestingState(double, double, double) = 0;
	virtual void setTakenState() = 0;
	virtual void setRemovedState() = 0;
	virtual void destroy() = 0;
	virtual void readBonusPosition(Vector3*) = 0;
	virtual ~IBonus() {}
};

#endif // IBONUS_H
