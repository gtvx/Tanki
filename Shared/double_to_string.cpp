#include "double_to_string.h"
#include <QString>

QString double_to_string(double value)
{
	QString s = QString::number(value, 'g', 16);
	if (!s.contains(QChar('.')))
		s.append(".0");
	return s;
}
