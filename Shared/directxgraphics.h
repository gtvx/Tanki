#ifndef DIRECTXGRAPHICS_H
#define DIRECTXGRAPHICS_H

#include <stdint.h>

class QString;
class QFont;
class IDirect3DDevice9Ex;

class DirectXGraphics
{
	IDirect3DDevice9Ex *_device;
public:
	DirectXGraphics();
	void setDevice(IDirect3DDevice9Ex*);
	IDirect3DDevice9Ex* getDevice() { return _device; }
};

#endif // DIRECTXGRAPHICS_H
