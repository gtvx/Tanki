#ifndef IRAILGUNSFXMODEL_H
#define IRAILGUNSFXMODEL_H

class RailgunShootSFXData;
class ClientObject;
class IGraphicEffect;
class ISound3DEffect;
class Vector3;
class Object3D;
class ClientObjectWeapon;
class ClientObjectUser;


class IRailgunSFXModel
{
public:
	virtual IGraphicEffect* createGraphicShotEffect(ClientObjectWeapon*, const Vector3*, const Vector3*) = 0;
	virtual IGraphicEffect* createChargeEffect(ClientObjectWeapon*, ClientObjectUser*, RailgunShootSFXData*, const Vector3*, Object3D*, int) = 0;
	virtual ISound3DEffect* createSoundShotEffect(ClientObject*, ClientObject*, const Vector3*) = 0;
};

#endif // IRAILGUNSFXMODEL_H
