#include "proplibsloader.h"
#include "alternativa/proplib/proplibregistry.h"
#include "proplibloadingtask.h"
#include "config.h"
#include "tanksmap.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


PropLibsLoader::PropLibsLoader(Config *config) :
	ResourceLoader("Props library loader", config)
{
	this->libRegistry = std::make_shared<PropLibRegistry>();
	config->propLibRegistry = this->libRegistry;
}

bool PropLibsLoader::run()
{
	//this.sequence = new TaskSequence();
	//Object jsonParser = JSON.parse(config->json);
	QJsonDocument json_document = QJsonDocument::fromJson(config->json);

	if (json_document.isNull()) {
		qDebug("error json config");
		return false;
	}

	QString xml = config->map->getMapId() + ".xml";

	QJsonArray items = json_document.object().value("items").toArray();

	for (int i = 0; i < items.count(); i++)
	{
		QJsonObject item = items.at(i).toObject();
		QString name = item.value("name").toString();

		if (name == xml)
		{
			QJsonArray libs = item.value("libs").toArray();

			for (int i = 0; i < libs.count(); i++)
			{
				QString libPath = libs.at(i).toString();

				PropLibLoadingTask propLibLoadingTask(libPath, this->libRegistry);
				propLibLoadingTask.run();

				//var libPath:Object;
				//this.sequence.addTask(new PropLibLoadingTask((libPath as String), this.libRegistry));
				//trace(("load lib: " + libPath));
			}
		}
	}


	//this.sequence.addEventListener(Event.COMPLETE, this.onProplobsLoadingComplete);
	//this.sequence.run();

	return true;
}



/*
void onProplobsLoadingComplete(e:Event)
{
	this.sequence = null;
	config.propLibRegistry = this.libRegistry;
	completeTask();
}
*/
