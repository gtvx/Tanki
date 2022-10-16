#include "network.h"
#include "service.h"
#include "commands/command.h"
#include "alternativa/tanks/model/usermodel.h"
#include "INetworkListener.h"
#include "_global.h"
#include "networkbytes.h"
#include "hash.h"
#include "IMain.h"
#include <QTcpSocket>
#include <QThread>
#include <QNetworkProxy>
#include <QTimer>
#include <QDebug>


//#define network_send_show
//#define network_recv_show



static void init(Network *network)
{
#ifndef L_PRO
	network->send("system;init_location;RU");
#endif
	new UserModel(network);
}

class AESEncrypterModel : public INetworkListener
{
	Network *network;
public:

	AESEncrypterModel(Network *network)
	{
		this->network = network;
	}

	virtual ~AESEncrypterModel()
	{

	}


	bool onData(Command *command)
	{
		if (command->type != CommandType::SYSTEM)
			return false;

		QString cmd = command->readArgument();

		if (cmd == "set_aes_data") {
			network->setAesData(NetworkBytesFrom(command->readRest()));

#ifndef L_PRO
			::init(network);
			//QThread::sleep(10);
			//network->close();
			network->removeListener(this);
			delete this;
#endif
		} else if (cmd == "init_locale") {

			QByteArray json = command->readArgument();
			(void)json;
			//int callbackId = command->readArgument().toInt();
			//network->sendSystemDependenciesLoaded(callbackId);

		} else if (cmd == "main_resources_loaded") {
#ifdef L_PRO
			::init(network);
#endif
		} else if (cmd == "load_resources") {

			QByteArray json = command->readArgument();
			int callbackId = command->readArgument().toInt();
			(void)json;
			network->sendSystemDependenciesLoaded(callbackId);

		} else if (cmd == "init_registration_model") {

		} else if (cmd == "showAlert") {

		}

		return true;
	}

};


class NetworkPrivate : public QObject
{
	Network *d;

	QTcpSocket socket;
	Settings const *settings;
	uint8_t private_key;
	uint8_t lastKey;
	bool _connected;
	bool _stop;
	QByteArray _read_data;
	QString result;
	QTimer timer_activ;


	void slotTimerActiv()
	{
		send("end~");
	}

	void connected()
	{
		//system("pause");
		qDebug("connected");
		Service::getIMain()->socketConnected();

#if (L_MT || L_GT)
		d->addListener(new AESEncrypterModel(d));
		lastKey = 2;
		send("2v|vwhp>jhwbdhvbgdwdend~"); //system;get_aes_data
#endif

#ifdef L_PRO
		d->addListener(new AESEncrypterModel(d));
		lastKey = 2;
		send("2v|vwhp>jhwbdhvbgdwd>UXend~"); //system;get_aes_data
#endif

#ifdef L_OT
		timer_activ.start(3 * 60000); //3 minutes
		lastKey = 158;
		::init(d);
#endif
	}



	void disconnected()
	{
		qDebug("disconnected");
		timer_activ.stop();
		Service::getIMain()->socketDisconnected();
		//QThread::sleep(3);
		//socket_connect();
	}


	void noConnection()
	{
		qDebug("no connection");
		Service::getIMain()->noConnection();
	}

	void slotStateChanged(QAbstractSocket::SocketState state)
	{
		switch (state)
		{
			case QAbstractSocket::ConnectedState:
				if (_connected == false) {
					_connected = true;
					_read_data.clear();
					connected();
				}
				break;

			case QAbstractSocket::UnconnectedState:
				if (_connected == true) {
					_connected = false;
					_read_data.clear();
					disconnected();
					if (_stop == false)
						_stop = true;
				} else {
					noConnection();
				}
				break;

			default:
				break;
		}
	}


	CommandType getType(const QByteArray &str)
	{
		if (str == "battle")
			return CommandType::BATTLE;
		else if (str == "auth")
			return CommandType::AUTH;
		else if (str == "registration")
			return CommandType::REGISTRATON;
		else if (str == "chat")
			return CommandType::CHAT;
		else if (str == "lobby")
			return CommandType::LOBBY;
		else if (str == "garage")
			return CommandType::GARAGE;
		else if (str == "ping")
			return CommandType::PING;
		else if (str == "lobby_chat")
			return CommandType::LOBBY_CHAT;
		else if (str == "system")
			return CommandType::SYSTEM;
		else
			return CommandType::UNKNOWN;
	}

	void slotReadyRead()
	{
		_read_data.append(socket.readAll());

		for (;;)
		{
#ifdef L_PRO
			int i = _read_data.indexOf(";end~");
#else
			int i = _read_data.indexOf("end~");
#endif


			if (i == -1)
				break;

			QByteArray data;
			data.setRawData(_read_data.constData(), i);
#ifdef network_recv_show
			qDebug().noquote() << "recv" << data;
#endif

			Command read_data(&data);
			QByteArray t = read_data.readArgument();

			read_data.type = getType(t);
			d->sendRequestToAllListeners(&read_data);

#ifdef L_PRO
			_read_data.remove(0, i + 5);
#else
			_read_data.remove(0, i + 4);
#endif


			if (_read_data.isEmpty())
				break;
		}
	}



	QString crypt(const QString &request)
	{
#if (L_MT || L_GT || L_PRO)
		int key = (this->lastKey + 1) % 9;
		if (key <= 0)
			key = 1;
#endif
#if L_OT
		int key = (this->lastKey * 2) % 9;
		if (key <= 0)
			key = 158;
#endif

		this->lastKey = key;

		int length = request.length();

		result.resize(length);

		const char16_t *in = (const char16_t*)request.utf16();
		char16_t *out = (char16_t*)result.utf16();

#ifdef L_OT
		int k = key;
#endif
#if (L_GT || L_MT || L_PRO)
		int k = key + private_key;
#endif

		for (int i = 0; i < length; i++)
		{
			*out = *in + k;
			in++;
			out++;
		}

		return QString::number(key) + result;
	}

	void send(const QByteArray &data)
	{
		socket.write(data);
		socket.flush();
	}


public:


	NetworkPrivate(Network *d, const Settings *settings)
	{
		this->d = d;
		this->settings = settings;

		private_key = 0;
		_connected = false;
		_stop = false;

#if 1
		QNetworkProxy proxy;
		//proxy.setHostName("127.0.0.1");
		//proxy.setPort(1234);
		proxy.setType(QNetworkProxy::NoProxy);
		socket.setProxy(proxy);
#endif
        connect(&timer_activ, &QTimer::timeout, this, &NetworkPrivate::slotTimerActiv);
        connect(&socket, &QIODevice::readyRead, this, &NetworkPrivate::slotReadyRead);
        connect(&socket, &QAbstractSocket::stateChanged, this, &NetworkPrivate::slotStateChanged);
	}

	~NetworkPrivate()
	{
        disconnect(&socket, &QIODevice::readyRead, this, &NetworkPrivate::slotReadyRead);
        disconnect(&socket, &QAbstractSocket::stateChanged, this, &NetworkPrivate::slotStateChanged);
	}


	void socketConnect()
	{
		if (_connected == true)
			return;
		socket.connectToHost(Service::getHost(), Service::getPort());
	}


	void socketClose()
	{
		if (_connected == false)
			return;
		socket.close();
	}

	void sendData(const QString &data)
	{
#ifdef network_send_show
		qDebug() << "send" << data;
#endif
		send(crypt(data).toUtf8() + "end~");
	}


	void setAesData(const QByteArray &data)
	{
		(void)data;
#if (L_MT || L_GT || L_PRO)
		QByteArray m = Hash::md5(data);

		//qDebug() << m;

		/*
		QString filename = "encrypts/" + m + ".swf";
		if (QFile::exists(filename)) {
			qDebug("exists");
		} else {
			qDebug("write");
			QFile file(filename);
			if (file.open(QIODevice::WriteOnly))
				file.write(data);
		}
		*/


		if (m == "8dd0ce3a2d70d540a655fabbb68a3d4d")
			private_key = 2;
		else if (m == "72e4f70e0f4b3741b037156cca10bfdf")
			private_key = 5;
		else if (m == "102d10b47526a101b1f96fe778a9a760")
			private_key = 7;
		else if (m == "859f33c3119665cde3cbf27f086427ac")
			private_key = 10;
		else if (m == "8aabe5b1e1635f17c99dab8c38679178")
			private_key = 12;
		else if (m == "aec19fd4b4f2a32e1f52dd3bb452b6ef")
			private_key = 13;
		else if (m == "58d035242d91e6b7cd6f56720cbc8167")
			private_key = 22;
		else if (m == "439c905f13fc7fb0bbef8c0980111588")
			private_key = 26;
		else if (m == "eb7f0ea4fc5dbd8d02369af6f5648ef3")
			private_key = 28;
		else if (m == "7ec1b047be1b86dc63f44cf80e9682ef")
			private_key = 32;
		else {
			qDebug() << "error" << m;
			socketClose();
		}


		//8dd0ce3a2d70d540a655fabbb68a3d4d 2
		//72e4f70e0f4b3741b037156cca10bfdf 5
		//102d10b47526a101b1f96fe778a9a760 7
		//859f33c3119665cde3cbf27f086427ac 10
		//8aabe5b1e1635f17c99dab8c38679178 12
		//aec19fd4b4f2a32e1f52dd3bb452b6ef 13
		//58d035242d91e6b7cd6f56720cbc8167 22
		//439c905f13fc7fb0bbef8c0980111588 26
		//eb7f0ea4fc5dbd8d02369af6f5648ef3 28
		//7ec1b047be1b86dc63f44cf80e9682ef 32
#endif
	}


	void acceptPackage(const QByteArray &data)
	{
		Command read_data(&data);
		QByteArray t = read_data.readArgument();
		read_data.type = getType(t);
		d->sendRequestToAllListeners(&read_data);
	}
};



Network::Network(const Settings *settings) :
	d(new NetworkPrivate(this, settings))
{
	this->settings = settings;
}

Network::~Network()
{
	delete d;
}

void Network::connect()
{
	d->socketConnect();
}

void Network::close()
{
	d->socketClose();
}


void Network::send(const QString &data)
{
	d->sendData(data);
}

void Network::sendSystemDependenciesLoaded(int id)
{
	if (id == 0)
	{
		qDebug("error sendSystemDependenciesLoaded");
		return;
	}
	this->send("system;dependencies_loaded;" + QString::number(id));
}



void Network::setAesData(const QByteArray &data)
{
	d->setAesData(data);
}

const Settings* Network::getSettings()
{
	return settings;
}

void Network::acceptPackage(const QByteArray &data)
{
	d->acceptPackage(data);
}

