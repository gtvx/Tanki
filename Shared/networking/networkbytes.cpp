#include <QByteArray>


static uint8_t atoi(const char *nptr, uint8_t *len)
{
	int result = 0;
	int position = 1;

	bool minus = false;

	char const *begin_num;

	if (nptr[0] == '-') {
		minus = true;
		begin_num = &nptr[1];
	} else {
		minus = false;
		begin_num = &nptr[0];
	}

	char const *c = begin_num;

	for (;;)
	{
		const char v = *c;
		if ((v < '0') || (v > '9'))
			break;
		c++;
	}

	char const *b = c;

	for (b--; b >= begin_num; b--)
	{
		result += position * (*b - '0');
		position *= 10;
	}

	*len = c - nptr;
	return minus == true ? -result : result;
}


QByteArray NetworkBytesFrom(const QByteArray &data)
{
	const char *data_begin = data.constData();
	const char *data_end = data.constData() + data.length();
	const char *current;

	int commas = 0;

	current = data_begin;
	while (current < data_end) {
		if (*current == ',')
			commas++;
		current++;
	}

	QByteArray result;
	result.reserve(commas + 1);

	current = data_begin;
	while (current < data_end) {
		uint8_t len;
		result.append(atoi(current, &len));
		current += len + 1;
	}

	return result;
}
