#include "resourcepath.h"
#include "file.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>


static QString resource_path;

static bool _init;

static void init()
{
	QByteArray data;

	QString p = "/files/resources/";
	QString filename = QDir::currentPath() + p + "resource_path.ini";

	bool b = false;

	if (QFile::exists(filename))
		b = File::read(filename, data);

	if (b == true && data.size() != 0)
	{
		QTextStream stream(&data);
		stream.setCodec("UTF-8");
		QString path;
		stream >> path;

		QDir dir(path);
		dir.makeAbsolute();
		::resource_path = dir.absolutePath() + "/";
	} else {
		::resource_path = QDir::currentPath() + p;
	}

	_init = true;
	//return QDir().exists(::dir);
}


QString ResourcePath::get()
{
	if (_init == false)
		init();

	return resource_path;
}
