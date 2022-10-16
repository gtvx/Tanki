#include "osgi.h"

OSGi* OSGi::instance;

OSGi::OSGi()
{
	//this.services = new Dictionary(false);
	//this.bundles = new Dictionary(false);
	//this._bundleList = new Vector.<Bundle>();
	//this._serviceList = new Vector.<Object>();
}

OSGi* OSGi::init(bool debug,
				 Stage *_stage,
				 DisplayObjectContainer *container,
				 const QString &server,
				 Array *ports,
				 const QString &proxyHost,
				 int proxyPort,
				 const QString &resources,
				 Object *log,
				 SharedObject *sharedObject,
				 const QString &language,
				 Object *loaderParams)
{
	(void)debug;
	(void)_stage;
	(void)container;
	(void)server;
	(void)ports;
	(void)proxyHost;
	(void)proxyPort;
	(void)resources;
	(void)log;
	(void)sharedObject;
	(void)language;
	(void)loaderParams;


	//container.focusRect = false;

	instance = new OSGi();

	/*
	instance.registerService(ILoaderParamsService, new LoaderParamsService(loaderParams));
	if (debug)
	{
		instance.registerService(IConsoleService, new ConsoleService(log));
		instance.registerService(IDebugService, new DebugService());
	}
	else
	{
		instance.registerService(IConsoleService, new DummyConsoleService());
	}
	instance.registerService(IFocusService, new FocusService(_stage));
	instance.registerService(IMainContainerService, new MainContainerService(_stage, container));
	instance.registerService(INetworkService, new NetworkService(server, ports, resources, proxyHost, proxyPort));
	instance.registerService(IStorageService, new StorageService(sharedObject));
	var dumpService:IDumpService = new DumpService(instance);
	instance.registerService(IDumpService, dumpService);
	instance.registerService(ILoaderService, new LoaderService());
	instance.registerService(ILocaleService, new LocaleService(language));
	dumpService.registerDumper(new BundleDumper(instance));
	dumpService.registerDumper(new ServiceDumper(instance));
	*/

	return instance;
}
