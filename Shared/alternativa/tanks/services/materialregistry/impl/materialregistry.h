#ifndef MATERIALREGISTRY_H
#define MATERIALREGISTRY_H

#include "../IMaterialRegistry.h"

class TextureMaterialRegistry;
class MaterialSequenceRegistry;
class SkinTextureRegistry;
class OSGi;
class ConsoleVarInt;
class QString;
class ConsoleVarFloat;


class MaterialRegistry : public IMaterialRegistry
{



	//textureMaterialRegistryInterval:ConsoleVarInt = new ConsoleVarInt("tmr_timer_interval", TIMER_INTERVAL, 30, 1000, onTimerIntervalChange);
	//mapMaterialRegistryInterval:ConsoleVarInt = new ConsoleVarInt("mmr_timer_interval", TIMER_INTERVAL, 30, 1000, onTimerIntervalChange);
	//materialSequenceRegistryInterval:ConsoleVarInt = new ConsoleVarInt("msr_timer_interval", TIMER_INTERVAL, 30, 1000, onTimerIntervalChange);
	//textureResolution:ConsoleVarFloat = new ConsoleVarFloat("tex_res", 5.8, 0, 100, onTextureResolutionChange);
	TextureMaterialRegistry *_textureMaterialRegistry;
	MaterialSequenceRegistry *_materialSequenceRegistry;
	TextureMaterialRegistry *_mapMaterialRegistry;
	SkinTextureRegistry *_skinTextureRegistry;


public:
	MaterialRegistry(OSGi *osgi);

	bool useMipMapping() override;
	void setUseMipMapping(bool value) override;
	ITextureMaterialRegistry* textureMaterialRegistry() override;
	IMaterialSequenceRegistry* materialSequenceRegistry() override;
	ITextureMaterialRegistry* mapMaterialRegistry() override;
	ISkinTextureRegistry* skinTextureRegistry() override;

	void clear() override;

	/*
	QString dump(params:Vector.<String>)
	{
		return ((this._textureMaterialRegistry.getDump() + this._materialSequenceRegistry.getDump()) + this._mapMaterialRegistry.getDump());
	}
	*/

	QString getDumperName();

	IndexedTextureConstructor* createTextureConstructor();

	void onTimerIntervalChange(ConsoleVarInt *variable);

private:
	void onTextureResolutionChange(ConsoleVarFloat *variable);
};

#endif // MATERIALREGISTRY_H
