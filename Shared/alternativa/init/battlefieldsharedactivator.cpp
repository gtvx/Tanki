#include "battlefieldsharedactivator.h"
#include "alternativa/tanks/services/materialregistry/impl/materialregistry.h"
#include "service.h"
#include "_global.h"

BattlefieldSharedActivator::BattlefieldSharedActivator()
{

}

void BattlefieldSharedActivator::start(OSGi *osgi)
{
	(void)osgi;

#ifdef GRAPHICS
	IMaterialRegistry *materialRegistry;
	//SharedObject *storage = IStorageService(osgi.getService(IStorageService)).getStorage();
	//if (storage.data.textureDebug)
	//materialRegistry = new DebugMaterialRegistry();


	materialRegistry = new MaterialRegistry(osgi);

	//IBattleSettings *battleSettings = IBattleSettings(osgi.getService(IBattleSettings));
	materialRegistry->setUseMipMapping(true); //battleSettings.enableMipMapping;
	//osgi.registerService(IMaterialRegistry, materialRegistry);
	//osgi.registerService(IObjectPoolService, new ObjectPoolService());
	//osgi.registerService(ITankEventDispatcher, new TankEventDispatcher());

	Service::setIMaterialRegistry(materialRegistry);
#endif
}

void BattlefieldSharedActivator::stop(OSGi *osgi)
{
	(void)osgi;
	//osgi.unregisterService(IMaterialRegistry);
	//osgi.unregisterService(IObjectPoolService);
	//osgi.unregisterService(ITankEventDispatcher);
}
