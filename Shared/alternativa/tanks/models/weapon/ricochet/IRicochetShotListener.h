#ifndef IRICOCHETSHOTLISTENER_H
#define IRICOCHETSHOTLISTENER_H

class RicochetShot;
class Vector3;
class Body;

class IRicochetShotListener
{
public:
	virtual void shotHit(RicochetShot*, const Vector3*, const Vector3*, Body*) = 0;
};

#endif // IRICOCHETSHOTLISTENER_H
