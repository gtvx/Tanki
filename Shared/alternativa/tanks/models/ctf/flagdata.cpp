#include "flagdata.h"

FlagData::FlagData(const QString &imageResourceId, const QString &pedestalModelId, const QString &pedestalModel_img)
{
	this->imageResourceId = imageResourceId;
	this->pedestalModelId = pedestalModelId;
	this->pedestalModel_img = pedestalModel_img;
}
