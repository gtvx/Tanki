#include "indicators.h"

Indicators::Indicators()
{
	//this->b_left = (getChildByName("L") as MovieClip);
	//this->b_name = (getChildByName("N") as MovieClip);
	//this->b_conn1 = (getChildByName("C1") as MovieClip);
	//this->b_crystal = (getChildByName("CR") as MovieClip);
	//this->b_right = (getChildByName("R") as MovieClip);
	//this->kd_icon = (getChildByName("kdicon") as MovieClip);
	//this->changeButton = (getChildByName("_changeButton") as BaseButton);
	//addChild(this->progressBar);
	//addChild(this->playerInfo);
	//addChild(this->scoreLabel);
	//addChild(this->kdRatio);
	//addChild(this->crystalInfo);
	//this->progressBar.x = 5;
	//this->progressBar.y = 5;
	//this->playerInfo.x = 6;
	//this->playerInfo.autoSize = TextFieldAutoSize.NONE;
	//this->scoreLabel.autoSize = TextFieldAutoSize.NONE;
	//this->scoreLabel.align = TextFormatAlign.RIGHT;
	//this->scoreLabel.width = 200;
	//this->scoreLabel.size = (this->kdRatio.size = (this->playerInfo.size = (this->crystalInfo.size = 12)));
	//this->scoreLabel.color = (this->kdRatio.color = (this->playerInfo.color = (this->crystalInfo.color = 0x12FF00)));
	//this->scoreLabel.y = (this->kdRatio.y = (this->playerInfo.y = (this->crystalInfo.y = 4)));
	//this->scoreLabel.height = (this->crystalInfo.height = (this->playerInfo.height = 18));
	//this->crystalInfo.align = TextFormatAlign.RIGHT;
	//this->crystalInfo.autoSize = TextFieldAutoSize.NONE;
	//this->crystalInfo.width = 59;
	//this->redZone = new Sprite();
	//this->timer = new Timer(50);
	//this->timer.addEventListener(TimerEvent.TIMER, this->hideRedZone);
	//this->progressBar.addChild(this->redZone);
	//this->redZone.alpha = 0;
}

int Indicators::getProgress()
{
	return this->_progress;
}

void Indicators::setProgress(int progress)
{
	//var _local_2:Graphics = this->progressBar.graphics;
	//var _local_3:int = (this->b_name.width + 4);
	this->_progress = progress;
	//_local_2.clear();
	//_local_2.beginFill(0x12FF00, 0.2);
	//_local_2.drawRect(0, 0, ((_local_3 / 10000) * this->_progress), 16);
	//_local_2.endFill();
}

void Indicators::setNewProgress(int nprogress)
{
	this->_nprogress = nprogress;
	this->_oprogress = this->_progress;
	//var _local_2:Graphics = this->redZone.graphics;
	//var _local_3:int = (this->b_name.width + 4);
	//this->redZone.alpha = 1;
	//_local_2.clear();
	//if (this->_nprogress >= this->_oprogress)
	//{
	//	_local_2.beginFill(0xFFCC00, 1);
	//	_local_2.drawRect(((_local_3 / 10000) * this->_oprogress), 0, ((_local_3 / 10000) * (nprogress - this->_oprogress)), 16);
	//	_local_2.endFill();
	//}
	//else
	//{
	//	_local_2.beginFill(0xFFCC00, 1);
	//	_local_2.drawRect(0, 0, _local_3, 16);
	//	_local_2.endFill();
	//}
	//this->timer.stop();
	//this->timer.start();
}

void Indicators::hideRedZone(TimerEvent*)
{
	//var _local_2:DisplayObject = this->redZone;
	//_local_2.alpha = (_local_2.alpha - 0.03);
	//if (_local_2.alpha <= 0)
	//{
	//	this->timer.stop();
	//}
}

void Indicators::setWidth(double width)
{
	//var _local_2:int;
	this->_width = width;
	//_local_2 = int((this->_width - 180));
	//this->b_name.width = _local_2;
	//this->b_conn1.x = (this->b_name.width + this->b_left.width);
	//this->b_crystal.width = 67;
	//this->b_crystal.x = (this->b_conn1.x + this->b_conn1.width);
	//this->b_right.x = (this->b_crystal.x + this->b_crystal.width);
	//this->changeButton.x = (this->b_right.x + 9);
	//this->crystalInfo.x = (this->b_crystal.x - 2);
	//this->kdRatio.x = int(((this->b_conn1.x - this->kdRatio.textWidth) - 3));
	//this->kd_icon.x = int(((this->b_conn1.x - this->kdRatio.textWidth) - 10));
	//this->scoreLabel.x = (this->kd_icon.x - 205);
	//this->playerInfo.width = (((this->b_name.width - this->kdRatio.width) - this->kd_icon.width) - 5);
	this->setProgress(this->_progress);
}
