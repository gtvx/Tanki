#include "file.h"
#include "IMain.h"
#include "service.h"
#include <QFile>
#include <QDebug>

bool File::read(const QString &filename, QByteArray &data)
{
	QFile file(filename);

	if (file.open(QIODevice::ReadOnly))
	{
		data = file.readAll();
		//qDebug() << "File::read" << data.size() << filename;
		file.close();
		return true;
	}
	else
	{
		file.close();
		data.clear();

		qDebug() << "error File::read" << filename;

		IMain *main = Service::getIMain();
		if (main != nullptr)
			main->log("error open " + filename);

		return false;
	}
}

bool File::save(const QString &filename, const QByteArray &data)
{
	QFile file(filename);
	if (file.open(QIODevice::WriteOnly))
	{
		file.write(data);
		return true;
	}
	else
	{
		return false;
	}
}
