#ifndef CLIENTFLAG_H
#define CLIENTFLAG_H

#include "alternativa/math/vector3.h"
#include <QString>

class ClientFlag
{
public:
	Vector3 flagBasePosition;
	Vector3 flagPosition;
	QString flagCarrierId;
};

#endif // CLIENTFLAG_H
