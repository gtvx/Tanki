#include "texturebytedatamap.h"

TextureByteDataMap::TextureByteDataMap()
{

}

std::shared_ptr<TextureByteData> TextureByteDataMap::getValue(const QString &key)
{
	return this->_data[key];
}

void TextureByteDataMap::putValue(const QString &key, std::shared_ptr<TextureByteData> &value)
{
	this->_data[key] = value;
}
