#include "directxview.h"

DirectXView::DirectXView()
{
	_hwnd = nullptr;
}

void DirectXView::setHWND(void *hwnd)
{
	this->_hwnd = hwnd;
}

void *DirectXView::getHWND()
{
	return this->_hwnd;
}
