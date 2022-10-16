#include "directxgraphics.h"

DirectXGraphics::DirectXGraphics()
{

}

void DirectXGraphics::setDevice(IDirect3DDevice9Ex *device)
{
	this->_device = device;
}
