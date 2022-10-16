#ifndef CONTEXT3DPROFILE_H
#define CONTEXT3DPROFILE_H

enum class Context3DProfile
{
	BASELINE, //baseline Используйте стандартный профиль поддержки функций.
	BASELINE_CONSTRAINED, //baselineConstrained Используйте ограниченный профиль поддержки функций для ориентации на более старые графические процессоры. Этот профиль, прежде всего, предназначен для устройств, которые поддерживают только шейдеры уровня PS_2.0, как и серия Intel GMA 9xx.
	BASELINE_EXTENDED, //baselineExtended Используйте расширенный профиль поддержки функций для более новых графических процессоров, которые поддерживают текстуры повышенного размера. Такой профиль увеличивает максимальный размер двумерной текстуры и текстуры RectangleTexture до 4096x4096
	ENHANCED, // enhanced  Используйте расширенный профиль для графических процессоров с поддержкой AGAL4.
	STANDARD, //standard Используйте стандартный профиль для процессоров с поддержкой MRT, AGAL2 и плавающих текстур.
	STANDARD_CONSTRAINED, //standardConstrained Используйте стандартный профиль для графических процессоров с поддержкой AGAL2 и плавающих текстур.
	STANDARD_EXTENDED, //standardExtended Используйте стандартный расширенный профиль для графических процессоров с поддержкой AGAL3 и функции рисования экземпляров.
};

#endif // CONTEXT3DPROFILE_H