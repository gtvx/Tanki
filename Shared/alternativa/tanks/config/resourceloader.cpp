#include "resourceloader.h"

ResourceLoader::ResourceLoader(const QString &name, Config *config)
{
	this->config = config;
	this->name = name;
}
