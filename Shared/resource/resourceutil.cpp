#include "resourceutil.h"
#include "resource/tanks/tankresourceloader.h"
#include "resource/tanks/tankresourcelist.h"
#include "resource/images/imageresourceloader.h"
#include "resource/images/imageresourcelist.h"
#include "images/imageresource.h"
#include "images/objectbitmapdata.h"
#include "alternativa/resource/stubbitmapdata.h"
#include "tanks/tankresource.h"
#include "resourcepath.h"
#include "_global.h"
#include "file.h"
#include <QHash>
#include <QString>
#include <QDebug>


static TankResourceLoader *loaderModels;

#ifdef GRAPHICS
static ImageResourceLoader *loaderImages;
#endif

bool ResourceUtil::isInBattle = false;

static QHash<QString, std::shared_ptr<BitmapData>> localImages;
static QHash<QString, TankResource*> localModels;




#ifdef GRAPHICS
ImageResource* ResourceUtil::getResourceImage(const QString &key)
{
	_function_name("ResourceUtil::getResourceImage");

	ImageResource *resource;

	if (isInBattle == false)
		resource = loaderImages->list->getImage(key);
	else
		resource = loaderImages->inbattleList->getImage(key);

	if (resource == nullptr)
	{
		static ImageResource *resource_null;
		if (resource_null == nullptr)
		{
			 std::shared_ptr<ObjectBitmapData> objectBitmapData = std::make_shared<ObjectBitmapData>();
			 objectBitmapData->data = std::make_shared<StubBitmapData>(0xFFFF0000);
			 resource_null = new ImageResource(objectBitmapData, "", false, nullptr, "");
		}

		resource = resource_null;

		qDebug() << "error getResourceImage" << key;
	}

	resource->load();

	return resource;
}
#endif


TankResource* ResourceUtil::getResourceModel(const QString &key)
{
	_function_name("ResourceUtil::getResourceModel");
	TankResource *resource = loaderModels->list->getModel(key);

	if (resource == nullptr)
	{
		qDebug() << "error getResourceModel" << key;
		return nullptr;
	}

	if (resource->loaded == true)
	{
		return resource;
	}

	if (resource->loaded == false && resource->error == false)
	{
		loaderModels->loadModel(resource);
	}

	if (resource->error == true)
	{
		qDebug() << "error getResourceModel" << key;
		return nullptr;
	}

	return resource;
}




TankResource* ResourceUtil::getResourceModelLocal(const QString &key)
{
	_function_name("ResourceUtil::getResourceModelLocal");

	TankResource *resource = localModels.value(key);

	if (resource == nullptr)
	{
		std::shared_ptr<Mesh> mesh;
		resource = new TankResource(mesh, key, key);
		resource->loaded = false;
		resource->error = false;
		TankResourceLoader::loadModelLocal(resource);
		localModels.insert(key, resource);
	}

	if (resource->loaded == true)
	{
		return resource;
	}

	//if (resource->error == true)
	{
		qDebug() << "error getResourceModelLocal" << key;
		return nullptr;
	}
}



#ifdef GRAPHICS
std::shared_ptr<BitmapData> ResourceUtil::getResourceImageLocal(const QString &filename)
{
	_function_name("ResourceUtil::getResourceImageLocal");

	std::shared_ptr<BitmapData> v = localImages.value(filename);
	if (v != nullptr)
		return v;

	QByteArray data;

	bool c1 = File::read(ResourcePath::get() + filename, data);
	std::shared_ptr<BitmapData> image = std::make_shared<BitmapData>();
	bool c2 = image->loadFromData(data);

	if (c1 && c2)
	{
		localImages.insert(filename, image);
		return image;
	}
	else
	{
		qDebug() << "error ResourceUtil::getResourceImageLocal" << filename;
		return nullptr;
	}
}


std::shared_ptr<BitmapData> ResourceUtil::getResourceImageLocal(const char *filename)
{
	return ResourceUtil::getResourceImageLocal(QString(filename));
}

#endif


void ResourceUtil::clear()
{

	loaderModels->clear();
	localModels.clear();

#ifdef GRAPHICS
	loaderImages->clear();
	localImages.clear();
#endif
}


bool ResourceUtil::isDataPresent(TankParts *parts)
{
	_function_name("ResourceUtil::isDataPresent");

	(void)parts;
	/*
	//var s:String;
	//var toCheck:Vector.<String> = new Vector.<String>();
	toCheck.push((parts.hullObjectId + "_lightmap"));
	toCheck.push((parts.hullObjectId + "_details"));
	toCheck.push((parts.turretObjectId + "_lightmap"));
	toCheck.push((parts.turretObjectId + "_details"));
	toCheck.push(parts.coloringObjectId);
	for each (s in toCheck)
	{
		if ((!(loaderImages.inbattleList.isLoaded(s))))
		{
			return false;
		}
	}
	toCheck = null;
	*/
	return true;
}

void ResourceUtil::loadResource()
{
	_function_name("ResourceUtil::loadResource");

	/*
	sfxMap = new Dictionary();
	sfxMap["railgun"] = "shot, chargingPart1, chargingPart2, chargingPart3";
	sfxMap["terminator"] = "shot, chargingPart1, chargingPart2, chargingPart3";
	sfxMap["smoky"] = "shot, explosion";
	sfxMap["hammer"] = "shot, explosion";
	sfxMap["smokyxt"] = "shot, explosion";
	sfxMap["flamethrower"] = "fire";
	sfxMap["twins"] = "explosionTexture, plasmaTexture, shotTexture";
	sfxMap["isida"] = "damagingRayId, damagingTargetBallId, damagingWeaponBallId, healingRayId, healingTargetBallId, healingWeaponBallId, idleWeaponBallId";
	sfxMap["thunder"] = "explosionResourceId, shotResourceId";
	sfxMap["frezee"] = "particleTextureResourceId, planeTextureResourceId";
	sfxMap["ricochet"] = "bumpFlashTextureId, explosionTextureId, shotFlashTextureId, shotTextureId, tailTrailTextureId";
	sfxMap["shaft"] = "explosionTexture, shot, trail";
	loading = new Vector.<String>();
	loaderSounds = new SoundResourceLoader(("resourceSound.json?rand=" + Math.random()));
	*/

	loaderModels = new TankResourceLoader("resourceModels.json");
#ifdef GRAPHICS
	loaderImages = new ImageResourceLoader("resourceImages.json");
#endif
}
