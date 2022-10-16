#ifndef IPLASMASHOTLISTENER_H
#define IPLASMASHOTLISTENER_H

class PlasmaShot;
class Vector3;
class Body;

class IPlasmaShotListener
{
public:
	virtual void plasmaShotDissolved(PlasmaShot*) = 0;
	virtual void plasmaShotHit(PlasmaShot*, const Vector3*, const Vector3*, Body*) = 0;
};

#endif // IPLASMASHOTLISTENER_H
