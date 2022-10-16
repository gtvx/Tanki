#include "listvector3.h"
#include "alternativa/math/vector3.h"

void ListVector3::new_alloc(int alloc)
{
	this->alloc = alloc;
	auto n = new Vector3[alloc];
	for (int i = 0; i < _length; i++)
		n[i].copy(&list[i]);
	delete []list;
	list = n;
}

ListVector3::ListVector3()
{
	alloc = 5;
	_length = 0;
	list = new Vector3[alloc];
}

ListVector3::~ListVector3()
{
	delete []list;
}

void ListVector3::add(const Vector3 *src)
{
	if (_length >= alloc)
	{
		new_alloc(alloc * 2);
	}
	list[_length].copy(src);
	_length++;
}

void ListVector3::resize(int new_length)
{
	if (new_length > alloc)
	{
		new_alloc(new_length);
	}
	_length = new_length;
}

void ListVector3::clear()
{
	_length = 0;
}
