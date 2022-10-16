#ifndef ROTATETURRETCOMMAND_H
#define ROTATETURRETCOMMAND_H

class RotateTurretCommand
{
public:

	double angle;
	int control;

	RotateTurretCommand(double angle, int control)
	{
		this->angle = angle;
		this->control = control;
	}
};

#endif // ROTATETURRETCOMMAND_H
