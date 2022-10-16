#ifndef TEXTUREBYTEDATAMAP_H
#define TEXTUREBYTEDATAMAP_H

#include <QHash>
#include <memory>

class TextureByteData;
class QString;


class TextureByteDataMap
{
	QHash<QString, std::shared_ptr<TextureByteData>> _data;
public:
	TextureByteDataMap();



	/*
	public function TextureByteDataMap(data:Object=null)
	{
		this._data = ((data == null) ? {} : data);
	}
	*/
	/*
	public function get data():Object
	{
		return (this._data);
	}
	*/

	std::shared_ptr<TextureByteData> getValue(const QString& key);

	void putValue(const QString& key, std::shared_ptr<TextureByteData> &value);

};

#endif // TEXTUREBYTEDATAMAP_H
