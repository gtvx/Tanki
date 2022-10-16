#include "dompointid.h"
#include <QString>


#ifdef L_GT

DomPointID::DomPointID(const QByteArray &v)
{
	if (v.length() == 1) {
		value = v.front();
	} else {
		value = 'A';
	}
}

QString DomPointID::toString() const
{
	return QString(QChar(value));
}

#else

DomPointID::DomPointID(const QByteArray &v)
{
	if (v.length() == 1) {
		char c = v.front();
		if (c >= '0' && c <= '9')
			value = c - '0';
		else
			value = 0;
	} else {
		value = 0;
	}
}

QString DomPointID::toString() const
{
	char c = value + '0';
	return QString(QChar(c));
}


#endif
