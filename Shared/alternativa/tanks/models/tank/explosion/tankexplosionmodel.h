#ifndef TANKEXPLOSIONMODEL_H
#define TANKEXPLOSIONMODEL_H

class ClientObjectHull;
class QString;
class TankData;

class TankExplosionModel
{
public:
	static bool initObject(ClientObjectHull *clientObject, const QString &explosionTextureId, const QString &shockWaveTextureId, const QString &smokeTextureId);
	static void objectUnloaded(ClientObjectHull *object);
	static void createExplosionEffects(ClientObjectHull *clientObject, TankData *tankData);
};

#endif // TANKEXPLOSIONMODEL_H
