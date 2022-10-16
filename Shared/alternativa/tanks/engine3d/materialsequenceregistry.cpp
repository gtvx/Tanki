#include "materialsequenceregistry.h"
#include "materialsequence.h"
#include "flash/display/bitmapdata.h"
#include <QTimer>


MaterialSequenceRegistry::MaterialSequenceRegistry(int timerInterval)
{
	o = new QObject;
	timer = new QTimer();
	QObject::connect(timer, QTimer::timeout, o, [this] {
		this->onTimer();
	});

	_useMipMapping = false;
	numScheduledSequences = 0;
	this->setTimerInterval(timerInterval);
}

MaterialSequenceRegistry::~MaterialSequenceRegistry()
{
	delete this->timer;
	delete o;
	this->clear();
}

void MaterialSequenceRegistry::setTimerInterval(int value)
{
	 this->timer->setInterval(value);
}

bool MaterialSequenceRegistry::getUseMipMapping()
{
	return this->_useMipMapping;
}

void MaterialSequenceRegistry::setUseMipMapping(bool value)
{
	 this->_useMipMapping = value;
}

MaterialSequence* MaterialSequenceRegistry::getSequence(MaterialType materialType,
														BitmapData *sourceImage,
														int frameWidth,
														double mipMapResolution,
														bool disposeSourceImage,
														bool createMirroredFrames)
{
	(void)materialType;

	MaterialSequence *sequence = this->sequenceByImage[sourceImage];
	if (sequence == nullptr)
	{
		sequence = new MaterialSequence(this, sourceImage, frameWidth, createMirroredFrames, disposeSourceImage, this->_useMipMapping, mipMapResolution);
		this->sequenceByImage[sourceImage] = sequence;
		this->scheduleSequence(sequence);
	}
	sequence->referenceCount++;
	return sequence;
}

MaterialSequence* MaterialSequenceRegistry::getSquareSequence(MaterialType materialType,
															  BitmapData *sourceImage,
															  double mipMapResolution,
															  bool disposeSourceImage,
															  bool createMirroredFrames)
{
	return this->getSequence(materialType, sourceImage, sourceImage->height(), mipMapResolution, disposeSourceImage, createMirroredFrames);
}

void MaterialSequenceRegistry::clear()
{
	for (;;)
	{
		QHash<BitmapData*, MaterialSequence*>::iterator i = this->sequenceByImage.begin();
		if (i == this->sequenceByImage.end())
			break;

		this->disposeSequence(i.value());
	}
}

QString MaterialSequenceRegistry::getDump()
{
	return "";
}

void MaterialSequenceRegistry::disposeSequence(MaterialSequence *sequence)
{
	this->sequenceByImage.remove(sequence->sourceImage);
	int index = this->scheduledSequences.indexOf(sequence);
	if (index != -1)
	{
		this->removeScheduledSequence(index);
	}
	sequence->dispose();
}

void MaterialSequenceRegistry::onTimer()
{
	for (int i = 0; i < this->numScheduledSequences; i++)
	{
		MaterialSequence *sequence = this->scheduledSequences[i];
		if (sequence->tick())
		{
			this->removeScheduledSequence(i);
			i--;
		}
	}
}

void MaterialSequenceRegistry::scheduleSequence(MaterialSequence *sequence)
{
	this->scheduledSequences.append(sequence);
	this->numScheduledSequences++;

	if (this->numScheduledSequences == 1)
	{
		this->timer->start();
	}
}


void MaterialSequenceRegistry::removeScheduledSequence(int index)
{
	this->scheduledSequences.removeAt(index);
	this->numScheduledSequences--;

	if (this->numScheduledSequences == 0)
	{
		this->timer->stop();
	}
}

