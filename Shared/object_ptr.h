#ifndef OBJECT_PTR_H
#define OBJECT_PTR_H

#include <cstddef>


class object
{
	int __ref;
public:

	object()
	{
		__ref = 0;
	}

	virtual ~object()
	{

	}

	void ___add_ref()
	{
		__ref++;
	}

	int ___release()
	{
		return --__ref;
	}
};


template<typename _Tp>
class object_ptr
{
	object *m_ptr;
public:

	object_ptr()
	{
		m_ptr = nullptr;
	}

	object_ptr(const std::nullptr_t)
	{
		m_ptr = nullptr;
	}

	object_ptr(_Tp *other)
	{
		m_ptr = static_cast<object*>(other);
		if (m_ptr != nullptr)
			m_ptr->___add_ref();
	}

	object_ptr(const object_ptr<_Tp> &other)
	{
		if (other.m_ptr != nullptr)
		{
			other.m_ptr->___add_ref();
			m_ptr = other.m_ptr;
		}
		else
		{
			m_ptr = nullptr;
		}
	}

	~object_ptr()
	{
		if (m_ptr != nullptr)
		{
			if (!m_ptr->___release())
			{
				delete m_ptr;
			}
		}
	}

	_Tp* operator->() const
	{
		return (_Tp*)m_ptr;
	}

	_Tp* get()
	{
		return (_Tp*)m_ptr;
	}

	void operator=(const object_ptr<_Tp> &other)
	{
		if (m_ptr != nullptr)
		{
			if (!m_ptr->___release())
			{
				delete m_ptr;
			}
		}

		if (other.m_ptr != nullptr)
		{
			other.m_ptr->___add_ref();
			m_ptr = other.m_ptr;
		}
		else
		{
			m_ptr = nullptr;
		}
	}

	void operator=(_Tp *other)
	{
		if (m_ptr != nullptr)
		{
			if (!m_ptr->___release())
			{
				delete m_ptr;
			}
		}

		m_ptr = static_cast<object*>(other);

		if (m_ptr != nullptr)
		{
			m_ptr->___add_ref();
		}
	}

	void operator=(const std::nullptr_t)
	{
		if (m_ptr != nullptr)
		{
			if (!m_ptr->___release())
			{
				delete m_ptr;
			}
		}
		m_ptr = nullptr;
	}

	bool operator==(const std::nullptr_t)
	{
		return this->m_ptr == nullptr;
	}

	bool operator!=(const std::nullptr_t)
	{
		return this->m_ptr != nullptr;
	}

	bool operator==(const object_ptr<_Tp> &other)
	{
		return this->m_ptr == other.m_ptr;
	}

	bool operator!=(const object_ptr<_Tp> &other)
	{
		return this->m_ptr != other.m_ptr;
	}

	template<typename _tsrc>
	object_ptr<_tsrc> convert()
	{
		if (m_ptr != nullptr)
		{
			m_ptr->___add_ref();
			return object_ptr<_tsrc>(static_cast<_tsrc*>(m_ptr));
		}
		else
		{
			return object_ptr<_tsrc>(nullptr);
		}
	}
};

template<typename _Tp, typename... _Args>
object_ptr<_Tp> new_object(_Args... args)
{
	return object_ptr<_Tp>(new _Tp(args...));
}

template<typename _Tp>
object_ptr<_Tp> new_object()
{
	return object_ptr<_Tp>(new _Tp());
}


#endif // OBJECT_PTR_H
