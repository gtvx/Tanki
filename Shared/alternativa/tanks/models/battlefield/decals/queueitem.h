#ifndef QUEUEITEM_H
#define QUEUEITEM_H


class QueueItem
{
	static QueueItem *poolTop;

public:
	QueueItem *next;
	void *data;

	QueueItem(void *data)
	{
		this->data = data;
	}

	static QueueItem* create(void *node)
	{
		if (poolTop == nullptr)
		{
			return new QueueItem(node);
		}

		QueueItem *item = poolTop;
		poolTop = poolTop->next;
		item->data = node;
		return item;
	}

	void destroy()
	{
		this->data = nullptr;
		this->next = poolTop;
		poolTop = this;
	}
};

#endif // QUEUEITEM_H
