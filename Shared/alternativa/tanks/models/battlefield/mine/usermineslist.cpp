#include "usermineslist.h"
#include "proximitymine.h"


UserMinesList::UserMinesList()
{
	head = nullptr;
	tail = nullptr;
}

UserMinesList::~UserMinesList()
{

}

void UserMinesList::addMine(ProximityMine *mine)
{
	if (this->head == nullptr)
	{
		this->head = this->tail = mine;
	}
	else
	{
		this->tail->next = mine;
		mine->prev = this->tail;
		this->tail = mine;
	}
}

void UserMinesList::removeMine(ProximityMine *mine)
{
	if (this->head == nullptr)
		return;

	if (mine == this->head)
	{
		if (mine == this->tail)
		{
			this->head = this->tail = nullptr;
		}
		else
		{
			this->head = this->head->next;
			this->head->prev = nullptr;
		}
	}
	else
	{
		if (mine == this->tail)
		{
			this->tail = this->tail->prev;
			this->tail->next = nullptr;
		}
		else
		{
			mine->prev->next = mine->next;
			mine->next->prev = mine->prev;
		}
	}

	mine->dispose();
}

void UserMinesList::clearMines()
{
	while (this->head != nullptr)
	{
		this->removeMine(this->head);
	}
}
