#ifndef DIRECTXVIEW_H
#define DIRECTXVIEW_H


class DirectXView
{
	void* _hwnd;
public:
	DirectXView();

	void setHWND(void* hwnd);
	void* getHWND();
};

#endif // DIRECTXVIEW_H
