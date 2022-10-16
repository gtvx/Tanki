#include "bonuscommonmodel.h"
#include "BonusCommonData.h"
#include "alternativa/object/clientobjectbonus.h"
#include "parabonus.h"
#include "resource/resourceutil.h"
#include "resource/tanks/tankresource.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/engine3d/core/object3d_clone.h"
#include "resource/images/imageresource.h"
#include "resource/images/objectbitmapdata.h"
#include "alternativa/resource/stubbitmapdata.h"
#include "service.h"
#include "alternativa/tanks/services/materialregistry/IMaterialRegistry.h"
#include "alternativa/tanks/engine3d/ITextureMaterialRegistry.h"
#include "alternativa/tanks/engine3d/MaterialType.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "_global.h"


static double MIPMAP_RESOLUTION = 4;


void BonusCommonModel::initObject(ClientObjectBonus *clientObject, const QString &boxResourceId, const QString &cordResourceId, int disappearingTime, const QString &parachuteInnerResourceId, const QString &parachuteResourceId)
{
	(void)cordResourceId;
	(void)parachuteInnerResourceId;
	(void)parachuteResourceId;

	if (clientObject->bonusData != nullptr)
		return;



	IResourceService *resourceService = nullptr; //(Main.osgi.getService(IResourceService) as IResourceService);
	BonusCommonData *bonusData = new BonusCommonData();
	bonusData->boxMesh = getMeshFromResource(resourceService, boxResourceId);
	bonusData->parachuteMesh = getMeshFromResource(resourceService, parachuteResourceId, true);
	//bonusData->parachuteInnerMesh = this->getMeshFromResource(resourceService, parachuteInnerResourceId);

#ifdef GRAPHICS
	IMaterialRegistry *materialRegistry = Service::getIMaterialRegistry();
	std::shared_ptr<BitmapData> cordResource = ResourceUtil::getResourceImage(cordResourceId)->bitmapData->data;
	bonusData->cordMaterial = materialRegistry->textureMaterialRegistry()->getMaterial(MaterialType::EFFECT, cordResource, MIPMAP_RESOLUTION);
#else
	bonusData->cordMaterial = nullptr;
#endif

	bonusData->duration = disappearingTime * 1000;

	clientObject->bonusData = bonusData;
}


IBonus* BonusCommonModel::getBonus(ClientObjectBonus *clientObject, const QString &bonusId, int livingTime, bool isFalling)
{
	BonusCommonData *data = clientObject->bonusData;
	ParaBonus *bonus = ParaBonus::create(data);
	bonus->init(bonusId, data->duration - livingTime, isFalling);
	return bonus;
}

std::shared_ptr<Mesh> BonusCommonModel::getMeshFromResource(IResourceService *resourceService, const QString &resourceId, bool lacklisted)
{
	(void)resourceService;
	(void)lacklisted;

	TankResource *tankResource = ResourceUtil::getResourceModel(resourceId);
	if (tankResource == nullptr)
		throw 14134;


#ifdef GRAPHICS


	ImageResource* imageResource = ResourceUtil::getResourceImage(resourceId);
	if (imageResource == nullptr) {
		qDebug("error 425442341");
		return nullptr;
	}

	std::shared_ptr<BitmapData> texture = imageResource->bitmapData->data;

	if (texture == nullptr)
		texture = std::make_shared<StubBitmapData>(0xFF00);


	IMaterialRegistry *materialRegistry = Service::getIMaterialRegistry();

	TextureMaterial *material = materialRegistry->textureMaterialRegistry()->getMaterial(MaterialType::EFFECT, texture, MIPMAP_RESOLUTION);
	/*
	if (blacklisted)
	{
		BattlefieldModel *bfModel = BattlefieldModel(Main.osgi.getService(IBattleField));
		if (bfModel.blacklist.indexOf(material.getTextureResource()) == -1)
		{
			bfModel.blacklist.push(material.getTextureResource());
		}
	}
	*/



	std::shared_ptr<Mesh> mesh = Mesh_clone(tankResource->mesh);
	mesh->setMaterialToAllFaces(material);

#else
	(void)MIPMAP_RESOLUTION;
	std::shared_ptr<Mesh> mesh = Mesh_clone(tankResource->mesh);
#endif

	return mesh;
}

