#include "fadingdecalsrenderer.h"
#include "decalentry.h"
#include "alternativa/engine3d/objects/decal.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "gametime.h"



FadingDecalsRenderer::FadingDecalsRenderer(int fadeTime, BattlefieldModel *bs)
{
	this->fadeTime = fadeTime;
	this->battleService = bs;
	this->numDecals = 0;
}

void FadingDecalsRenderer::render(int param1)
{
	int i = 0;
	int numDecals = this->numDecals;
	int j = 0;

	while (j < numDecals)
	{
		std::shared_ptr<DecalEntry> decalEntry = this->entries.value(j);
		int time = param1 - decalEntry->startTime;

		if (time > this->fadeTime)
		{
			i++;
			this->battleService->removeDecal(decalEntry->decal.get());
			this->numDecals--;
		}
		else
		{
			decalEntry->decal->alpha = 1. - (double(time) / double(this->fadeTime));

			if (i > 0)
			{
				this->entries.insert(j - i, decalEntry);
			}
		}
		j++;
	}

	i = this->numDecals;
	while (i < numDecals)
	{
		this->entries.insert(i, nullptr);
		i++;
	}
}

void FadingDecalsRenderer::add(std::shared_ptr<Decal> &decal)
{
	this->entries.insert(this->numDecals, std::make_shared<DecalEntry>(decal, GetGameTime()));
	this->numDecals++;
}
