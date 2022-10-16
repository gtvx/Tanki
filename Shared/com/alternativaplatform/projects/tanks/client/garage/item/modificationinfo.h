#ifndef MODIFICATIONINFO_H
#define MODIFICATIONINFO_H

#include <QVector>
#include "itempropertyvalue.h"

class ModificationInfo
{
public:
	QString previewId;
	QVector<ItemPropertyValue> itemProperties;
	int crystalPrice;
	int rankId;
};

#endif // MODIFICATIONINFO_H
