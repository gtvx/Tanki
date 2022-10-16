#ifndef USERMINESLIST_H
#define USERMINESLIST_H

class ProximityMine;

class UserMinesList
{
public:
	ProximityMine *head;
	ProximityMine *tail;

	UserMinesList();
	~UserMinesList();
	void addMine(ProximityMine *mine);
	void removeMine(ProximityMine *mine);
	void clearMines();
};

#endif // USERMINESLIST_H
