#ifndef DOMPOINTID_H
#define DOMPOINTID_H

#include "_global.h"
#include <stdint.h>

class QString;
class QByteArray;


#ifdef L_GT

class DomPointID
{
public:	
	char value;

	DomPointID() {}

	DomPointID(const DomPointID &t)
	{
		this->value = t.value;
	}

    void operator=(const DomPointID& d)
    {
        this->value = d.value;
    }

	DomPointID(const QByteArray &);

	inline bool operator==(const DomPointID& right) {
		return this->value == right.value;
	}

	QString toString() const;
};

#else



class DomPointID
{
public:

	int value;

	DomPointID() {}

	DomPointID(const DomPointID &t)
	{
		this->value = t.value;
	}

    void operator=(const DomPointID& d)
    {
        this->value = d.value;
    }

	DomPointID(int value)
	{
		this->value = value;
	}

	DomPointID(const QByteArray &);

	QString toString() const;
};

#endif


static inline bool operator==(const DomPointID& d1, const DomPointID& d2)
{
   return d1.value == d2.value;
}

static inline uint32_t qHash(const DomPointID& mc)
{
	return mc.value;
}

#endif // DOMPOINTID_H
