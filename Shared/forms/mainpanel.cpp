#include "mainpanel.h"
#include "controls/panel/mainpanelrank.h"


MainPanel::MainPanel(bool isTester)
{
	this->_isTester = isTester;
	rank = new MainPanelRank(0);

	//rangIcon = new RangIconNormal();
	//playerInfo = new PlayerInfo();
	//buttonBar = new ButtonBar();
}

void MainPanel::setRang(int value)
{
	this->_rang = value;
	this->rank->setValue(value);
	//this->playerInfo.rang = value;
	//this->rangIcon.rang1 = this->_rang;
}

int MainPanel::getRang()
{
	return this->_rang;
}

void MainPanel::configUI(Event *e)
{
	(void)e;
	//this->y = 3;
	//addChild(this->rangIcon);
	//addChild(this->playerInfo);
	//addChild(this->buttonBar);
	//this->rangIcon.y = -2;
	//this->rangIcon.x = 2;
	//removeEventListener(Event.ADDED_TO_STAGE, this->configUI);
	//this->playerInfo.indicators.changeButton.addEventListener(MouseEvent.CLICK, this->listClick);
	//this->buttonBar.addButton = this->playerInfo.indicators.changeButton;
	//stage.addEventListener(Event.RESIZE, this->onResize);
	this->onResize();
}

void MainPanel::listClick()
{
	//this->buttonBar.dispatchEvent(new MainButtonBarEvents(1));
}

void MainPanel::onResize()
{
	//int minWidth = int(Math.max(1000, stage.stageWidth));
	//this->buttonBar.x = ((minWidth - this->buttonBar.width) - 7);
	//this->playerInfo.width = (minWidth - this->buttonBar.width);
}

void MainPanel::hide()
{
	//stage.removeEventListener(Event.RESIZE, this->onResize);
}

bool MainPanel::getIsTester()
{
	return this->_isTester;
}

void MainPanel::setIsTester(bool value)
{
	this->_isTester = value;
	//this->buttonBar->isTester = this->_isTester;
	//this->buttonBar->draw();
	this->onResize();
}


void MainPanel::draw(Stage *stage)
{
	rank->draw(stage);
}
