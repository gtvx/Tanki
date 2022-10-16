#ifndef COMMAND_H
#define COMMAND_H

#include <QByteArray>
#include "CommandType.h"

class Command
{
	QByteArray const *data;
	int pos;

public:
	CommandType type;

	Command(const QByteArray *data)
	{
		this->pos = 0;
		this->data = data;
	}

	QByteArray readArgument()
	{
		QByteArray b;
		int i = data->indexOf(';', pos);

		if (i == -1)
			return readRest();

		b.setRawData(data->constData() + pos, i - pos);
		pos = i + 1;
		return b;
	}

	const QByteArray& readAll()
	{
		return *data;
	}

	QByteArray readRest()
	{
		return QByteArray::fromRawData(data->constData() + pos, data->size() - pos);
	}
};

#endif // COMMAND_H
