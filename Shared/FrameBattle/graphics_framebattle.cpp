#include "graphics_framebattle.h"
#include "ui_graphics_framebattle.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "alternativa/tanks/models/battlefield/battleview3d.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "alternativa/engine3d/core/view.h"
#include "flash/display/stage.h"
#include "flash/display/stage3d.h"
#include "service.h"

#include <QResizeEvent>



GraphicsFrameBattle::GraphicsFrameBattle(Network *network) :
	QFrame(nullptr),
	ui(new Ui::GraphicsFrameBattle)
{
	ui->setupUi(this);
	this->network = network;

	draw_enable = true;
	this->setUpdatesEnabled(false);
	battlefieldModel = nullptr;
	stage = nullptr;
}

GraphicsFrameBattle::~GraphicsFrameBattle()
{
	if (stage != nullptr)
		delete stage;
	delete ui;
}

bool GraphicsFrameBattle::init()
{
	HWND hwnd = (HWND)this->winId();


	this->stage = new Stage();

	Stage3D *stage3d = stage->stage3D();
	stage3d->setHWND(hwnd);

	battlefieldModel = Service::getBattlefieldModel();

	if (battlefieldModel == nullptr) {
		return false;
	}

	BattlefieldData *bfData = battlefieldModel->bfData;
	BattleView3D *viewport = bfData->viewport;
	viewport->setStage(this->stage);

	GameCamera *camera = viewport->camera;
	camera->view->addToStage(stage);

	return true;
}


void GraphicsFrameBattle::setDrawEnable(bool enable)
{
	draw_enable = enable;
}

void GraphicsFrameBattle::setSettings(const Settings *settings)
{
	this->settings = settings;
}


void GraphicsFrameBattle::initTank(Tank *tank)
{
	(void)tank;
}

void GraphicsFrameBattle::setBot(Bot *bot)
{
	(void)bot;
}

void GraphicsFrameBattle::draw()
{

}

void GraphicsFrameBattle::resizeEvent(QResizeEvent *event)
{
	QSize size = event->size();

	int width = size.width();
	int height = size.height();

	if (stage != nullptr && battlefieldModel != nullptr)
	{
		stage->setStageSize(width, height);
		BattlefieldData *bfData = battlefieldModel->bfData;
		BattleView3D *viewport = bfData->viewport;
		GameCamera *camera = viewport->camera;
		camera->view->setSize(width, height);
	}
}

void GraphicsFrameBattle::getSize(int *width, int *height)
{
	*width = this->width();
	*height = this->height();
}

void GraphicsFrameBattle::paintEvent(QPaintEvent *)
{

}
