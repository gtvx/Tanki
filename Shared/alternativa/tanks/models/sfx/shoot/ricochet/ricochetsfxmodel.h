#ifndef RICOCHETSFXMODEL_H
#define RICOCHETSFXMODEL_H

#include "IRicochetSFXModel.h"

class ClientObjectWeapon;
class QString;
class RicochetSFXData;


class RicochetSFXModel : public IRicochetSFXModel
{
public:
	RicochetSFXModel();
	virtual ~RicochetSFXModel();

	void initObject(ClientObjectWeapon *clientObject,
					const QString &bumpFlashTextureId,
					const QString &explosionSoundId,
					const QString &explosionTextureId,
					const QString &ricochetSoundId,
					const QString &shotFlashTextureId,
					const QString &shotSoundId,
					const QString &shotTextureId,
					const QString &tailTrailTextureId);

	void objectLoaded(ClientObjectWeapon *clientObject);
	void objectUnloaded(ClientObjectWeapon *clientObject);
	RicochetSFXData* getSfxData(ClientObjectWeapon *clientObject) override;
};

#endif // RICOCHETSFXMODEL_H
