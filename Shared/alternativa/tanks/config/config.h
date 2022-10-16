#ifndef CONFIG_H
#define CONFIG_H

#include <QByteArray>
#include <memory>

class QString;
class TanksMap;
class PropLibRegistry;

class Config
{
	//TaskSequence *taskSequence;
public:
	TanksMap *map;
	QByteArray json;
	std::shared_ptr<PropLibRegistry> propLibRegistry;


	Config();
	~Config();
	void load(const QString &url, const QString &mapId);
};

#endif // CONFIG_H
