#ifndef LISTVECTOR3_H
#define LISTVECTOR3_H

#include "alternativa/math/vector3.h"

class ListVector3
{
	int alloc;
	int _length;
	Vector3 *list;

	void new_alloc(int);

public:
	ListVector3();
	~ListVector3();

	void add(const Vector3 *str);
	void clear();
	void resize(int new_length);

	int length() const
	{
		return _length;
	}

	Vector3* at(int i)
	{
		return &list[i];
	}

	Vector3* last()
	{
		return &list[_length-1];
	}
};

#endif // LISTVECTOR3_H
