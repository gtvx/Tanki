#ifndef CONFIGXMLLOADER_H
#define CONFIGXMLLOADER_H

class Config;

#include <QString>

class ConfigXMLLoader //Task
{
	Config *config;
	QString url;
	//URLLoader loader;

public:
	ConfigXMLLoader(const QString &url, Config *config);
	bool run();
};

#endif // CONFIGXMLLOADER_H
