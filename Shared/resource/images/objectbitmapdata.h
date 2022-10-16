#ifndef OBJECTBITMAPDATA_H
#define OBJECTBITMAPDATA_H

#include <QString>
#include <memory>

class BitmapData;

class ObjectBitmapData
{
public:

	std::shared_ptr<BitmapData> data;
	QString url;

	ObjectBitmapData();
};

#endif // OBJECTBITMAPDATA_H
