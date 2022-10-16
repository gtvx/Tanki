#ifndef ITANK_H
#define ITANK_H

class TankData;
class Vector3;
class ClientObject;
class ClientObjectUser;

class ITank
{
public:
	virtual TankData* getTankData(ClientObjectUser*) = 0;
	virtual void update(TankData*, int, int, double, double, const Vector3*) = 0;
	virtual void disableUserControls(bool) = 0;
	virtual void enableUserControls() = 0;
	virtual void stop(TankData*) = 0;
	virtual void resetIdleTimer(bool) = 0;
	virtual bool getUserControlsEnabled() = 0;
	virtual void readLocalTankPosition(Vector3*) = 0;
	virtual void effectStarted(ClientObjectUser*, int, int) = 0;
	virtual void effectStopped(ClientObjectUser*, int) = 0;
};

#endif // ITANK_H
