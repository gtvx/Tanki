#include "proplibloadingtask.h"
#include "alternativa/utils/taraparser.h"
#include "alternativa/proplib/proplibrary.h"
#include "alternativa/proplib/proplibregistry.h"
#include "file.h"
#include "service.h"
#include <QDebug>


PropLibLoadingTask::PropLibLoadingTask(const QString &url, std::shared_ptr<PropLibRegistry> &libRegistry)
{
	this->url = url;

	if (this->url.startsWith("resources/")) {
		this->url.remove(0, 10);
	}
	if (this->url.indexOf(QChar('?')) >= 0)
		this->url = this->url.split(QChar('?'))[0];

	this->libRegistry = libRegistry;
}

bool PropLibLoadingTask::run()
{
	//this.loader = new CacheURLLoader();
	//this.loader.dataFormat = URLLoaderDataFormat.BINARY;
	//this.loader.addEventListener(Event.COMPLETE, this.onLoadingComplete);
	//this.loader.load(new URLRequest(this.url));

	QString filename = Service::getResourcesPath() + this->url;

	QByteArray _data;

	if (File::read(filename, _data))
	{
		std::shared_ptr<ByteArrayMap> m = TARAParse(_data);
		if (m != nullptr) {
			std::shared_ptr<PropLibrary> propLibrary = std::make_shared<PropLibrary>(m);
			this->libRegistry->addLibrary(propLibrary);
			return true;
		}
	}
	else
	{
		qDebug() << "error load PropLibLoadingTask" << filename;
	}

	return false;
}


/*
void onLoadingComplete(event:Event)
{
	trace(("loaded prop: " + this.url));
	var propLibrary:PropLibrary = new PropLibrary(TARAParser.parse(this.loader.data));
	this.libRegistry.addLibrary(propLibrary);
	completeTask();
	this.loader = null;
	(Main.osgi.getService(IConsole) as IConsole).addLine(("loaded prop: " + this.url));
}
*/
