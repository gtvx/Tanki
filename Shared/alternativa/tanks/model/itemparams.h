#ifndef ITEMPARAMS_H
#define ITEMPARAMS_H

#include <QString>
#include <QVector>
#include <memory>
#include "com/alternativaplatform/projects/tanks/client/commons/models/itemtype/itemtypeenum.h"
#include "com/alternativaplatform/projects/tanks/client/garage/item/itempropertyvalue.h"
#include "com/alternativaplatform/projects/tanks/client/garage/item/modificationinfo.h"



class ItemParams
{
public:

	QString baseItemId;
	QString description;
	QString name;
	QString previewId;

	ItemTypeEnum itemType;

	int modificationIndex;
	int itemIndex;
	int nextModificationPrice;
	int nextModificationRankId;
	int price;
	int rankId;
	bool inventoryItem;


	QVector<ItemPropertyValue> itemProperties;
	QVector<std::shared_ptr<ModificationInfo>> modifications;
	//Array nextModificationProperties;

	ItemParams() {}

	ItemParams(const QString &baseItemId,
			   const QString &description,
			   bool inventoryItem,
			   int itemIndex,
			   QVector<ItemPropertyValue> &itemProperties,
			   ItemTypeEnum itemType,
			   int modificationIndex,
			   const QString &name,
			   int nextModificationPrice,
			   void* nextModificationProperties,
			   int nextModificationRankId,
			   const QString &previewId,
			   int price,
			   int rankId,
			   QVector<std::shared_ptr<ModificationInfo>> modifications
			   );
};

#endif // ITEMPARAMS_H
