#include "config.h"
#include "tanksmap.h"
#include "alternativa/proplib/proplibregistry.h"
#include "alternativa/tanks/config/proplibsloader.h"
#include "alternativa/tanks/config/configxmlloader.h"
#include "_global.h"


Config::Config()
{
	map = nullptr;
}

Config::~Config()
{
	if (map != nullptr)
		delete map;
}

void Config::load(const QString &url, const QString &mapId)
{
	(void)url;

	this->map = new TanksMap(this, mapId);
	ConfigXMLLoader configXMLLoader(url, this);
#ifdef GRAPHICS
	PropLibsLoader propLibsLoader(this);
#endif
	//this->taskSequence = new TaskSequence();
	//this->taskSequence.addTask(new ConfigXMLLoader(url, this));
	//this->taskSequence.addTask(new PropLibsLoader(this));
	//this->map = new TanksMap(this, mapId);
	//this->taskSequence.addTask(this.map);
	//this->taskSequence.addEventListener(Event.COMPLETE, this.onSequenceComplete);
	//this->taskSequence.run();


	configXMLLoader.run();
#ifdef GRAPHICS
	propLibsLoader.run();
#endif
	this->map->run();
}

