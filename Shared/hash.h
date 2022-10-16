#ifndef HASH_H
#define HASH_H

class QByteArray;

class Hash
{
public:	
	static QByteArray md5(const QByteArray &data);
};

#endif // HASH_H
