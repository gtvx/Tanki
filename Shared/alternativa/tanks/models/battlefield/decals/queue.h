#ifndef QUEUE_H
#define QUEUE_H

#include "queueitem.h"

template<class T>
class Queue
{
	QueueItem *head;
	QueueItem *tail;
	int size;

public:
	Queue()
	{
		head = nullptr;
		tail = nullptr;
		size = 0;
	}


	void put(T* node)
	{
		this->size++;
		QueueItem *item = QueueItem::create((void*)node);
		if (this->tail == nullptr)
		{
			this->head = item;
			this->tail = item;
		}
		else
		{
			this->tail->next = item;
			this->tail = item;
		}
	}

	T* pop()
	{
		if (this->head == nullptr)
		{
			return nullptr;
		}

		this->size--;
		T* node = (T*)this->head->data;
		QueueItem *item = this->head;
		this->head = this->head->next;
		item->destroy();
		return node;
	}

	int getSize()
	{
		return this->size;
	}
};

#endif // QUEUE_H
