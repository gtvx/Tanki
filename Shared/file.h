#ifndef FILE_H
#define FILE_H

class QByteArray;
class QString;

class File
{
public:
	static bool read(const QString&, QByteArray &data);
	static bool save(const QString&, const QByteArray &data);
};

#endif // FILE_H
