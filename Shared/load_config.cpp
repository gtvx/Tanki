#include <QDomDocument>
#include <QJsonDocument>
#include <QJsonObject>
#include "service.h"
#include "_global.h"
#include "file.h"
#include "get_dir.h"

#ifdef WIN32
#include <Shlobj.h>
static QString GetPathFolder(int csidl)
{
	wchar_t buffer[4096];
	if (FAILED(::SHGetSpecialFolderPath(0, buffer, csidl, false)))
		return QString();
	return QString::fromWCharArray(buffer);
}

static QString Path(const QString &path)
{
	if(!path.startsWith("%"))
		return path;
	if (path.startsWith("%AppData%"))
		return GetPathFolder(CSIDL_APPDATA) + path.mid(9);
	if (path.startsWith("%LocalAppData%"))
		return GetPathFolder(CSIDL_LOCAL_APPDATA) + path.mid(14);

	return path;
}
#else
static QString Path(const QString &path)
{
    return path;
}
#endif

bool load_config()
{
	QByteArray data;
	if (!File::read(get_dir() + "/config.xml", data))
		return false;

	QDomDocument doc;
	if (doc.setContent(data) == false)
		return false;

	QDomElement xml = doc.documentElement();

	QString resources = xml.firstChildElement("resources").text();
	if (resources.isEmpty())
		return false;

	QString client = xml.firstChildElement("client_path").text();
	if (client.isEmpty())
		return false;

	Service::setResourcesPath(Path(resources));
	Service::setClientPath(Path(client));

	QString ip = xml.firstChildElement("ip").text();
	uint16_t port = xml.firstChildElement("port").text().toUInt();

	if (ip.isEmpty() || port == 0) {

		QString path_socket = Path(xml.firstChildElement("socket").text());
		if (path_socket.isEmpty())
			return false;

		QByteArray data;
		if (!File::read(path_socket, data))
			return false;

		QJsonDocument doc = QJsonDocument::fromJson(data);
		if (doc.isNull())
			return false;

		QJsonObject o = doc.object();
		ip = o.value("ip").toString();
		port = o.value("port").toInt();
	}

	if (ip.isEmpty() || port == 0) {
		return false;
	}

	Service::setHost(ip);
	Service::setPort(port);

	return true;
}

