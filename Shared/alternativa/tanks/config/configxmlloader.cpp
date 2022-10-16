#include "configxmlloader.h"
#include "config.h"
#include "file.h"
#include "service.h"

#include <QDebug>

ConfigXMLLoader::ConfigXMLLoader(const QString &url, Config *config)
{
	this->url = url;
	this->config = config;
}

bool ConfigXMLLoader::run()
{
	//(Main.osgi.getService(IConsole) as IConsole).addLine(("start load map config from: " + this.url));
	//this->loader = new URLLoader();
	//this->loader.dataFormat = URLLoaderDataFormat.TEXT;
	//this->loader.addEventListener(Event.COMPLETE, this.onLoadingComplete);
	//this->loader.load(new URLRequest(this.url));
	//this.url

	QString filename = Service::getClientPath() + this->url;

	if (File::read(filename, this->config->json))
	{
		return true;
	}
	else
	{
		qDebug("error ConfigXMLLoader");
		return false;
	}
}


/*
void onLoadingComplete(event:Event)
{
	this->config.json = String(this.loader.data);
	this->loader = null;
	completeTask();
	(Main.osgi.getService(IConsole) as IConsole).addLine("loaded map config");
}
*/
