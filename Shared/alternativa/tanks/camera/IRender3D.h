#ifndef IRENDER3D_H
#define IRENDER3D_H

class Device;
class Camera3D;

class IRender3D
{
public:
	virtual void render(Device*, Camera3D*) = 0;
};

#endif // IRENDER3D_H
