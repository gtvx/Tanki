#include "hash.h"
#include <QCryptographicHash>


QByteArray Hash::md5(const QByteArray &data)
{
	return QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex();
}
