#ifndef USERMODEL_H
#define USERMODEL_H

#include "networking/INetworkListener.h"

class QString;
class Network;

class UserModel : public INetworkListener
{
	Network *network;
public:
	UserModel(Network *network);
	~UserModel();

	bool onData(Command *data);
	void auth(const QString &mail, const QString &password, const QString &captcha);
};

#endif // USERMODEL_H
