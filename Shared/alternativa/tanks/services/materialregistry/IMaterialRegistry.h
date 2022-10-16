#ifndef IMATERIALREGISTRY_H
#define IMATERIALREGISTRY_H

class ITextureMaterialRegistry;
class IMaterialSequenceRegistry;
class ISkinTextureRegistry;
class IndexedTextureConstructor;


class IMaterialRegistry
{
public:
	virtual bool useMipMapping() = 0;
	virtual void setUseMipMapping(bool) = 0;
	virtual ITextureMaterialRegistry* textureMaterialRegistry() = 0;
	virtual IMaterialSequenceRegistry* materialSequenceRegistry() = 0;
	virtual ISkinTextureRegistry* skinTextureRegistry() = 0;
	virtual ITextureMaterialRegistry* mapMaterialRegistry() = 0;
	virtual void clear() = 0;
	virtual IndexedTextureConstructor *createTextureConstructor() = 0;
};

#endif // IMATERIALREGISTRY_H
