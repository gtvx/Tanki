#include "itemtypeenumfromint.h"

ItemTypeEnum ItemTypeEnumFromInt(int sct)
{
	return (ItemTypeEnum)sct;

	/*
	switch (sct)
	{
		case 1:
			return ItemTypeEnum::WEAPON;
		case 2:
			return ItemTypeEnum::ARMOR;
		case 3:
			return ItemTypeEnum::COLOR;
		case 4:
			return ItemTypeEnum::INVENTORY;
		case 5:
			return ItemTypeEnum::PLUGIN;
		case 6:
			return ItemTypeEnum::KIT;
	}
	*/
}
