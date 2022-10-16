#ifndef FADINGDECALSRENDERER_H
#define FADINGDECALSRENDERER_H

#include <QHash>
#include <memory>

class DecalEntry;
class BattlefieldModel;
class Decal;

class FadingDecalsRenderer
{
	int fadeTime;
	int numDecals;
	QHash<int, std::shared_ptr<DecalEntry>> entries;

public:
	BattlefieldModel *battleService;
	FadingDecalsRenderer(int fadeTime, BattlefieldModel *bs);
	void render(int);
	void add(std::shared_ptr<Decal> &decal);
};

#endif // FADINGDECALSRENDERER_H
