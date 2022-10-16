#ifndef ISPECIALEFFECT_H
#define ISPECIALEFFECT_H

class GameCamera;
class ClientObjectUser;


class ISpecialEffect
{
public:
	virtual bool play(int, GameCamera*) = 0;
	virtual void destroy() = 0;
	virtual void kill() = 0;
	virtual ClientObjectUser* getOwner() = 0;
};

#endif // ISPECIALEFFECT_H
