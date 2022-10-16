#include "clientobjectuser.h"
#include <QString>

ClientObjectUser::ClientObjectUser(const QString &id, const QString &name) :
	ClientObject(id, nullptr, name, nullptr)
{
	tankData = nullptr;
}
