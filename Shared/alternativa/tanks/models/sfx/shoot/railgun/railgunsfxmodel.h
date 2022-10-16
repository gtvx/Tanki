#ifndef RAILGUNSFXMODEL_H
#define RAILGUNSFXMODEL_H

#include "IRailgunSFXModel.h"
#include <memory>
#include <qglobal.h>

class ClientObject;
class QString;
class RailgunShootSFXData;
class ClientObjectWeapon;
class IGraphicEffect;
class ClientObjectUser;
class Vector3;
class Object3D;
class ISound3DEffect;
class BitmapData;
class BattlefieldModel;
class IBattleField;
class TanksTextureMaterial;
class Material;
class AnimatedLightEffect;
class IMaterialRegistry;
class TextureAnimation;
class ObjectPoolService;
class ColorTransform;



class RailgunSFXModel : public IRailgunSFXModel
{
	IMaterialRegistry *materialRegistry;
	ObjectPoolService *objectPoolService;
	ColorTransform *colorTransform;
	//private var matrix:Matrix = new Matrix();
	IBattleField *battlefield;
	BattlefieldModel *bfModel;
	int numLoadedObjects;
	std::shared_ptr<TanksTextureMaterial> trailMaterial;
	//private var shotSound:Sound;
	QVector<std::shared_ptr<Material>> *chargeMaterials;
	TextureAnimation *charingData;
	AnimatedLightEffect *chargeLightEffect;

public:
	RailgunSFXModel();
	virtual ~RailgunSFXModel();

	void initObject(RailgunShootSFXData *SFXData,
					const QString &chargingPart1,
					const QString &chargingPart2,
					const QString &chargingPart3,
					const QString &railImageId,
					const QString &shotSoundId);

	IGraphicEffect* createGraphicShotEffect(ClientObjectWeapon *clientObject, const Vector3 *startPosition, const Vector3 *hitPosition)  override;

	IGraphicEffect* createChargeEffect(ClientObjectWeapon *clientObject,
									   ClientObjectUser *user,
									   RailgunShootSFXData *sfxData,
									   const Vector3 *localMuzzlePosition,
									   Object3D *turret,
									   int chargingTime) override;

	void createChargeLightEffect(const Vector3 *param1, Object3D *param2, int param3, ClientObjectWeapon *turretObject);

private:
	void createRailLightEffect(ClientObjectWeapon *clientObject, const Vector3 *param1, const Vector3 *param2, ClientObjectWeapon *turretObject);
	void createShotLightEffect(const Vector3 *param1, ClientObjectWeapon *turretObject);
	ISound3DEffect* createSoundShotEffect(ClientObject *clientObject, ClientObject *effectOwner, const Vector3 *startGlobalPos) override;

public:
	void objectLoaded(RailgunShootSFXData *SFXData);
	void objectUnloaded(ClientObjectWeapon *clientObject);

private:
	void createChargeMaterials(const QString &chargingPart1, const QString &chargingPart2, const QString &chargingPart3);
	std::shared_ptr<BitmapData> createChargeTexture(BitmapData *chargingPart1, BitmapData *chargingPart2, BitmapData *chargingPart3);
	void drawPart1(BitmapData *sourceBitmapData, BitmapData *destBitmapData, int frame, int frameSize);
	void drawPart2(BitmapData *sourceBitmapData, BitmapData *destBitmapData, int frame, int frameSize);
	void drawPart3(BitmapData *sourceBitmapData, BitmapData *destBitmapData, int frame, int frameSize);
};

#endif // RAILGUNSFXMODEL_H
