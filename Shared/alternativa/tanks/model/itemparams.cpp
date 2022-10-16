#include "itemparams.h"


ItemParams::ItemParams(const QString &baseItemId,
					   const QString &description,
					   bool inventoryItem,
					   int itemIndex,
					   QVector<ItemPropertyValue> &itemProperties,
					   ItemTypeEnum itemType,
					   int modificationIndex,
					   const QString &name,
					   int nextModificationPrice,
					   void *nextModificationProperties,
					   int nextModificationRankId,
					   const QString &previewId,
					   int price,
					   int rankId,
					   QVector<std::shared_ptr<ModificationInfo>> modifications
					   )
{
	(void)nextModificationProperties;
	this->baseItemId = baseItemId;
	this->description = description;
	this->inventoryItem = inventoryItem;
	this->itemIndex = itemIndex;
	this->itemProperties = itemProperties;
	this->itemType = itemType;
	this->modificationIndex = modificationIndex;
	this->name = name;
	this->nextModificationPrice = nextModificationPrice;
	//this->nextModificationProperties = nextModificationProperties;
	this->nextModificationRankId = nextModificationRankId;
	this->previewId = previewId;
	this->price = price;
	this->rankId = rankId;
	this->modifications = modifications;
}
