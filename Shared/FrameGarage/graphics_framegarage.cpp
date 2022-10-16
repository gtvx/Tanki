#include "graphics_framegarage.h"
#include "ui_graphics_framegarage.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "alternativa/tanks/model/garagemodel.h"
#include "alternativa/tanks/gui/garagewindow.h"
#include "alternativa/tanks/model/tankpreview.h"
#include "alternativa/engine3d/core/view.h"
#include "flash/display/stage.h"
#include "flash/display/stage3d.h"

#include <QResizeEvent>





GraphicsFrameGarage::GraphicsFrameGarage(Network *network) :
	QFrame(nullptr),
	ui(new Ui::GraphicsFrameGarage)
{
	ui->setupUi(this);
	this->network = network;

	draw_enable = true;

	this->setUpdatesEnabled(false);
}

GraphicsFrameGarage::~GraphicsFrameGarage()
{
	timer.stop();

	GarageWindow *garageWindow = garageModel->garageWindow;
	TankPreview *tankPreview = garageWindow->tankPreview;

	tankPreview->dispose();
	if (stage != nullptr)
		delete stage;

	garageModel = nullptr;

	delete ui;
}


bool GraphicsFrameGarage::init(GarageModel *garageModel)
{
	HWND hwnd = (HWND)this->winId();

	this->garageModel = garageModel;

	GarageWindow *garageWindow = garageModel->garageWindow;
	TankPreview *tankPreview = garageWindow->tankPreview;

	GameCamera *camera = tankPreview->camera.get();

	this->stage = new Stage();

	Stage3D *stage3d = stage->stage3D();
	stage3d->setHWND(hwnd);

	camera->view->addToStage(stage);

	return true;
}

void GraphicsFrameGarage::setDrawEnable(bool enable)
{
	draw_enable = enable;
}

void GraphicsFrameGarage::setSettings(const Settings *settings)
{
	this->settings = settings;
}


void GraphicsFrameGarage::resizeEvent(QResizeEvent *event)
{
	QSize size = event->size();

	int width = size.width();
	int height = size.height();


	if (stage != nullptr)
	{
		stage->setStageSize(width, height);

		GarageWindow *garageWindow = garageModel->garageWindow;
		TankPreview *tankPreview = garageWindow->tankPreview;
		GameCamera *camera = tankPreview->camera.get();

		camera->view->setSize(width, height);
	}
}

void GraphicsFrameGarage::getSize(int *width, int *height)
{
	*width = this->width();
	*height = this->height();
}
