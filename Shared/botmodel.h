#ifndef BOTMODEL_H
#define BOTMODEL_H

#include "_global.h"

class ClientObjectUser;
class QByteArray;
class QString;
class Vector3;

class BotModel
{
	void configureBot(ClientObjectUser *clientObject, Vector3 *targetPos);
public:

	BotModel();

	void addBot(const QByteArray &js);
	void removeBot(const QString &nickname);
	void destroy();
	bool playerIsBot(const QString &nickname);
	bool isBot(const QString &nickname);
};

#endif // BOTMODEL_H
