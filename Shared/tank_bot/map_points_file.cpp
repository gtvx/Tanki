#include "map_points_file.h"
#include "map_points.h"
#include <QFile>
//#include <QDebug>

bool map_point_init(MapPoints *map);


namespace
{
	class StreamRead
	{
		uint8_t *data, *end;

	public:
		StreamRead(QByteArray &b)
		{
			init(b);
		}

		void init(QByteArray &b)
		{
			this->data = (uint8_t*)b.data();
			this->end = data + b.size();
		}

		uint8_t readUInt8()
		{
			uint8_t value = *(uint8_t*)this->data;
			data += sizeof(uint8_t);
			return value;
		}

		uint16_t readUInt16()
		{
			uint16_t value = *(uint16_t*)this->data;
			data += sizeof(uint16_t);
			return value;
		}

		uint32_t readUInt32()
		{
			uint32_t value = *(uint32_t*)this->data;
			data += sizeof(uint32_t);
			return value;
		}

		int32_t readInt32()
		{
			int32_t value = *(int32_t*)this->data;
			data += sizeof(int32_t);
			return value;
		}

		uint32_t readUInt29()
		{
			uint32_t value;
			uint8_t ch = this->readUInt8() & 0xFF;

			if (ch < 128){
				return ch;
			}

			value = (ch & 0x7F) << 7;
			ch = this->readUInt8() & 0xFF;
			if (ch < 128){
				return value | ch;
			}

			value = (value | (ch & 0x7F)) << 7;
			ch = this->readUInt8() & 0xFF;
			if (ch < 128){
				return value | ch;
			}

			value = (value | (ch & 0x7F)) << 8;
			ch = this->readUInt8() & 0xFF;
			return value | ch;
		}

		int32_t readInt29()
		{
			int32_t result = readUInt29();
			const int32_t maxPositiveInclusive = (1 << 28) - 1;
			if (result <= maxPositiveInclusive)
				return result;  // Postive number

			// Negative number. -x is stored as 2^29 - x
			const int32_t upperExclusiveBound = 1 << 29;
			return result - upperExclusiveBound;
		}

		QByteArray readAll()
		{
			return QByteArray((const char*)data, end - data);
		}
	};


	class StreamWrite
	{
		QByteArray data;

	public:

		void writeUInt8(uint8_t value)
		{
			data.append((char)value);
		}

		void writeUInt16(uint16_t value)
		{
			data.append((const char*)&value, sizeof(uint16_t));
		}

		void writeUInt32(uint32_t value)
		{
			data.append((const char*)&value, sizeof(uint32_t));
		}

		void writeInt32(int32_t value)
		{
			data.append((const char*)&value, sizeof(int32_t));
		}

		void writeUInt29(uint32_t value)
		{
			value &= 0x1fffffff;
			if (value < 0x80)
			{
				writeUInt8(value);
			}
			else if (value < 0x4000)
			{
				writeUInt8(((value >> 7) & 0x7F) | 0x80);
				writeUInt8((value & 0x7F));
			}
			else if (value < 0x200000)
			{
				writeUInt8(((value >> 14) & 0x7F) | 0x80);
				writeUInt8(((value >> 7) & 0x7F) | 0x80);
				writeUInt8((value & 0x7F));
			}
			else
			{
				writeUInt8(((value >> 22) & 0x7F) | 0x80);
				writeUInt8(((value >> 15) & 0x7F) | 0x80);
				writeUInt8(((value >> 8) & 0x7F) | 0x80);
				writeUInt8((value & 0xFF));
			}
		}

		void writeInt29(int32_t value)
		{
			const int32_t upperExclusiveBound = 1 << 29;
			if (value < 0)
				writeUInt29(value + upperExclusiveBound); // -x is stored as 2^29 - x
			else
				writeUInt29(value);
		}

		void addData(const QByteArray &data)
		{
			this->data.append(data);
		}

		QByteArray& getData()
		{
			return data;
		}
	};
}




bool map_points_load_file(const QString &filename, MapPoints *map)
{
	QFile file(filename);
	if (file.open(QIODevice::ReadOnly) == false) {
		return false;
	}

	QByteArray data = file.readAll();
	StreamRead stream(data);

	uint16_t type = stream.readUInt16();

	if (type != 0x1410) {
		return false;
	}

	uint16_t version = stream.readUInt16();

	map->length = stream.readUInt29();

	if (version != 3) {
		return false;
	}

	data = qUncompress(stream.readAll());
	stream.init(data);

	uint32_t index = 0;

	if (version == 3)
	{
		map->point_list = new MapPoint[map->length];
		map->point_first = map->point_list;
		map->point_end = &map->point_list[map->length-1];

		MapPoint *parent = nullptr;

		for (uint32_t i = 0; i < map->length; i++)
		{
			uint32_t ID_XP = stream.readUInt29();
			uint32_t ID_XM = stream.readUInt29();
			uint32_t ID_YP = stream.readUInt29();
			uint32_t ID_YM = stream.readUInt29();

			int32_t x = stream.readInt29();
			int32_t y = stream.readInt29();
			int32_t z = stream.readInt29();

			uint8_t diagonal = stream.readUInt8();

			MapPoint *point = &map->point_list[index];

			if (parent != nullptr)
				parent->next = point;

			index++;

			point->prev = parent;
			point->next = nullptr;

			point->INDEX = index;

			point->INDEX_XP = ID_XP;
			point->INDEX_XM = ID_XM;
			point->INDEX_YP = ID_YP;
			point->INDEX_YM = ID_YM;

			point->x  = x;
			point->y  = y;
			point->z  = z;

			point->diagonal = diagonal;

			parent = point;
		}
	}
	else
	{
		return false;
	}



	map_point_init(map);

	return true;
}


bool map_points_write_file(const QString &filename, MapPoints *map)
{
	StreamWrite stream;
	stream.writeUInt16(0x1410);
	stream.writeUInt16(3);

	stream.writeUInt29(map->length);


	StreamWrite stream2;
	MapPoint *point = map->point_first;
	while (point != nullptr)
	{
		stream2.writeUInt29(point->INDEX_XP);
		stream2.writeUInt29(point->INDEX_XM);
		stream2.writeUInt29(point->INDEX_YP);
		stream2.writeUInt29(point->INDEX_YM);
		stream2.writeInt29(point->x);
		stream2.writeInt29(point->y);
		stream2.writeInt29(point->z);
		stream2.writeUInt8(point->diagonal);
		point = point->next;
	}

	stream.addData(qCompress(stream2.getData()));

	QFile file(filename);
	if (file.open(QIODevice::WriteOnly) == false)
		return false;

	file.write(stream.getData());
	file.close();

	return true;
}
