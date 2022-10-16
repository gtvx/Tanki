#ifndef CLIENTOBJECT_H
#define CLIENTOBJECT_H

class QString;
class ICommandHandler;
class ClientClass;



class ClientObject
{
	QString *_id;
	QString *_name;
	ClientClass *_clientClass;
public:
	ClientObject(const QString &id, ClientClass *clientClass, const QString &name, ICommandHandler *handler);
	virtual ~ClientObject();
	QString getId();
};


#endif // CLIENTOBJECT_H
