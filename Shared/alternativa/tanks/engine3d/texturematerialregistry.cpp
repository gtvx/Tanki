#include "texturematerialregistry.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "alternativa/engine3d/materials/materialentry.h"
#include "texturematerialfactory.h"
#include "resource/images/MultiframeResourceData.h"
#include "resource/images/imageresource.h"
#include "resource/images/objectbitmapdata.h"
#include "flash/display/bitmapdata.h"
#include "alternativa/tanks/materials/animatedpaintmaterial.h"
#include "alternativa/tanks/materials/paintmaterial.h"



TextureMaterialRegistry::TextureMaterialRegistry(int value)
{
	(void)value;
	mipMappingEnabled = true;
}

void TextureMaterialRegistry::setTimerInterval(int value)
{
	(void)value;
}

AnimatedPaintMaterial *TextureMaterialRegistry::getAnimatedPaint(ImageResource *imageResource, std::shared_ptr<BitmapData> &lightmap, std::shared_ptr<BitmapData> &details, const QString &objId)
{
	this->materialStat.requestCount++;
	QString name = imageResource->id + objId;

	if (this->entryForTextureString.contains(name))
	{
		MaterialEntry *entry = this->entryForTextureString.value(name).get();
		entry->referenceCount++;

		if (!entry->material->isType(AnimatedPaintMaterial::TYPE))
		{
			throw 3454343;
		}

		return (AnimatedPaintMaterial*)entry->material.get();
	}

	MultiframeResourceData *multiframeData = imageResource->multiframeData;

	int width = int(imageResource->bitmapData->data->width() / multiframeData->widthFrame);
	int height = int(imageResource->bitmapData->data->height() / multiframeData->heigthFrame);

	std::shared_ptr<AnimatedPaintMaterial> animatedPaintMaterial =
			new_AnimatedPaintMaterial(imageResource->bitmapData->data,
									  lightmap,
									  details,
									  width,
									  height,
									  multiframeData->fps, multiframeData->numFrames, ((!(!(this->mipMappingEnabled))) ? MipMapping::OBJECT_DISTANCE : MipMapping::NONE));

	MaterialEntry *entry = this->createPaintMaterialEntry(name, animatedPaintMaterial);
	entry->referenceCount++;
	this->materials.append(animatedPaintMaterial);
	this->materialStat.createCount++;
	return animatedPaintMaterial.get();
}

PaintMaterial *TextureMaterialRegistry::getPaint1(ImageResource *imageResource,std::shared_ptr<BitmapData> &lightmap, std::shared_ptr<BitmapData> &details, const QString &objId)
{
	this->materialStat.requestCount++;

	QString name = imageResource->id + objId;
	if (this->entryForTextureString.contains(name))
	{
		MaterialEntry *entry = this->entryForTextureString.value(name).get();
		entry->referenceCount++;

		if (!entry->material->isType(PaintMaterial::TYPE))
		{
			throw 3454343;
		}

		return (PaintMaterial*)entry->material.get();
	}


	std::shared_ptr<PaintMaterial> maintMaterial = new_PaintMaterial(imageResource->bitmapData->data,
															   lightmap,
															   details,
															   ((!(!(this->mipMappingEnabled))) ? MipMapping::OBJECT_DISTANCE : MipMapping::NONE));

	MaterialEntry *entry = this->createPaintMaterialEntry(name, maintMaterial);
	entry->referenceCount++;
	this->materials.append(maintMaterial);
	this->materialStat.createCount++;
	return maintMaterial.get();
}

MaterialEntry *TextureMaterialRegistry::createPaintMaterialEntry(const QString &param1, std::shared_ptr<TextureMaterial> param2)
{
	std::shared_ptr<MaterialEntry> entry = std::make_shared<MaterialEntry>(param1, param2);
	this->entryForTextureString.insert(param1, entry);
	this->entryForMaterial.insert(param2.get(), entry);
	return entry.get();
}

void TextureMaterialRegistry::setResoluion(double value)
{
	for (std::shared_ptr<TextureMaterial> &material : this->materials)
	{
		material->resolution = value;
	}
}

bool TextureMaterialRegistry::getUseMipMapping()
{
	return this->_useMipMapping;
}

void TextureMaterialRegistry::setUseMipMapping(bool value)
{
	this->_useMipMapping = value;
}

TextureMaterial* TextureMaterialRegistry::getMaterial(MaterialType materialType, std::shared_ptr<BitmapData> &bitmapData, double mipMapResolution, bool)
{
	(void)materialType;
	(void)mipMapResolution;

	if (bitmapData == nullptr)
	{
		throw 1342435;
		//throw (new ArgumentError("Texture is null"));
	}
	this->materialStat.requestCount++;
	std::shared_ptr<MaterialEntry> entry = this->getOrCreateEntry(bitmapData);
	entry->referenceCount++;
	return entry->material.get();
}

std::shared_ptr<MaterialEntry> TextureMaterialRegistry::getOrCreateEntry(std::shared_ptr<BitmapData> &bitmapData)
{
	std::shared_ptr<MaterialEntry> entry = this->entryForTexture[bitmapData];
	if (entry == nullptr)
	{
		std::shared_ptr<TextureMaterial> material = this->createMaterial(bitmapData);
		entry = this->createMaterialEntry(bitmapData, material);
	}

	return entry;
}

std::shared_ptr<TextureMaterial> TextureMaterialRegistry::createMaterial(std::shared_ptr<BitmapData> &bitmapData)
{
	std::shared_ptr<BitmapData> _bitmapData = this->getTexture(bitmapData);
	std::shared_ptr<TextureMaterial> textureMaterial = TextureMaterialFactory::createTextureMaterial(_bitmapData, this->mipMappingEnabled);
	this->materials.append(textureMaterial);
	this->materialStat.createCount++;
	return textureMaterial;
}

std::shared_ptr<MaterialEntry> TextureMaterialRegistry::createMaterialEntry(std::shared_ptr<BitmapData> bitmapData,
																			std::shared_ptr<TextureMaterial> &textureMaterial)
{
	std::shared_ptr<MaterialEntry> entry = std::make_shared<MaterialEntry>(bitmapData, std::dynamic_pointer_cast<TextureMaterial>(textureMaterial));
	this->entryForTexture.insert(bitmapData, entry);
	this->entryForMaterial.insert(textureMaterial.get(), entry);
	return entry;
}

std::shared_ptr<BitmapData> TextureMaterialRegistry::getTexture(std::shared_ptr<BitmapData> &bitmapData)
{
	return bitmapData;
}

QString TextureMaterialRegistry::getDump()
{
	/*
	var totalBitmapSize:int;
	var numMaterials:int;
	var numMaps:int;
	var s:String = "=== TextureMaterialRegistry ===\n";
	s = (s + (((((((("Total mipmaps: " + numMaps) + "\n") + "Total size: ") + totalBitmapSize) + "\n") + "Scheduled materials: ") + 0) + "\n"));
	return (s);
	*/
	return "";
}

void TextureMaterialRegistry::disposeMaterial(TextureMaterial *material)
{
	if (material == nullptr)
	{
		return;
	}

	MaterialEntry *entry = this->entryForMaterial.value(material).get();
	if (entry != nullptr)
	{
		this->materialStat.releaseCount++;
		entry->referenceCount--;
		if (entry->referenceCount == 0)
		{
			this->removeMaterialEntry(entry);
		}
	}
}

void TextureMaterialRegistry::removeMaterialEntry(MaterialEntry *materialEntry)
{
	this->materialStat.destroyCount++;
	std::shared_ptr<TextureMaterial> material = materialEntry->material;

	this->entryForTexture.clear();

	//if (param1.keyData in this->entryForTexture)
		//delete this->entryForTexture[param1->keyData];

	this->entryForMaterial.remove(material.get());

	materialEntry->material = nullptr;

	this->materials.removeOne(material);
	//int i = this->materials.indexOf(material);
	//this->materials->splice(i, 1);

	material->dispose();
	material = nullptr;
}

void TextureMaterialRegistry::forEachMaterial()
{
	for (std::shared_ptr<TextureMaterial> material : this->materials)
	{
		_clearTexture(material.get());
	}
}

void TextureMaterialRegistry::clear()
{
	this->forEachMaterial();
	this->materials.clear();
	this->entryForTexture.clear();
	this->entryForMaterial.clear();
	this->materialStat.clear();
}

void TextureMaterialRegistry::_clearTexture(TextureMaterial *material)
{
	material->setTexture(nullptr);
}
