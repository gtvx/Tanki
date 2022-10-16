#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H

#include <QString>

class Config;

class ResourceLoader //Task
{
public:
	Config *config;
	QString name;

	ResourceLoader(const QString &name, Config *config);
};

#endif // RESOURCELOADER_H
