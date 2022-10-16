#ifndef OSGI_H
#define OSGI_H


class QString;
class Stage;
class DisplayObjectContainer;
class Array;
class Object;
class SharedObject;



class OSGi
{
	static OSGi *instance;

	//private var bundles:Dictionary;
	//private var _bundleList:Vector.<Bundle>;
	//private var services:Dictionary;
	//private var _serviceList:Vector.<Object>;

public:
	OSGi();


	static OSGi* init(bool debug,
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
					  Object *loaderParams);

	static OSGi* osgi()
	{
		return instance;
	}

};

#endif // OSGI_H
