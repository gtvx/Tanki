#ifndef MAPCLIENT_H
#define MAPCLIENT_H

#include <QString>

class MapClient
{
public:
	QString previewId;
	QString id;
	QString name;
	QString gameName;
	QString themeName;

	int maxPeople;
	int maxRank;
	int minRank;

	bool ctf;
	bool tdm;
	bool dom;
	bool hr;
};

#endif // MAPCLIENT_H
