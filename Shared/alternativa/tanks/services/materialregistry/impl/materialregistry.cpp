#include "materialregistry.h"
#include "alternativa/tanks/engine3d/materialsequenceregistry.h"
#include "alternativa/tanks/vehicles/tanks/skintextureregistry.h"
#include "alternativa/tanks/engine3d/texturematerialregistry.h"


static const int TIMER_INTERVAL = 60;


MaterialRegistry::MaterialRegistry(OSGi *osgi)
{
	(void)osgi;

	this->_textureMaterialRegistry = new TextureMaterialRegistry(TIMER_INTERVAL);
	this->_materialSequenceRegistry = new MaterialSequenceRegistry(TIMER_INTERVAL);
	this->_mapMaterialRegistry = new TextureMaterialRegistry(TIMER_INTERVAL);
	this->_skinTextureRegistry = new SkinTextureRegistry();

	//var dumpService:IDumpService = IDumpService(osgi.getService(IDumpService));
	//if (dumpService != null)
		//dumpService.registerDumper(this);

}

bool MaterialRegistry::useMipMapping()
{
	return this->_textureMaterialRegistry->getUseMipMapping();
}

void MaterialRegistry::setUseMipMapping(bool value)
{
	this->_textureMaterialRegistry->setUseMipMapping(value);
	this->_materialSequenceRegistry->setUseMipMapping(value);
	this->_mapMaterialRegistry->setUseMipMapping(value);
}

ITextureMaterialRegistry* MaterialRegistry::textureMaterialRegistry()
{
	return this->_textureMaterialRegistry;
}

IMaterialSequenceRegistry* MaterialRegistry::materialSequenceRegistry()
{
	return this->_materialSequenceRegistry;
}

ITextureMaterialRegistry* MaterialRegistry::mapMaterialRegistry()
{
	return this->_mapMaterialRegistry;
}

ISkinTextureRegistry *MaterialRegistry::skinTextureRegistry()
{
	return this->_skinTextureRegistry;
}

void MaterialRegistry::clear()
{
	this->_textureMaterialRegistry->clear();
	this->_materialSequenceRegistry->clear();
	this->_mapMaterialRegistry->clear();
	this->_skinTextureRegistry->clear();
}

QString MaterialRegistry::getDumperName()
{
	return "materials";
}

IndexedTextureConstructor *MaterialRegistry::createTextureConstructor()
{
	return nullptr; //new IndexedTextureConstructor();
}

void MaterialRegistry::onTimerIntervalChange(ConsoleVarInt *variable)
{
	(void)variable;
	/*
	switch (variable)
	{
		case this.textureMaterialRegistryInterval:
			this._textureMaterialRegistry.timerInterval = variable.value;
			break;
		case this.mapMaterialRegistryInterval:
			this._mapMaterialRegistry.timerInterval = variable.value;
			break;
		case this.materialSequenceRegistryInterval:
			this._materialSequenceRegistry.timerInterval = variable.value;
			break;
	}
	*/
}

void MaterialRegistry::onTextureResolutionChange(ConsoleVarFloat *variable)
{
	(void)variable;
	//this->_mapMaterialRegistry->setResoluion(variable.value);
}
