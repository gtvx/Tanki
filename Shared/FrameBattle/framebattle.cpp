#include "framebattle.h"
#include "ui_framebattle.h"
#include "networking/network.h"
#include "battlechatmessage.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "alternativa/tanks/models/battlefield/battleview3d.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "alternativa/physics/collision/primitives/collisionbox.h"
#include "alternativa/physics/collision/primitives/collisionrect.h"
#include "alternativa/physics/collision/primitives/collisiontriangle.h"
#include "alternativa/physics/physicsscene.h"
#include "alternativa/tanks/physics/tankscollisiondetector.h"
#include "alternativa/physics/collision/collisionkdtree.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/engine3d/core/face.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/physics/bodystate.h"
#include "alternativa/physics/collisionprimitivelist.h"
#include "alternativa/physics/collisionprimitivelistitem.h"
#include "alternativa/physics/contact.h"
#include "alternativa/physics/contactpoint.h"
#include "alternativa/physics/collision/collisionkdnode.h"
#include "alternativa/physics/collision/collisionkdtree.h"
#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/tanks/models/tank/tankmodel.h"
#include "com/alternativaplatform/projects/tanks/client/models/tank/TankSpawnState.h"
#include "projects/tanks/client/battleselect/types/BattleClient.h"
#include "alternativa/tanks/models/ctf/ctfmodel.h"
#include "alternativa/tanks/models/ctf/ctfflag.h"
#include "tank_bot/bot.h"
#include "tank_bot/botsettings.h"
#include "tank_bot/map_points.h"
#include "alternativa/tanks/models/battlefield/mine/proximitymine.h"
#include "alternativa/tanks/models/battlefield/mine/battleminesmodel.h"
#include "alternativa/tanks/models/effects/common/bonuscommon/parabonus.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "alternativa/engine3d/core/view.h"


#include "tanks.h"
#include "debug.h"
#include "settings.h"
#include "service.h"

#include <QScrollBar>
#include <QPainter>
#include <QResizeEvent>
#include <QDebug>


static QFont font_username("fantasy", 10, QFont::Normal);


class Damage
{
public:
	uint32_t color;
	int value;
	int c;
	int x;
	int y;
	int z;
};



static Tank* GetLocalTank()
{
	return TANKS::get_my_tank();
}



static void setBound(const Vector3 *hs, BoundBox *bound)
{
	bound->MinX = -hs->x;
	bound->MinY = -hs->y;
	bound->MinZ = -hs->z;
	bound->MaxX = hs->x;
	bound->MaxY = hs->y;
	bound->MaxZ = hs->z;
}


static void draw(Graphics *graphics, Camera3D *camera, Object3D *object, ulong color)
{
	object->matrix.setMatrix(&object->position, &object->rotation);
	object->matrix.append(&camera->matrix);

	//Debug_drawBounds(canvas, &object->bound, camera->viewSizeX, camera->viewSizeY, &object->matrix4, 0x000000);

	Mesh *mesh = (Mesh*)object;
	Face *face = mesh->faceList().get();

	while (face != nullptr) {
		Debug_draw_point(graphics, camera->viewSizeX, camera->viewSizeY, object->matrix, face->wrapper.get(), color);
		face = face->next.get();
	}
}


void drawTank(GameCamera *camera, Graphics *graphics, Tank *tank)
{
	TankSkin *skin = tank->skin;
	Mesh *mesh_hull = skin->hullMesh;
	Mesh *mesh_turret = skin->turretMesh;

	uint32_t color;

	TankSpawnState spawnState = tank->tankData->spawnState;
	if (spawnState == TankSpawnState::ACTIVE) {
		color = 0x4C99BA;
	} else if (spawnState == TankSpawnState::NEWCOME) {
		color = 0xFF0000;
	} else {
		color = 0x000000;
	}
	uint32_t color_team;

	BattleTeamType team = tank->tankData->teamType;
	switch (team)
	{
		case BattleTeamType::BLUE:
			color_team = 0x0000A0;
			break;
		case BattleTeamType::RED:
			color_team = 0xA00000;
			break;
		default:
			color_team = 0x00A000;
			break;
	}

	draw(graphics, camera, mesh_hull, color);
	draw(graphics, camera, mesh_turret, color_team);

	if (!tank->tankData->local)
	{
		Vector3 pos_name;
		Mesh *mesh = tank->skin->turretMesh;
		pos_name.x = mesh->position.x;
		pos_name.y = mesh->position.y;
		pos_name.z = mesh->position.z + 200.;
		int offset_x = -(tank->username_width / 2);
		int offset_y = 0;

		Debug_draw_string(graphics, camera->viewSizeX, camera->viewSizeY, camera->matrix, pos_name, offset_x, offset_y, *tank->tankData->userName, color_team, font_username);
	}

	//qDebug() << mesh_turret->x << mesh_turret->y << mesh_turret->z;
}


void draw(CollisionKdNode *node, GameCamera *camera, Graphics *graphics)
{
	if (node == nullptr)
		return;

	Debug_drawBounds(graphics, node->boundBox, camera->viewSizeX, camera->viewSizeY, camera->matrix, 0x00A000);

	draw(node->positiveNode, camera, graphics);
	draw(node->negativeNode, camera, graphics);
}





FrameBattle::FrameBattle(Network *network) :
	QFrame(nullptr),
	ui(new Ui::FrameBattle)
{
	ui->setupUi(this);
	this->network = network;

	battlefieldModel = nullptr;
	botSettings = nullptr;

	suicideIndicator = false;
	draw_enable = true;

	ui->textEdit_chat->setStyleSheet("background: rgba(0,0,255,20%)");
	ui->lineEdit_chat_enter->setStyleSheet("background: rgba(0,0,255,20%)");
	lock_hide_chat = false;
	connect(&timer_chat, &QTimer::timeout, this, &FrameBattle::slot_time_out);
	timer_chat.setSingleShot(true);
	timer_chat.setInterval(10000);
	hideAllChat();
	hideChat();
}

FrameBattle::~FrameBattle()
{
	delete ui;
}

void FrameBattle::sendMessage(const QString &msg, bool team)
{
	network->send("battle;chat;" + msg + ";" + (team ? "true" : "false"));
}

void FrameBattle::slot_time_out()
{
	if (!isFocusChat())
		hideAllChat();
}

void FrameBattle::on_lineEdit_chat_enter_returnPressed()
{
	QString text = ui->lineEdit_chat_enter->text();
	if (text.isEmpty()) {
		lock_hide_chat = false;
		return;
	} else {
		lock_hide_chat = true;
		sendMessage(text, false);
		ui->lineEdit_chat_enter->clear();
	}
	hideChat();
}

void FrameBattle::battleChatMessage(const BattleChatMessage &message)
{
	QScrollBar *sb = ui->textEdit_chat->verticalScrollBar();
	int s = sb->value();
	bool scroll_down = s == sb->maximum();

	QTextCursor cursor = ui->textEdit_chat->textCursor();
	cursor.movePosition(QTextCursor::End);


	QTextCharFormat fmt = ui->textEdit_chat->currentCharFormat();

	if (message.type == BattleChatMessage::TYPE::SPECTATOR) {
		fmt.setForeground(QBrush(Qt::yellow));
		ui->textEdit_chat->setCurrentCharFormat(fmt);
		ui->textEdit_chat->insertPlainText(message.text + "\n");
		showAllChat();
		timer_chat.start();
	} else if (message.type == BattleChatMessage::TYPE::SYSTEM) {
		fmt.setForeground(QBrush(Qt::darkGreen));
		ui->textEdit_chat->setCurrentCharFormat(fmt);
		ui->textEdit_chat->insertPlainText(message.text + "\n");
		showAllChat();
		timer_chat.start();
	} else if (message.type == BattleChatMessage::TYPE::USER) {
		QBrush brush;
		brush.setStyle(Qt::SolidPattern);
		if (message.team_type == BattleTeamType::NONE) {
			brush.setColor(QColor(0, 100, 0));
		} else if (message.team_type == BattleTeamType::RED) {
			brush.setColor(Qt::red);
		} else if (message.team_type == BattleTeamType::BLUE) {
			brush.setColor(Qt::blue);
		} else
			return;

		fmt.setForeground(brush);

		cursor.setCharFormat(fmt);
		cursor.insertText(message.nickname + ": ");
		fmt.setForeground(QBrush(Qt::black));
		cursor.setCharFormat(fmt);
		cursor.insertText(message.text + "\n");

		//ui->textEdit_chat->setCurrentCharFormat(fmt);
		//ui->textEdit_chat->insertPlainText(message.nickname + ": ");
		//fmt.setForeground(QBrush(Qt::black));
		//ui->textEdit_chat->setCurrentCharFormat(fmt);
		//ui->textEdit_chat->insertPlainText(message.text + "\n");
		showAllChat();
		timer_chat.start();
	}


	if (scroll_down == true)
		sb->setValue(sb->maximum());
	else
		sb->setValue(s);
}

void FrameBattle::showChat()
{
	ui->lineEdit_chat_enter->setEnabled(true);
	ui->lineEdit_chat_enter->show();
	ui->lineEdit_chat_enter->setFocus();
}

void FrameBattle::hideChat()
{
	ui->lineEdit_chat_enter->setEnabled(false);
	ui->lineEdit_chat_enter->hide();
}


void FrameBattle::showAllChat()
{
	ui->frame_chat->setEnabled(true);
	ui->frame_chat->show();
}

void FrameBattle::hideAllChat()
{
	ui->frame_chat->setEnabled(false);
	ui->frame_chat->hide();
	hideChat();
}

bool FrameBattle::isFocusChat()
{
	return ui->lineEdit_chat_enter->hasFocus();
}

void FrameBattle::enter()
{
	if (lock_hide_chat == true) {
		lock_hide_chat = false;
		return;
	}

	if (ui->lineEdit_chat_enter->isEnabled())
	{
		hideAllChat();
	}
	else
	{
		showAllChat();
		showChat();
	}
}

void FrameBattle::paintEvent(QPaintEvent *event)
{
	(void)event;

	if (!draw_enable)
		return;

	QPainter painter(this);


	graphics.setPainter(&painter);

	if (settings->antialiasing)
		painter.setRenderHint(QPainter::Antialiasing);


	int w = this->width();
	int h = this->height();

	BattlefieldModel *battlefieldModel = Service::getBattlefieldModel();
	if (battlefieldModel == nullptr)
		return;

	BattlefieldData *bfData = battlefieldModel->bfData;

	if (bfData == nullptr)
		return;

	if (bfData->viewport == nullptr)
		return;

	GameCamera *camera = bfData->viewport->camera;

	painter.setViewport(camera->viewSizeX, camera->viewSizeY, w, h);


#if 1

	TanksCollisionDetector *collisionDetector = (TanksCollisionDetector*)battlefieldModel->bfData->physicsScene->collisionDetector;
	if (collisionDetector == nullptr)
		return;

	QVector<CollisionPrimitive*> *staticChildren = collisionDetector->tree->staticChildren;
	if (staticChildren == nullptr)
		return;

	for (int i = 0; i < staticChildren->length(); i++)
	{
		CollisionPrimitive *primitive = staticChildren->at(i);

		if (primitive->type == CollisionPrimitive::BOX)  //Коробка
		{
			CollisionBox *box = (CollisionBox*)primitive;
			Matrix4 m;
			m.copy(&box->transform);
			m.append(&camera->matrix);
			BoundBox bound;
			setBound(&box->hs, &bound);
			Debug_drawBounds(&graphics, bound, camera->viewSizeX, camera->viewSizeY, m, 0xFF0000);
		} else if (primitive->type == CollisionPrimitive::RECT)  //Коробка
		{
			CollisionRect *rect = (CollisionRect*)primitive;
			Matrix4 m;
			m.copy(&rect->transform);
			BoundBox bound;
			setBound(&rect->hs, &bound);
			m.append(&camera->matrix);
			Debug_drawRect(&graphics, bound, camera->viewSizeX, camera->viewSizeY, m, 0xFF0000);
		} else if (primitive->type == CollisionPrimitive::TRIANGLE)  //Треугольник
		{
			CollisionTriangle *triangle = (CollisionTriangle*)primitive;
			Matrix4 m;
			m.copy(&triangle->transform);
			m.append(&camera->matrix);
			Debug_drawTriangle(&graphics, triangle->v0,
							   triangle->v1,
							   triangle->v2,
							   camera->viewSizeX, camera->viewSizeY, m, 0x000000); //black
		}
	}

	QHashIterator<TankData*, bool> i(*battlefieldModel->bfData->activeTanks);
	while (i.hasNext()) {
		i.next();
		TankData *tankdata = i.key();
		Tank *tank = tankdata->tank;
		drawTank(camera, &graphics, tank);

		/*
		CollisionPrimitiveListItem *item = tank->collisionPrimitives->head;
		while (item != nullptr)
		{
			CollisionPrimitive *primitive = item->primitive;
			CollisionBox *box = (CollisionBox*)primitive;
			//Matrix4 m = primitive->transform;
			//m.append(&camera->matrix);
			//qDebug() << primitive->aabb->maxX;
			BoundBox bound;
			setBound(&box->hs, &bound);
			Matrix4 m = box->transform;
			m.append(&camera->matrix);
			Debug_drawBounds(&canvas, &bound, camera->viewSizeX, camera->viewSizeY, &m, 0xFFFFFF);
			item = item->next;
		}
		*/
	}
#endif

	{
		QMutableVectorIterator<Damage*> i(damages);
		while (i.hasNext()) {
			Damage *d = i.next();
			if (--d->c < 0) {
				delete d;
				i.remove();
				continue;
			}
			d->z++;

			Vector3 pos;
			pos.x = d->x;
			pos.y = d->y;
			pos.z = d->z + 10;
			int offset_x = -2;
			int offset_y = 0;

			Debug_draw_string(&graphics, camera->viewSizeX, camera->viewSizeY, camera->matrix, pos, offset_x, offset_y, QString::number(d->value), d->color, font_username);
		}
	}

	{
		//TanksCollisionDetector *collisionDetector = (TanksCollisionDetector*)battlefieldModel->bfData->physicsScene->collisionDetector;
		//::draw(collisionDetector->tree->rootNode, camera, &graphics);
	}

	{
		CTFModel *ctfmodel = Service::getCTFModel();
		if (ctfmodel != nullptr)
		{
			{
				Object3D *o = ctfmodel->pedestal_red.get();
				Matrix4 m;
				m.copy(&o->matrix);;
				m.append(&camera->matrix);
				Debug_drawBounds(&graphics, o->bound, camera->viewSizeX, camera->viewSizeY, m, 0xFF0000);
			}
			{
				Object3D *o = ctfmodel->pedestal_blue.get();
				Matrix4 m;
				m.copy(&o->matrix);;
				m.append(&camera->matrix);
				Debug_drawBounds(&graphics, o->bound, camera->viewSizeX, camera->viewSizeY, m, 0x0000FF);
			}

			{
				CollisionBox *c = ctfmodel->flag_red->triggerCollisionPrimitive;
				BoundBox bound;
				setBound(&c->hs, &bound);
				Matrix4 m;
				m.copy(&c->transform);;
				m.append(&camera->matrix);
				Debug_drawBounds(&graphics, bound, camera->viewSizeX, camera->viewSizeY, m, 0xFF0000);
			}
			{
				CollisionBox *c = ctfmodel->flag_blue->triggerCollisionPrimitive;
				BoundBox bound;
				setBound(&c->hs, &bound);
				Matrix4 m;
				m.copy(&c->transform);;
				m.append(&camera->matrix);
				Debug_drawBounds(&graphics, bound, camera->viewSizeX, camera->viewSizeY, m, 0x0000FF);
			}
		}
	}


	if (bot != nullptr)
	{
		Tank *tank = GetLocalTank();

		if (botSettings != nullptr && botSettings->draw_points == true && tank != nullptr)
		{
			MapPoints *map = bot->getMapPoints();


			int32_t local_x = tank->state.pos.x;
			int32_t local_y = tank->state.pos.y;
			int32_t local_z = tank->state.pos.z;

			MapPoint *point = map->point_first;
			while (point != nullptr)
			{
				auto get_distance = [](int32_t ax, int32_t ay, int32_t az, int32_t bx, int32_t by, int32_t bz)
				{
					return abs(ax - bx) + abs(ay - by) + abs(az - bz);
					//auto pow2 = [](double value) { return value * value; };
					//return MyMath::sqrt(pow2(ax - bx) + pow2(ay - by) + pow2(az - bz));
				};

				uint32_t distance = get_distance(local_x, local_y, local_z, point->x, point->y, point->z);
				if (distance < 1000)
				{
					uint32_t _color = point->locked == true ? 0xFF0000 : 0x000000;

					Vector3 vector_start;
					vector_start.x = point->x;
					vector_start.y = point->y;
					vector_start.z = point->z;

					{
						Vector3 vector_stop;
						vector_stop.x = point->x;
						vector_stop.y = point->y;
						vector_stop.z = point->z + 30;
						Debug_draw_point(&graphics, camera->viewSizeX, camera->viewSizeY, camera->matrix, vector_start, vector_stop, _color);
					}


					if (point->XP != nullptr)
					{
						Vector3 vector_stop;
						vector_stop.x = point->x + 15;
						vector_stop.y = point->y;
						vector_stop.z = point->z;
						Debug_draw_point(&graphics, camera->viewSizeX, camera->viewSizeY, camera->matrix, vector_start, vector_stop, _color);
					}

					if (point->XM != nullptr)
					{
						Vector3 vector_stop;
						vector_stop.x = point->x - 15;
						vector_stop.y = point->y;
						vector_stop.z = point->z;
						Debug_draw_point(&graphics, camera->viewSizeX, camera->viewSizeY, camera->matrix, vector_start, vector_stop, _color);
					}

					if (point->YP != nullptr)
					{
						Vector3 vector_stop;
						vector_stop.x = point->x;
						vector_stop.y = point->y + 15;
						vector_stop.z = point->z;
						Debug_draw_point(&graphics, camera->viewSizeX, camera->viewSizeY, camera->matrix, vector_start, vector_stop, _color);
					}

					if (point->YM != nullptr)
					{
						Vector3 vector_stop;
						vector_stop.x = point->x;
						vector_stop.y = point->y - 15;
						vector_stop.z = point->z;
						Debug_draw_point(&graphics, camera->viewSizeX, camera->viewSizeY, camera->matrix, vector_start, vector_stop, _color);
					}

					if (point->XP_YP != nullptr)
					{
						Vector3 vector_stop;
						vector_stop.x = point->x + 15;
						vector_stop.y = point->y + 15;
						vector_stop.z = point->z;
						Debug_draw_point(&graphics, camera->viewSizeX, camera->viewSizeY, camera->matrix, vector_start, vector_stop, _color);
					}

					if (point->XP_YM != nullptr)
					{
						Vector3 vector_stop;
						vector_stop.x = point->x + 15;
						vector_stop.y = point->y - 15;
						vector_stop.z = point->z;
						Debug_draw_point(&graphics, camera->viewSizeX, camera->viewSizeY, camera->matrix, vector_start, vector_stop, _color);
					}

					if (point->XM_YP != nullptr)
					{
						Vector3 vector_stop;
						vector_stop.x = point->x - 15;
						vector_stop.y = point->y + 15;
						vector_stop.z = point->z;
						Debug_draw_point(&graphics, camera->viewSizeX, camera->viewSizeY, camera->matrix, vector_start, vector_stop, _color);
					}

					if (point->XM_YM != nullptr)
					{
						Vector3 vector_stop;
						vector_stop.x = point->x - 15;
						vector_stop.y = point->y - 15;
						vector_stop.z = point->z;
						Debug_draw_point(&graphics, camera->viewSizeX, camera->viewSizeY, camera->matrix, vector_start, vector_stop, _color);
					}
				}

				point = point->next;
			}
		}
	}


	{
		QHashIterator<QString, IBonus*> i(*bfData->bonuses);
		while (i.hasNext()) {
			i.next();
			ParaBonus *bonus = (ParaBonus*)i.value();
			Mesh *mesh = bonus->getMesh();
			mesh->matrix.setMatrix(&mesh->position, &mesh->rotation);
			Matrix4 m;
			m.copy(&mesh->matrix);;
			m.append(&camera->matrix);
			Debug_drawBounds(&graphics, mesh->bound, camera->viewSizeX, camera->viewSizeY, m, 0x0000FF);

			/*
			CollisionBox *c = bonus->getCollisionBox();
			BoundBox bound;
			setBound(&c->hs, &bound);
			Matrix4 m;
			m.copy(&c->transform);;
			m.append(&camera->matrix);
			Debug_drawBounds(&graphics, bound, camera->viewSizeX, camera->viewSizeY, m, 0x0000FF);
			*/
		}
	}

	{
		QHash<QString, ProximityMine*> *minesOnFieldById = battlefieldModel->battleMinesModel->minesOnFieldById;

		if (minesOnFieldById->count() != 0)
		{
			QHashIterator<QString, ProximityMine*> i(*minesOnFieldById);
			while (i.hasNext()) {
				i.next();

				ProximityMine *mine = i.value();

				{
					Vector3 vector_stop;
					vector_stop.x = mine->position.x;
					vector_stop.y = mine->position.y;
					vector_stop.z = mine->position.z + 30;
					Debug_draw_point(&graphics, camera->viewSizeX, camera->viewSizeY, camera->matrix,  mine->position, vector_stop, 0xFF0000);
				}
			}
		}
	}


	if (suicideIndicator == true)
	{
		if (suicideIndicatorAnimationUp == true) {
			if (suicideIndicatorAnimation >= 60) {
				suicideIndicatorAnimationUp = false;
			} else {
				suicideIndicatorAnimation += 3;
			}
		} else {
			if (suicideIndicatorAnimation <= 4) {
				suicideIndicatorAnimationUp = true;
			} else {
				suicideIndicatorAnimation -= 3;
			}
		}

		int size = suicideIndicatorAnimation;

		painter.setPen(QPen(0xFF0000));
		painter.drawEllipse(0-(size/2), 0-(size/2), size, size);
	}
}



void FrameBattle::createDamageEffect(Tank *tank, int damage, uint32_t color)
{
	if (draw_enable == false)
		return;

	Damage *d = new Damage;
	d->x = tank->state.pos.x;
	d->y = tank->state.pos.y;
	d->z = tank->state.pos.z;
	d->c = 50;
	d->color = color;
	d->value = damage;
	damages.append(d);
}


void FrameBattle::showSuicideIndicator()
{
	suicideIndicatorAnimation = 1;
	suicideIndicatorAnimationUp = true;
	suicideIndicator = true;
}

void FrameBattle::hideSuicideIndicator()
{
	suicideIndicator = false;
}

void FrameBattle::setDrawEnable(bool enable)
{
	draw_enable = enable;
}

void FrameBattle::setSettings(const Settings *settings)
{
	this->settings = settings;
}


void FrameBattle::initTank(Tank *tank)
{
	QFontMetrics fm(font_username);
	tank->username_width = fm.horizontalAdvance(*tank->tankData->userName);
}

void FrameBattle::setBot(Bot *bot)
{
	this->bot = bot;
	if (this->bot == nullptr) {
		this->botSettings = nullptr;
	} else {
		this->botSettings = bot->getBotSettings();
	}
}

void FrameBattle::draw()
{
	if (draw_enable == true)
		repaint();
}

void FrameBattle::getSize(int *width, int *height)
{
	*width = this->width();
	*height = this->height();
}

bool FrameBattle::init()
{
	battlefieldModel = Service::getBattlefieldModel();

	if (battlefieldModel == nullptr) {
		return false;
	}

	return true;
}

void FrameBattle::resizeEvent(QResizeEvent *event)
{
	QSize size = event->size();

	int width = size.width();
	int height = size.height();

	if (battlefieldModel != nullptr)
	{
		BattlefieldData *bfData = battlefieldModel->bfData;
		BattleView3D *viewport = bfData->viewport;
		GameCamera *camera = viewport->camera;

		camera->view->setSize(width, height);
	}
}
